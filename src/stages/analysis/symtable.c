/**
 * \file symtable.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/symtable.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stages/analysis/typedesc.h"
#include "utils/common.h"
#include "utils/hash.h"
#include "utils/memory/memtrace.h"

/**
 * \brief The initial capacity of a symbol table.
 *
 * \details This macro defines the initial number of buckets in a symbol table's
 * hash table when the symbol table is first created. A larger initial capacity
 * can reduce the likelihood of hash collisions and improve symbol lookup
 * performance.
 */
#define SYMTABLE_INITIAL_CAPACITY 16

/**
 * \brief The load factor threshold for symbol table resizing.
 *
 * \details This macro defines the load factor threshold at which a symbol table
 * should be resized (rehashed) to maintain efficient symbol lookup operations.
 * When the number of symbols in the table exceeds this threshold as a ratio of
 * the capacity, a resizing operation is triggered to prevent hash collisions
 * and ensure performance.
 */
#define SYMTABLE_LOAD_FACTOR 0.75

symbol_t* symbol_init(const char* id, size_t len, ast_node_t* node)
{
  symbol_t* sym = (symbol_t*)malloc(sizeof(symbol_t));
  assert(sym != NULL);
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
  assert(table != NULL);

  if (parent != NULL)
    list_push_back(parent->children, table);

  table->parent = parent;
  table->children = list_init();
  table->size = 0;
  table->capacity = SYMTABLE_INITIAL_CAPACITY;
  table->buckets = (symbol_t**)calloc(table->capacity, sizeof(symbol_t*));
  assert(table->buckets != NULL);
  return table;
}

void symtable_free(symtable_t* table)
{
  list_for_each(table->children, (list_for_each_func_t)symtable_free);
  list_free(table->children);

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
  if (((double)table->size + 1) / (double)table->capacity >= SYMTABLE_LOAD_FACTOR)
    symtable_expand(table);

  size_t h = hash_digest(new_sym->id, new_sym->len);
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    ++table->size;
    table->buckets[idx] = new_sym;
    new_sym->next = NULL;
    new_sym->parent = table;
    return NULL;
  }

  symbol_t* last = NULL;

  for (symbol_t* sym = table->buckets[idx]; sym != NULL; last = sym, sym = sym->next)
    if (sym->len == new_sym->len && strncmp(sym->id, new_sym->id, sym->len) == 0)
      return sym;

  ++table->size;
  last->next = new_sym;
  new_sym->next = NULL;
  new_sym->parent = table;
  return NULL;
}

symbol_t* symtable_get(symtable_t* table, const char* id, size_t len)
{
  size_t h = hash_digest(id, len);
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

void symtable_expand(symtable_t* table)
{
  list_t* syms = list_init();

  for (size_t i = 0, j = 0; i < table->capacity && j < table->size; ++i)
    for (symbol_t* sym = table->buckets[i]; sym != NULL; sym = sym->next, ++j)
      list_push_back(syms, sym);

  table->size = 0;
  table->capacity <<= 1;

  table->buckets = (symbol_t**)realloc(table->buckets, table->capacity * sizeof(symbol_t*));
  memset(table->buckets, 0, table->capacity * sizeof(symbol_t*));

  while (!list_empty(syms))
  {
    symbol_t* sym = (symbol_t*)list_pop_back(syms);
    symtable_insert(table, sym);
  }

  list_free(syms);
}
