/**
 * \file typetable.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#include "typetable.h"

#include <stddef.h>
#include <string.h>

#include "hash.h"
#include "memtrace.h"
#include "util.h"

/**
 * \brief The initial capacity of the typetable when it is created.
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
 * \brief Represents an entry in the typetable.
 */
typedef struct typetable_entry_s typetable_entry_t;

struct typetable_entry_s
{
  ast_node_t* node; // Pointer to the AST node associated with this entry.
  typedesc_t* desc; // Pointer to the type descriptor associated with the node.

  typetable_entry_t* next; // Pointer to the next entry in the same bucket.
};

struct typetable_s
{
  size_t size; // Number of entries currently stored in the table.
  size_t capacity; // Current capacity of the table.
  typetable_entry_t** buckets; // Array of bucket pointers to store the entries.
};

static typetable_entry_t* typetable_entry_init(ast_node_t* node, typedesc_t* desc)
{
  typetable_entry_t* entry = (typetable_entry_t*)malloc(sizeof(typetable_entry_t));
  assert(entry != NULL);

  entry->node = node;
  entry->desc = desc;
  entry->next = NULL;

  return entry;
}

static void typetable_insert_entry(typetable_t* table, typetable_entry_t* entry)
{
  hash_t h = hash_digest(entry->node, sizeof(ast_node_t*));
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
    {
      it->next = NULL;
      entries[entry_count] = it;
    }

  table->size = 0;
  table->capacity <<= 1;

  table->buckets = (typetable_entry_t**)realloc(table->buckets, table->capacity * sizeof(typetable_entry_t*));
  memset(table->buckets, 0, table->capacity * sizeof(typetable_entry_t*));

  for (size_t i = 0; i < entry_count; ++i)
    typetable_insert_entry(table, entries[i]);

  free(entries);
}

typetable_t* typetable_init(void)
{
  typetable_t* table = (typetable_t*)malloc(sizeof(typetable_t));
  assert(table != NULL);

  table->size = 0;
  table->capacity = TYPETABLE_INITIAL_CAPACITY;

  table->buckets = (typetable_entry_t**)calloc(table->capacity, sizeof(typetable_entry_t*));
  assert(table->buckets != NULL);

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

bool typetable_insert(typetable_t* table, ast_node_t* node, typedesc_t* desc)
{
  if (((double)table->size + 1) / (double)table->capacity >= TYPETABLE_LOAD_FACTOR)
    typetable_expand(table);

  hash_t h = hash_digest(node, sizeof(ast_node_t*));
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    table->size++;
    table->buckets[idx] = typetable_entry_init(node, desc);

    return true;
  }

  typetable_entry_t* last = NULL;

  for (typetable_entry_t* it = table->buckets[idx]; it != NULL; last = it, it = it->next)
    if (it->node == node)
      return false;

  table->size++;
  last->next = typetable_entry_init(node, desc);

  return true;
}

typedesc_t* typetable_lookup(typetable_t* table, ast_node_t* node)
{
  hash_t h = hash_digest(node, sizeof(ast_node_t*));
  size_t idx = h % table->capacity;

  if (table->buckets[idx] == NULL)
    return NULL;

  for (typetable_entry_t* it = table->buckets[idx]; it != NULL; it = it->next)
    if (it->node == node)
      return it->desc;
  
  return NULL;
}
