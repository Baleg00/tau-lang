/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#include "stages/analysis/types/typetable.h"

#include <stddef.h>
#include <string.h>

#include "ast/ast.h"
#include "utils/common.h"
#include "utils/hash.h"
#include "utils/memory/memtrace.h"

/**
 * \brief The initial capacity for the type table.
 *
 * \details This macro defines the initial capacity for the type table, which
 * represents the number of buckets used to store type information. Increasing
 * this capacity may improve performance when dealing with a large number of types.
 */
#define TYPETABLE_INITIAL_CAPACITY 16

/**
 * \brief The load factor of the typetable.
 * 
 * \details The load factor determines when the typetable should be resized to
 * maintain efficient access times. When the number of entries reaches the
 * capacity multiplied by the load factor, the table will be resized to increase
 * its capacity.
 */
#define TYPETABLE_LOAD_FACTOR 0.75

/**
 * \brief Represents an entry in a type table.
 *
 * \details This structure holds information about a type entry in a type table.
 * It includes a reference to the AST node and the associated type descriptor.
 */
typedef struct typetable_entry_t typetable_entry_t;

struct typetable_entry_t
{
  ast_node_t* node; // A pointer to the AST node associated with the type.
  typedesc_t* desc; // A pointer to the descriptor of the type.
  typetable_entry_t* next; // A pointer to the next entry in case of collisions.
};

struct typetable_t
{
  size_t size; // The number of type entries in the table.
  size_t capacity; // The total capacity of the table.
  typetable_entry_t** buckets; // An array of buckets for storing type entries.
};

static typetable_entry_t* typetable_entry_init(ast_node_t* node, typedesc_t* desc)
{
  typetable_entry_t* entry = (typetable_entry_t*)malloc(sizeof(typetable_entry_t));
  ASSERT(entry != NULL);

  entry->node = node;
  entry->desc = desc;
  entry->next = NULL;

  return entry;
}

static void typetable_insert_entry(typetable_t* table, typetable_entry_t* entry)
{
  size_t h = (size_t)hash_digest(&entry->node, sizeof(ast_node_t*));
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    table->size++;
    table->buckets[idx] = entry;

    return;
  }

  typetable_entry_t* last = table->buckets[idx];

  while (last->next != NULL)
    last = last->next;

  table->size++;
  last->next = entry;
}

static void typetable_expand(typetable_t* table)
{
  size_t entry_count = 0;
  typetable_entry_t** entries = (typetable_entry_t**)malloc(table->size * sizeof(typetable_entry_t*));

  for (size_t i = 0; i < table->capacity && entry_count < table->size; ++i)
    for (typetable_entry_t* it = table->buckets[i]; it != NULL; it = it->next, ++entry_count)
      entries[entry_count] = it;

  for (size_t i = 0; i < entry_count; ++i)
    entries[i]->next = NULL;

  table->size = 0;
  table->capacity <<= 1;

  table->buckets = (typetable_entry_t**)realloc(table->buckets, table->capacity * sizeof(typetable_entry_t*));
  ASSERT(table->buckets != NULL);

  memset(table->buckets, 0, table->capacity * sizeof(typetable_entry_t*));

  for (size_t i = 0; i < entry_count; ++i)
    typetable_insert_entry(table, entries[i]);

  free(entries);
}

typetable_t* typetable_init(void)
{
  typetable_t* table = (typetable_t*)malloc(sizeof(typetable_t));
  ASSERT(table != NULL);

  table->size = 0;
  table->capacity = TYPETABLE_INITIAL_CAPACITY;

  table->buckets = (typetable_entry_t**)calloc(table->capacity, sizeof(typetable_entry_t*));
  ASSERT(table->buckets != NULL);

  return table;
}

void typetable_free(typetable_t* table)
{
  for (size_t i = 0, j = 0; i < table->capacity && j < table->size; ++i)
    for (typetable_entry_t *it = table->buckets[i], *next; it != NULL; it = next, ++j)
    {
      next = it->next;
      free(it);
    }

  free(table->buckets);
  free(table);
}

typedesc_t* typetable_insert(typetable_t* table, ast_node_t* node, typedesc_t* desc)
{
  if (((double)table->size + 1) / (double)table->capacity >= TYPETABLE_LOAD_FACTOR)
    typetable_expand(table);

  size_t h = (size_t)hash_digest(&node, sizeof(ast_node_t*));
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    table->size++;
    table->buckets[idx] = typetable_entry_init(node, desc);

    return NULL;
  }

  typetable_entry_t* last = NULL;

  for (typetable_entry_t* it = table->buckets[idx]; it != NULL; last = it, it = it->next)
    if (it->node == node)
    {
      typedesc_t* old_desc = it->desc;
      it->desc = desc;

      return old_desc;
    }

  table->size++;
  last->next = typetable_entry_init(node, desc);

  return NULL;
}

typedesc_t* typetable_lookup(typetable_t* table, ast_node_t* node)
{
  size_t h = (size_t)hash_digest(&node, sizeof(ast_node_t*));
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
    return NULL;

  for (typetable_entry_t* it = table->buckets[idx]; it != NULL; it = it->next)
    if (it->node == node)
      return it->desc;

  return NULL;
}
