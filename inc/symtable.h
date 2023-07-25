#ifndef TAU_SYMTABLE_H
#define TAU_SYMTABLE_H

#include <stddef.h>

#include "typedefs.h"
#include "list.h"

struct symbol_s
{
  symtable_t* scope;
  
  const char* id;
  size_t len;
  ast_node_t* node;

  symbol_t* next;
};

struct symtable_s
{
  symtable_t* parent;
  list_t* children;

  size_t size, capacity;
  symbol_t** buckets;
};

symbol_t* symbol_init(const char* id, size_t len, ast_node_t* node);
void symbol_free(symbol_t* sym);

symtable_t* symtable_init(symtable_t* parent);
void symtable_free(symtable_t* table);

symbol_t* symtable_insert(symtable_t* table, symbol_t* new_sym);
symbol_t* symtable_lookup(symtable_t* table, const char* id, size_t len);

void symtable_expand(symtable_t* table);

#endif