#ifndef TAU_SYMBOL_H
#define TAU_SYMBOL_H

#include "forward_decls.h"

struct symbol_s
{
  symtable_t* scope;

  char* id;
 
  ast_node_t* node;
 
  symbol_t* next;
};

symbol_t* symbol_init(char* id, ast_node_t* node);
void symbol_free(symbol_t* sym);

#endif