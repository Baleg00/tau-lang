/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/symtable.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "utils/common.h"
#include "utils/hash.h"
#include "utils/memory/memtrace.h"

/// The initial number of buckets in a symbol table.
#define SYMTABLE_INITIAL_CAPACITY ((size_t)16)

/// The load factor threshold for symbol table resizing.
#define SYMTABLE_LOAD_FACTOR 0.75

/**
 * \brief Inserts a symbol into a symbol table without checking the
 * load factor and expanding the table's capacity.
 *
 * \param[int,out] table Pointer to the symbol table to insert into.
 * \param[int] new_sym Pointer to the symbol to be inserted.
 * \returns NULL if insertion was successful, otherwise a pointer to the
 * colliding symbol.
 */
static symbol_t* symtable_insert_no_expand(symtable_t* table, symbol_t* new_sym)
{
  size_t h = (size_t)hash_digest(new_sym->id, new_sym->len);
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    table->size++;
    table->buckets[idx] = new_sym;
    new_sym->next = NULL;
    new_sym->parent = table;

    return NULL;
  }

  symbol_t* last = NULL;

  for (symbol_t* sym = table->buckets[idx]; sym != NULL; last = sym, sym = sym->next)
    if (sym->len == new_sym->len && strncmp(sym->id, new_sym->id, sym->len) == 0)
      return sym;

  ASSERT(last != NULL);

  table->size++;

  last->next = new_sym;
  new_sym->next = NULL;
  new_sym->parent = table;

  return NULL;
}

/**
 * \brief Expands the capacity of a symbol table to accommodate more symbols.
 *
 * \param[in,out] table Pointer to the symbol table to be expanded.
 * \param[in] new_capacity The new capacity of the symbol table.
 */
static void symtable_expand(symtable_t* table, size_t new_capacity)
{
  if (new_capacity <= table->capacity)
    return;

  size_t symbol_count = table->size;

  symbol_t** symbols = (symbol_t**)malloc(symbol_count * sizeof(symbol_t*));
  ASSERT(symbols != NULL);

  for (size_t i = 0, j = 0; i < table->capacity && j < symbol_count; ++i)
    for (symbol_t* sym = table->buckets[i]; sym != NULL; sym = sym->next, ++j)
      symbols[j] = sym;

  table->size = 0;
  table->capacity = new_capacity;

  table->buckets = (symbol_t**)realloc(table->buckets, table->capacity * sizeof(symbol_t*));
  ASSERT(table->buckets != NULL);

  memset(table->buckets, 0, table->capacity * sizeof(symbol_t*));

  for (size_t i = 0; i < symbol_count; ++i)
    symtable_insert_no_expand(table, symbols[i]);

  free(symbols);
}

symbol_t* symbol_init(const char* id, size_t len, ast_node_t* node)
{
  symbol_t* sym = (symbol_t*)malloc(sizeof(symbol_t));
  ASSERT(sym != NULL);

  sym->parent = NULL;
  sym->id = id;
  sym->len = len;
  sym->node = node;

  return sym;
}

symbol_t* symbol_init_with_str_view(string_view_t id, ast_node_t* node)
{
  return symbol_init(id.buf, id.len, node);
}

void symbol_free(symbol_t* sym)
{
  free(sym);
}

symtable_t* symtable_init(symtable_t* parent)
{
  symtable_t* table = (symtable_t*)malloc(sizeof(symtable_t));
  ASSERT(table != NULL);

  if (parent != NULL)
    vector_push(parent->children, table);

  table->parent = parent;
  table->children = vector_init();
  table->size = 0;
  table->capacity = SYMTABLE_INITIAL_CAPACITY;
  table->buckets = (symbol_t**)calloc(table->capacity, sizeof(symbol_t*));
  ASSERT(table->buckets != NULL);

  return table;
}

void symtable_free(symtable_t* table)
{
  vector_for_each(table->children, (vector_for_each_func_t)symtable_free);
  vector_free(table->children);

  for (size_t i = 0, j = 0; i < table->capacity && j < table->size; ++i)
    for (symbol_t *next, *sym = table->buckets[i]; sym != NULL; sym = next, ++j)
    {
      next = sym->next;
      symbol_free(sym);
    }

  free(table->buckets);
  free(table);
}

symbol_t* symtable_insert(symtable_t* table, symbol_t* new_sym)
{
  if ((double)table->size + 1 >= SYMTABLE_LOAD_FACTOR * (double)table->capacity)
    symtable_expand(table, table->capacity << 1);

  return symtable_insert_no_expand(table, new_sym);
}

symbol_t* symtable_get(symtable_t* table, const char* id, size_t len)
{
  size_t h = (size_t)hash_digest(id, len);
  size_t idx = h % table->capacity;

  for (symbol_t* sym = table->buckets[idx]; sym != NULL; sym = sym->next)
    if (sym->len == len && strncmp(sym->id, id, sym->len) == 0)
      return sym;

  return NULL;
}

symbol_t* symtable_get_with_str_view(symtable_t* table, string_view_t id)
{
  return symtable_get(table, id.buf, id.len);
}

symbol_t* symtable_lookup(symtable_t* table, const char* id, size_t len)
{
  while (table != NULL)
  {
    symbol_t* sym = symtable_get(table, id, len);

    if (sym != NULL)
      return sym;

    table = table->parent;
  }

  return NULL;
}

symbol_t* symtable_lookup_with_str_view(symtable_t* table, string_view_t id)
{
  return symtable_lookup(table, id.buf, id.len);
}

void symtable_merge(symtable_t* dest, symtable_t* src)
{
  size_t new_capacity = dest->capacity;

  while ((double)dest->size + (double)src->size >= SYMTABLE_LOAD_FACTOR * (double)new_capacity)
    new_capacity <<= 1;

  if (dest->capacity < new_capacity)
    symtable_expand(dest, new_capacity);

  for (size_t i = 0, j = 0; i < src->capacity && j < src->size; ++i)
    for (symbol_t* sym = src->buckets[i], *next = NULL; sym != NULL; sym = next, ++j)
    {
      next = sym->next;

      symtable_insert_no_expand(dest, sym);
    }

  src->size = 0;
  memset(src->buckets, 0, sizeof(symbol_t*) * src->capacity);

  vector_extend(dest->children, src->children);

  for (size_t i = 0; i < vector_size(src->children); ++i)
  {
    symtable_t* child = (symtable_t*)vector_get(src->children, i);

    child->parent = dest;
  }

  vector_clear(src->children);

  symtable_free(src);
}
