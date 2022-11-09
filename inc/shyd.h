#ifndef TAU_SHYD_H
#define TAU_SHYD_H

#include <stdbool.h>

#include "forward_decls.h"

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

shyd_elem_t* shyd_elem_init(parser_t* par, shyd_kind_t kind);
void shyd_elem_free(shyd_elem_t* elem);

bool shyd_flush_until_kind(queue_t* out_queue, stack_t* op_stack, shyd_kind_t kind);
void shyd_flush_for_op(queue_t* out_queue, stack_t* op_stack, op_kind_t op);

bool shyd_parse_typed_expr(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);
bool shyd_parse_paren_open(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);
bool shyd_parse_paren_close(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);
bool shyd_parse_bracket_open(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);
bool shyd_parse_bracket_close(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);
bool shyd_parse_term(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);
bool shyd_parse_operator(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);

bool shyd_postfix_step(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term);
queue_t* shyd_postfix(parser_t* par);

void shyd_ast_term(shyd_elem_t* elem, stack_t* node_stack);
void shyd_ast_operator(shyd_elem_t* elem, stack_t* node_stack);

ast_node_t* shyd_ast(parser_t* par);

#endif