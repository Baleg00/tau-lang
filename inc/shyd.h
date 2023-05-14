#ifndef TAU_SHYD_H
#define TAU_SHYD_H

#include <stddef.h>
#include <stdbool.h>

#include "typedefs.h"

typedef enum shyd_kind_e
{
  SHYD_UNKNOWN,
  SHYD_TERM,
  SHYD_TYPE,
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

typedef struct shyd_s
{
  parser_t* par;

  queue_t* out_queue;
  stack_t* op_stack;

  bool prev_term;
} shyd_t;

shyd_t* shyd_init(parser_t* par);
void shyd_free(shyd_t* shyd);

shyd_elem_t* shyd_elem_init(parser_t* par, shyd_kind_t kind);
void shyd_elem_free(shyd_elem_t* elem);

bool shyd_flush_until_kind(shyd_t* shyd, shyd_kind_t kind);
void shyd_flush_for_op(shyd_t* shyd, op_kind_t op);

bool shyd_parse_typed_expr(shyd_t* shyd);
bool shyd_parse_paren_open(shyd_t* shyd);
bool shyd_parse_paren_close(shyd_t* shyd);
bool shyd_parse_bracket_open(shyd_t* shyd);
bool shyd_parse_bracket_close(shyd_t* shyd);
bool shyd_parse_call(shyd_t* shyd);
bool shyd_parse_term(shyd_t* shyd);
bool shyd_parse_operator(shyd_t* shyd);

bool shyd_postfix_step(shyd_t* shyd);
void shyd_postfix(shyd_t* shyd);

void shyd_ast_term(shyd_elem_t* elem, stack_t* node_stack);
void shyd_ast_op_unary(shyd_elem_t* elem, stack_t* node_stack);
void shyd_ast_op_binary(shyd_elem_t* elem, stack_t* node_stack);
void shyd_ast_op_call(shyd_elem_t* elem, stack_t* node_stack);
void shyd_ast_op(shyd_elem_t* elem, stack_t* node_stack);

ast_node_t* shyd_ast(parser_t* par);

#endif