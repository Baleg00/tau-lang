#ifndef TAU_SYMTABLE_H
#define TAU_SYMTABLE_H

#include <stddef.h>

#include "forward_decls.h"

struct symtable_s
{
  symtable_t* parent;
  list_t* children;

  size_t size, capacity;
  symbol_t** buckets;
};

symtable_t* symtable_init(symtable_t* parent);
void symtable_free(symtable_t* table);

symbol_t* symtable_insert(symtable_t* table, symbol_t* new_sym);
symbol_t* symtable_lookup(symtable_t* table, char* id);

void symtable_expand(symtable_t* table);

#endif