#include "shyd.h"

#include "util.h"
#include "crumb.h"
#include "op.h"
#include "token.h"
#include "ast.h"
#include "parser.h"
#include "list.h"
#include "stack.h"
#include "queue.h"
#include "memtrace.h"

shyd_t* shyd_init(parser_t* par)
{
  shyd_t* shyd = (shyd_t*)malloc(sizeof(shyd_t));
  assert(shyd != NULL);

  shyd->par = par;
  shyd->out_queue = queue_init();
  shyd->op_stack = stack_init();
  shyd->prev_term = false;

  return shyd;
}

void shyd_free(shyd_t* shyd)
{
  queue_free(shyd->out_queue);
  stack_free(shyd->op_stack);
  free(shyd);
}

shyd_elem_t* shyd_elem_init(parser_t* par, shyd_kind_t kind)
{
  shyd_elem_t* elem = (shyd_elem_t*)malloc(sizeof(shyd_elem_t));
  elem->kind = kind;
  elem->tok = parser_current(par);
  return elem;
}

void shyd_elem_free(shyd_elem_t* elem)
{
  free(elem);
}

bool shyd_flush_until_kind(shyd_t* shyd, shyd_kind_t kind)
{
  while (!stack_empty(shyd->op_stack) && ((shyd_elem_t*)stack_peek(shyd->op_stack))->kind != kind)
    queue_offer(shyd->out_queue, stack_pop(shyd->op_stack));

  if (stack_empty(shyd->op_stack))
    return false;

  shyd_elem_free((shyd_elem_t*)stack_pop(shyd->op_stack));

  return true;
}

void shyd_flush_for_op(shyd_t* shyd, op_kind_t op)
{
  while (!stack_empty(shyd->op_stack))
  {
    shyd_elem_t* elem = (shyd_elem_t*)stack_peek(shyd->op_stack);

    if (elem->kind == SHYD_PAREN_OPEN || 
      elem->kind == SHYD_BRACKET_OPEN || 
      op_precedence(elem->op) > op_precedence(op))
      return;

    queue_offer(shyd->out_queue, stack_pop(shyd->op_stack));
  }
}

bool shyd_parse_typed_expr(shyd_t* shyd)
{
  switch (parser_current(shyd->par)->kind)
  {
  case TOK_KW_IS:
  case TOK_KW_AS:
    if (!shyd->prev_term)
      return false;
    break;
  case TOK_KW_SIZEOF:
  case TOK_KW_ALIGNOF:
    if (shyd->prev_term)
      return false;
  }

  shyd_elem_t* elem = shyd_elem_init(shyd->par, SHYD_OP);
  
  switch (parser_current(shyd->par)->kind)
  {
  case TOK_KW_IS:      elem->op = OP_IS; break;
  case TOK_KW_AS:      elem->op = OP_AS; break;
  case TOK_KW_SIZEOF:  elem->op = OP_SIZEOF; break;
  case TOK_KW_ALIGNOF: elem->op = OP_ALIGNOF; break;
  }

  stack_push(shyd->op_stack, elem);

  parser_next(shyd->par);

  shyd_elem_t* type = shyd_elem_init(shyd->par, SHYD_TYPE);
  type->node = parser_parse_type(shyd->par);
  queue_offer(shyd->out_queue, type);

  shyd->prev_term = true;
  
  return true;
}

