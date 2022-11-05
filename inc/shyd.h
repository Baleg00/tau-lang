#ifndef TAU_SHYD_H
#define TAU_SHYD_H

#include <stdbool.h>

#include "parser.h"
#include "stack.h"
#include "queue.h"

typedef enum shyd_kind_e
{
  SHYD_TERM,
  SHYD_OP,
  SHYD_PAREN_OPEN,
  SHYD_BRACKET_OPEN,
} shyd_kind_t;

typedef struct shyd_elem_s
{
  shyd_kind_t kind;
  token_t* tok;
  ast_node_t* node;
  op_kind_t op;
} shyd_elem_t;

shyd_elem_t* shyd_elem_init(parser_t* par, shyd_kind_t kind);
void shyd_elem_free(shyd_elem_t* elem);

bool shyd_to_postfix_step(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);
queue_t* shyd_to_postfix(parser_t* par);
ast_node_t* shyd_to_ast(parser_t* par);

#endif