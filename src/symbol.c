#include "symbol.h"

#include <stdlib.h>

#include "util.h"
#include "memtrace.h"

symbol_t* symbol_init(char* id, ast_node_t* node)
{
  symbol_t* sym = (symbol_t*)malloc(sizeof(symbol_t));
  assert(sym != NULL);
  sym->scope = NULL;
  sym->id = id;
  sym->node = node;
  return sym;
}

void symbol_free(symbol_t* sym)
{
  free(sym);
}