bool shyd_parse_paren_open(shyd_t* shyd)
{
  if (shyd->prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(shyd->par, SHYD_PAREN_OPEN);
  stack_push(shyd->op_stack, elem);

  parser_next(shyd->par);

  shyd->prev_term = false;
  
  return true;
}

bool shyd_parse_paren_close(shyd_t* shyd)
{
  if (!shyd->prev_term)
    return false;

  if (!shyd_flush_until_kind(shyd, SHYD_PAREN_OPEN))
    return false;
  
  parser_next(shyd->par);
  
  shyd->prev_term = true;

  return true;
}

bool shyd_parse_bracket_open(shyd_t* shyd)
{
  if (!shyd->prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(shyd->par, SHYD_BRACKET_OPEN);
  stack_push(shyd->op_stack, elem);
  
  parser_next(shyd->par);
  
  shyd->prev_term = false;
  
  return true;
}

bool shyd_parse_bracket_close(shyd_t* shyd)
{
  if (!shyd->prev_term)
    return false;

  if (!shyd_flush_until_kind(shyd, SHYD_BRACKET_OPEN))
    return false;

  shyd_flush_for_op(shyd, OP_SUBS);
  
  shyd_elem_t* elem = shyd_elem_init(shyd->par, SHYD_OP);
  elem->op = OP_SUBS;
  stack_push(shyd->op_stack, elem);
  
  parser_next(shyd->par);
  
  shyd->prev_term = true;
  
  return true;
}

bool shyd_parse_call(shyd_t* shyd)
{
  if (!shyd->prev_term)
    return false;

  ast_node_t* node = parser_node_init(shyd->par, AST_EXPR_OP);
  node->expr_op.kind = OP_CALL;
  parser_expect(shyd->par, TOK_PUNCT_PAREN_LEFT);
  node->expr_op.op_call.args = parser_parse_delimited_list(shyd->par, TOK_PUNCT_COMMA, parser_parse_expr);
  parser_expect(shyd->par, TOK_PUNCT_PAREN_RIGHT);

  shyd_flush_for_op(shyd, OP_CALL);

  shyd_elem_t* elem = shyd_elem_init(shyd->par, SHYD_OP);
  elem->op = OP_CALL;
  elem->node = node;
  stack_push(shyd->op_stack, elem);
  
  return true;
}

bool shyd_parse_term(shyd_t* shyd)
{
  if (shyd->prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(shyd->par, SHYD_TERM);
  queue_offer(shyd->out_queue, elem);

  parser_next(shyd->par);

  shyd->prev_term = true;

  return true;
}

bool shyd_parse_operator(shyd_t* shyd)
{
  op_kind_t op = OP_UNKNOWN;

  switch (parser_current(shyd->par)->kind)
  {
  case TOK_PUNCT_PLUS:                  op = shyd->prev_term ? OP_ARIT_ADD : OP_ARIT_POS; break;
  case TOK_PUNCT_PLUS_PLUS:             op = shyd->prev_term ? OP_ARIT_INC_POST : OP_ARIT_INC_PRE; break;
  case TOK_PUNCT_PLUS_EQUAL:            op = OP_ARIT_ADD_ASSIGN; break;
  case TOK_PUNCT_MINUS:                 op = shyd->prev_term ? OP_ARIT_SUB : OP_ARIT_NEG; break;
  case TOK_PUNCT_MINUS_MINUS:           op = shyd->prev_term ? OP_ARIT_DEC_POST : OP_ARIT_DEC_PRE; break;
  case TOK_PUNCT_MINUS_EQUAL:           op = OP_ARIT_SUB_ASSIGN; break;
  case TOK_PUNCT_ASTERISK:              op = shyd->prev_term ? OP_ARIT_MUL : OP_IND; break;
  case TOK_PUNCT_ASTERISK_EQUAL:        op = OP_ARIT_MUL_ASSIGN; break;
  case TOK_PUNCT_ASTERISK_DOT:          op = OP_IND_MEMBER; break;
  case TOK_PUNCT_SLASH:                 op = OP_ARIT_DIV; break;
  case TOK_PUNCT_SLASH_EQUAL:           op = OP_ARIT_DIV_ASSIGN; break;
  case TOK_PUNCT_PERCENT:               op = OP_ARIT_MOD; break;
  case TOK_PUNCT_PERCENT_EQUAL:         op = OP_ARIT_MOD_ASSIGN; break;
  case TOK_PUNCT_AMPERSAND:             op = shyd->prev_term ? OP_BIT_AND : OP_ADDR; break;
  case TOK_PUNCT_AMPERSAND_AMPERSAND:   op = OP_LOGIC_AND; break;
  case TOK_PUNCT_AMPERSAND_EQUAL:       op = OP_BIT_AND_ASSIGN; break;
  case TOK_PUNCT_BAR:                   op = OP_BIT_OR; break;
  case TOK_PUNCT_BAR_BAR:               op = OP_LOGIC_OR; break;
  case TOK_PUNCT_BAR_EQUAL:             op = OP_BIT_OR_ASSIGN; break;
  case TOK_PUNCT_HAT:                   op = OP_BIT_XOR; break;
  case TOK_PUNCT_HAT_EQUAL:             op = OP_BIT_XOR_ASSIGN; break;
  case TOK_PUNCT_TILDE:                 op = OP_BIT_NOT; break;
  case TOK_PUNCT_LESS:                  op = OP_COMP_LT; break;
  case TOK_PUNCT_LESS_LESS:             op = OP_BIT_LSH; break;
  case TOK_PUNCT_LESS_LESS_EQUAL:       op = OP_BIT_LSH_ASSIGN; break;
  case TOK_PUNCT_LESS_EQUAL:            op = OP_COMP_LE; break;
  case TOK_PUNCT_GREATER:               op = OP_COMP_GT; break;
  case TOK_PUNCT_GREATER_GREATER:       op = OP_BIT_RSH; break;
  case TOK_PUNCT_GREATER_GREATER_EQUAL: op = OP_BIT_RSH_ASSIGN; break;
  case TOK_PUNCT_GREATER_EQUAL:         op = OP_COMP_GE; break;
  case TOK_PUNCT_BANG:                  op = OP_LOGIC_NOT; break;
  case TOK_PUNCT_BANG_EQUAL:            op = OP_COMP_NE; break;
  case TOK_PUNCT_DOT:                   op = OP_MEMBER; break;
  case TOK_PUNCT_DOT_DOT:               op = OP_RANGE; break;
  case TOK_PUNCT_QUESTION_DOT:          op = OP_SAFE_IND_MEMBER; break;
  case TOK_PUNCT_EQUAL:                 op = OP_ASSIGN; break;
  case TOK_PUNCT_EQUAL_EQUAL:           op = OP_COMP_EQ; break;
  case TOK_PUNCT_SEMICOLON:             op = OP_SEMICOLON; break;
  default: return false;
  }

  shyd_flush_for_op(shyd, op);

  shyd_elem_t* elem = shyd_elem_init(shyd->par, SHYD_OP);
  elem->op = op;
  stack_push(shyd->op_stack, elem);
  
  parser_next(shyd->par);

  shyd->prev_term = false;

  return true;
}

bool shyd_postfix_step(shyd_t* shyd)
{
  switch (parser_current(shyd->par)->kind)
  {
  case TOK_KW_IS:
  case TOK_KW_AS:
  case TOK_KW_SIZEOF:
  case TOK_KW_ALIGNOF: return shyd_parse_typed_expr(shyd);
  case TOK_PUNCT_PAREN_LEFT: return shyd->prev_term ? shyd_parse_call(shyd) : shyd_parse_paren_open(shyd);
  case TOK_PUNCT_PAREN_RIGHT: return shyd_parse_paren_close(shyd);
  case TOK_PUNCT_BRACKET_LEFT: return shyd_parse_bracket_open(shyd);
  case TOK_PUNCT_BRACKET_RIGHT: return shyd_parse_bracket_close(shyd);
  }

  if (parser_current(shyd->par)->kind == TOK_ID || token_is_literal(parser_current(shyd->par)))
    return shyd_parse_term(shyd);
  else if (token_is_punctuation(parser_current(shyd->par)))
    return shyd_parse_operator(shyd);

  return false;
}

void shyd_postfix(shyd_t* shyd)
{
  while (shyd_postfix_step(shyd));

  while (!stack_empty(shyd->op_stack))
  {
    shyd_elem_t* elem = (shyd_elem_t*)stack_pop(shyd->op_stack);
    
    if (elem->kind == SHYD_PAREN_OPEN)
    {
      crumb_error(elem->tok->loc, "Missing closing parenthesis!");
      exit(EXIT_FAILURE);
    }
    
    if (elem->kind == SHYD_BRACKET_OPEN)
    {
      crumb_error(elem->tok->loc, "Missing closing bracket!");
      exit(EXIT_FAILURE);
    }

    queue_offer(shyd->out_queue, elem);
  }
}

void shyd_ast_op_unary(shyd_elem_t* elem, stack_t* node_stack)
{
  ast_node_t* node = ast_node_init(AST_EXPR_OP);
  node->tok = elem->tok;
  node->expr_op.kind = elem->op;

  if (stack_empty(node_stack))
  {
    crumb_error(node->tok->loc, "Missing argument for unary operation!");
    exit(EXIT_FAILURE);
  }

  node->expr_op.op_unary.arg = (ast_node_t*)stack_pop(node_stack);

  stack_push(node_stack, node);
}

void shyd_ast_op_binary(shyd_elem_t* elem, stack_t* node_stack)
{
  ast_node_t* node = ast_node_init(AST_EXPR_OP);
  node->tok = elem->tok;
  node->expr_op.kind = elem->op;

  if (stack_empty(node_stack))
  {
    crumb_error(node->tok->loc, "Missing argument for binary operation!");
    exit(EXIT_FAILURE);
  }

  node->expr_op.op_binary.lhs = (ast_node_t*)stack_pop(node_stack);

  if (stack_empty(node_stack))
  {
    crumb_error(node->tok->loc, "Missing argument for binary operation!");
    exit(EXIT_FAILURE);
  }

  node->expr_op.op_binary.rhs = (ast_node_t*)stack_pop(node_stack);

  stack_push(node_stack, node);
}

void shyd_ast_op_call(shyd_elem_t* elem, stack_t* node_stack)
{
  if (stack_empty(node_stack))
  {
    crumb_error(elem->node->tok->loc, "Missing callee for function call!");
    exit(EXIT_FAILURE);
  }

  elem->node->expr_op.op_call.callee = (ast_node_t*)stack_pop(node_stack);

  stack_push(node_stack, elem->node);
}

void shyd_ast_term(shyd_elem_t* elem, stack_t* node_stack)
{
  ast_kind_t kind = AST_UNKNOWN;

  switch (elem->tok->kind)
  {
  case TOK_ID:
    kind = AST_ID;
    break;
  case TOK_LIT_INT_DEC:
  case TOK_LIT_INT_HEX:
  case TOK_LIT_INT_OCT:
  case TOK_LIT_INT_BIN:
    kind = AST_EXPR_LIT_INT;
    break;
  case TOK_LIT_FLT_DEC:
    kind = AST_EXPR_LIT_FLT;
    break;
  case TOK_LIT_STR:
    kind = AST_EXPR_LIT_STR;
    break;
  case TOK_LIT_CHAR:
    kind = AST_EXPR_LIT_CHAR;
    break;
  case TOK_LIT_BOOL_TRUE:
  case TOK_LIT_BOOL_FALSE:
    kind = AST_EXPR_LIT_BOOL;
    break;
  case TOK_LIT_NULL:
    kind = AST_EXPR_LIT_NULL;
    break;
  default:
    crumb_error(elem->tok->loc, "Unexpected token! Expected term!");
    exit(EXIT_FAILURE);
  }

  ast_node_t* node = ast_node_init(kind);
  node->tok = elem->tok;

  stack_push(node_stack, node);
}

void shyd_ast_op(shyd_elem_t* elem, stack_t* node_stack)
{
  if (op_is_unary(elem->op))
    shyd_ast_op_unary(elem, node_stack);
  else if (op_is_binary(elem->op))
    shyd_ast_op_binary(elem, node_stack);
  else if (elem->op == OP_CALL)
    shyd_ast_op_call(elem, node_stack);
  else
    unreachable();
}

ast_node_t* shyd_ast(parser_t* par)
{
  shyd_t* shyd = shyd_init(par);

  shyd_postfix(shyd);

  stack_t* node_stack = stack_init();

  while (!queue_empty(shyd->out_queue))
  {
    shyd_elem_t* elem = (shyd_elem_t*)queue_poll(shyd->out_queue);

    switch (elem->kind)
    {
    case SHYD_TERM: shyd_ast_term(elem, node_stack); break;
    case SHYD_TYPE: stack_push(node_stack, elem->node); break;
    case SHYD_OP:   shyd_ast_op(elem, node_stack); break;
    default: unreachable();
    }

    shyd_elem_free(elem);
  }

  ast_node_t* root = stack_empty(node_stack) ? NULL : (ast_node_t*)stack_pop(node_stack);

  assert(("Dangling nodes on stack!", stack_empty(node_stack)));

  stack_free(node_stack);

  shyd_free(shyd);

  return root;
}