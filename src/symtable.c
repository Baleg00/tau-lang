#include "symtable.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "util.h"
#include "list.h"
#include "symbol.h"

#define SYMTABLE_INITIAL_CAPACITY 16
#define SYMTABLE_LOAD_FACTOR 0.75

static inline uint64_t fnv1a_hash(const uint8_t* data, size_t size)
{
  uint64_t hash = 0xcbf29ce484222325ULL;

  for (size_t i = 0; i < size; ++i)
  {
    hash ^= data[i];
    hash *= 0x00000100000001B3ULL;
  }

  return hash;
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
  list_for_each(table->children, symtable_free);
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

  uint64_t hash = fnv1a_hash(new_sym->id, strlen(new_sym->id));
  size_t idx = hash % table->capacity;

  if (table->buckets[idx] == NULL)
  {
    ++table->size;
    table->buckets[idx] = new_sym;
    new_sym->next = NULL;
    return NULL;
  }

  symbol_t* last = NULL;

  for (symbol_t* sym = table->buckets[idx]; sym != NULL; last = sym, sym = sym->next)
    if (strcmp(sym->id, new_sym->id) == 0)
      return sym;

  ++table->size;
  last->next = new_sym;
  new_sym->next = NULL;
  return NULL;
}

symbol_t* symtable_lookup(symtable_t* table, char* id)
{
  uint64_t hash = fnv1a_hash(id, strlen(id));
  size_t idx = hash % table->capacity;

  for (symbol_t* sym = table->buckets[idx]; sym != NULL; sym = sym->next)
    if (strcmp(sym->id, id) == 0)
      return sym;

  if (table->parent == NULL)
    return NULL;

  return symtable_lookup(table->parent, id);
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
