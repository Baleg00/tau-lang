/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#include "stages/analysis/types/typetable.h"

#include "ast/ast.h"
#include "utils/hash.h"

/// The initial number of buckets in a type table.
#define TYPETABLE_INITIAL_CAPACITY 16

/// The load factor threshold for type table resizing.
#define TYPETABLE_LOAD_FACTOR 0.75

/// Represents an entry in a type table.
typedef struct tau_typetable_entry_t tau_typetable_entry_t;

struct tau_typetable_entry_t
{
  tau_ast_node_t* node;        ///< Pointer to the AST node associated with the type.
  tau_typedesc_t* desc;        ///< Pointer to the descriptor of the type.
  tau_typetable_entry_t* next; ///< Pointer to the next entry in the bucket.
};

struct tau_typetable_t
{
  size_t size;                 ///< The number of type entries in the table.
  size_t capacity;             ///< The total capacity of the table.
  tau_typetable_entry_t** buckets; ///< An array of buckets for storing type entries.
};

/**
 * \brief Initializes a new type table entry.
 *
 * \param[in] node Pointer to the AST node to be associated with the entry.
 * \param[in] desc Pointer to the type descriptor for the AST node.
 * \returns Pointer to the newly initialized type table entry.
 */
static tau_typetable_entry_t* tau_typetable_entry_init(tau_ast_node_t* node, tau_typedesc_t* desc)
{
  tau_typetable_entry_t* entry = (tau_typetable_entry_t*)malloc(sizeof(tau_typetable_entry_t));
  TAU_ASSERT(entry != NULL);

  entry->node = node;
  entry->desc = desc;
  entry->next = NULL;

  return entry;
}

/**
 * \brief Inserts an entry into a type table.
 *
 * \param[in,out] table Pointer to the type table to insert into.
 * \param[in] entry Pointer to the entry to be inserted.
 */
static void tau_typetable_insert_entry(tau_typetable_t* table, tau_typetable_entry_t* entry)
{
  size_t h = (size_t)tau_hash_digest(&entry->node, sizeof(tau_ast_node_t*));
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    table->size++;
    table->buckets[idx] = entry;
    entry->next = NULL;

    return;
  }

  tau_typetable_entry_t* last = table->buckets[idx];

  while (last->next != NULL)
    last = last->next;

  table->size++;
  last->next = entry;
  entry->next = NULL;
}

/**
 * \brief Expands the capacity of a type table.
 *
 * \param[in,out] table Pointer to the type table to be expanded.
 * \param[in] new_capacity The new capacity of the type table.
 */
static void tau_typetable_expand(tau_typetable_t* table, size_t new_capacity)
{
  if (table->capacity >= new_capacity)
    return;

  size_t entry_count = 0;
  tau_typetable_entry_t** entries = (tau_typetable_entry_t**)malloc(table->size * sizeof(tau_typetable_entry_t*));

  for (size_t i = 0; i < table->capacity && entry_count < table->size; ++i)
    for (tau_typetable_entry_t* it = table->buckets[i]; it != NULL; it = it->next, ++entry_count)
      entries[entry_count] = it;

  for (size_t i = 0; i < entry_count; ++i)
    entries[i]->next = NULL;

  table->size = 0;
  table->capacity = new_capacity;

  table->buckets = (tau_typetable_entry_t**)realloc(table->buckets, table->capacity * sizeof(tau_typetable_entry_t*));
  TAU_ASSERT(table->buckets != NULL);

  memset(table->buckets, 0, table->capacity * sizeof(tau_typetable_entry_t*));

  for (size_t i = 0; i < entry_count; ++i)
    tau_typetable_insert_entry(table, entries[i]);

  free(entries);
}

tau_typetable_t* tau_typetable_init(void)
{
  tau_typetable_t* table = (tau_typetable_t*)malloc(sizeof(tau_typetable_t));
  TAU_ASSERT(table != NULL);

  table->size = 0;
  table->capacity = TYPETABLE_INITIAL_CAPACITY;

  table->buckets = (tau_typetable_entry_t**)calloc(table->capacity, sizeof(tau_typetable_entry_t*));
  TAU_ASSERT(table->buckets != NULL);

  return table;
}

void tau_typetable_free(tau_typetable_t* table)
{
  for (size_t i = 0, j = 0; i < table->capacity && j < table->size; ++i)
    for (tau_typetable_entry_t *it = table->buckets[i], *next; it != NULL; it = next, ++j)
    {
      next = it->next;
      free(it);
    }

  free(table->buckets);
  free(table);
}

tau_typedesc_t* tau_typetable_insert(tau_typetable_t* table, tau_ast_node_t* node, tau_typedesc_t* desc)
{
  if ((double)table->size + 1 >= TYPETABLE_LOAD_FACTOR * (double)table->capacity)
    tau_typetable_expand(table, table->capacity << 1);

  size_t h = (size_t)tau_hash_digest(&node, sizeof(tau_ast_node_t*));
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    table->size++;
    table->buckets[idx] = tau_typetable_entry_init(node, desc);

    return NULL;
  }

  tau_typetable_entry_t* last = NULL;

  for (tau_typetable_entry_t* it = table->buckets[idx]; it != NULL; last = it, it = it->next)
    if (it->node == node)
    {
      tau_typedesc_t* old_desc = it->desc;
      it->desc = desc;

      return old_desc;
    }

  TAU_ASSERT(last != NULL);

  table->size++;
  last->next = tau_typetable_entry_init(node, desc);

  return NULL;
}

tau_typedesc_t* tau_typetable_lookup(tau_typetable_t* table, tau_ast_node_t* node)
{
  size_t h = (size_t)tau_hash_digest(&node, sizeof(tau_ast_node_t*));
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
    return NULL;

  for (tau_typetable_entry_t* it = table->buckets[idx]; it != NULL; it = it->next)
    if (it->node == node)
      return it->desc;

  return NULL;
}

void tau_typetable_merge(tau_typetable_t* dest, tau_typetable_t* src)
{
  size_t new_capacity = dest->capacity;

  while (dest->size + src->size >= new_capacity)
    new_capacity <<= 1;

  tau_typetable_expand(dest, new_capacity);

  for (size_t i = 0, j = 0; i < src->capacity && j < src->size; ++i)
    for (tau_typetable_entry_t* entry = src->buckets[i], *next = NULL; entry != NULL; entry = next, ++j)
    {
      next = entry->next;

      tau_typetable_insert_entry(dest, entry);
    }

  src->size = 0;
  memset(src->buckets, 0, sizeof(tau_typetable_entry_t*) * src->capacity);

  tau_typetable_free(src);
}
