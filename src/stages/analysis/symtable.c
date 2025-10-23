/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/symtable.h"

#include "utils/hash.h"

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
static tau_symbol_t* tau_symtable_insert_no_expand(tau_symtable_t* table, tau_symbol_t* new_sym)
{
  size_t h = (size_t)tau_hash_digest(new_sym->id, new_sym->len);
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    table->size++;
    table->buckets[idx] = new_sym;
    new_sym->next = NULL;
    new_sym->parent = table;

    return NULL;
  }

  tau_symbol_t* last = NULL;

  for (tau_symbol_t* sym = table->buckets[idx]; sym != NULL; last = sym, sym = sym->next)
    if (sym->len == new_sym->len && strncmp(sym->id, new_sym->id, sym->len) == 0)
      return sym;

  TAU_ASSERT(last != NULL);

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
static void tau_symtable_expand(tau_symtable_t* table, size_t new_capacity)
{
  if (new_capacity <= table->capacity)
    return;

  size_t tau_symbol_count = table->size;

  tau_symbol_t** symbols = (tau_symbol_t**)malloc(tau_symbol_count * sizeof(tau_symbol_t*));
  TAU_ASSERT(symbols != NULL);

  for (size_t i = 0, j = 0; i < table->capacity && j < tau_symbol_count; ++i)
    for (tau_symbol_t* sym = table->buckets[i]; sym != NULL; sym = sym->next, ++j)
      symbols[j] = sym;

  table->size = 0;
  table->capacity = new_capacity;

  table->buckets = (tau_symbol_t**)realloc(table->buckets, table->capacity * sizeof(tau_symbol_t*));
  TAU_ASSERT(table->buckets != NULL);

  memset(table->buckets, 0, table->capacity * sizeof(tau_symbol_t*));

  for (size_t i = 0; i < tau_symbol_count; ++i)
    tau_symtable_insert_no_expand(table, symbols[i]);

  free(symbols);
}

tau_symbol_t* tau_symbol_init(const char* id, size_t len, tau_ast_node_t* node)
{
  tau_symbol_t* sym = (tau_symbol_t*)malloc(sizeof(tau_symbol_t));
  TAU_ASSERT(sym != NULL);

  sym->parent = NULL;
  sym->id = id;
  sym->len = len;
  sym->node = node;

  return sym;
}

tau_symbol_t* tau_symbol_init_with_str_view(tau_string_view_t id, tau_ast_node_t* node)
{
  return tau_symbol_init(id.buf, id.len, node);
}

void tau_symbol_free(tau_symbol_t* sym)
{
  free(sym);
}

tau_symtable_t* tau_symtable_init(tau_symtable_t* parent)
{
  tau_symtable_t* table = (tau_symtable_t*)malloc(sizeof(tau_symtable_t));
  TAU_ASSERT(table != NULL);

  if (parent != NULL)
    tau_vector_push(parent->children, table);

  table->parent = parent;
  table->children = tau_vector_init();
  table->size = 0;
  table->capacity = SYMTABLE_INITIAL_CAPACITY;
  table->buckets = (tau_symbol_t**)calloc(table->capacity, sizeof(tau_symbol_t*));
  TAU_ASSERT(table->buckets != NULL);

  return table;
}

void tau_symtable_free(tau_symtable_t* table)
{
  tau_vector_for_each(table->children, (tau_vector_for_each_func_t)tau_symtable_free);
  tau_vector_free(table->children);

  for (size_t i = 0, j = 0; i < table->capacity && j < table->size; ++i)
    for (tau_symbol_t *next, *sym = table->buckets[i]; sym != NULL; sym = next, ++j)
    {
      next = sym->next;
      tau_symbol_free(sym);
    }

  free(table->buckets);
  free(table);
}

tau_symbol_t* tau_symtable_insert(tau_symtable_t* table, tau_symbol_t* new_sym)
{
  if ((double)table->size + 1 >= SYMTABLE_LOAD_FACTOR * (double)table->capacity)
    tau_symtable_expand(table, table->capacity << 1);

  return tau_symtable_insert_no_expand(table, new_sym);
}

tau_symbol_t* tau_symtable_get(tau_symtable_t* table, const char* id, size_t len)
{
  size_t h = (size_t)tau_hash_digest(id, len);
  size_t idx = h % table->capacity;

  for (tau_symbol_t* sym = table->buckets[idx]; sym != NULL; sym = sym->next)
    if (sym->len == len && strncmp(sym->id, id, sym->len) == 0)
      return sym;

  return NULL;
}

tau_symbol_t* tau_symtable_get_with_str_view(tau_symtable_t* table, tau_string_view_t id)
{
  return tau_symtable_get(table, id.buf, id.len);
}

tau_symbol_t* tau_symtable_lookup(tau_symtable_t* table, const char* id, size_t len)
{
  while (table != NULL)
  {
    tau_symbol_t* sym = tau_symtable_get(table, id, len);

    if (sym != NULL)
      return sym;

    table = table->parent;
  }

  return NULL;
}

tau_symbol_t* tau_symtable_lookup_with_str_view(tau_symtable_t* table, tau_string_view_t id)
{
  return tau_symtable_lookup(table, id.buf, id.len);
}

void tau_symtable_merge(tau_symtable_t* dest, tau_symtable_t* src)
{
  size_t new_capacity = dest->capacity;

  while ((double)dest->size + (double)src->size >= SYMTABLE_LOAD_FACTOR * (double)new_capacity)
    new_capacity <<= 1;

  if (dest->capacity < new_capacity)
    tau_symtable_expand(dest, new_capacity);

  for (size_t i = 0, j = 0; i < src->capacity && j < src->size; ++i)
    for (tau_symbol_t* sym = src->buckets[i], *next = NULL; sym != NULL; sym = next, ++j)
    {
      next = sym->next;

      tau_symtable_insert_no_expand(dest, sym);
    }

  src->size = 0;
  memset(src->buckets, 0, sizeof(tau_symbol_t*) * src->capacity);

  tau_vector_extend(dest->children, src->children);

  for (size_t i = 0; i < tau_vector_size(src->children); ++i)
  {
    tau_symtable_t* child = (tau_symtable_t*)tau_vector_get(src->children, i);

    child->parent = dest;
  }

  tau_vector_clear(src->children);

  tau_symtable_free(src);
}
