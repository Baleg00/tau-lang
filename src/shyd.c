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

bool shyd_flush_until_kind(queue_t* out_queue, stack_t* op_stack, shyd_kind_t kind)
{
  while (!stack_empty(op_stack) && ((shyd_elem_t*)stack_peek(op_stack))->kind != kind)
    queue_offer(out_queue, stack_pop(op_stack));

  if (stack_empty(op_stack))
    return false;

  shyd_elem_free((shyd_elem_t*)stack_pop(op_stack));

  return true;
}

void shyd_flush_for_op(queue_t* out_queue, stack_t* op_stack, op_kind_t op)
{
  while (!stack_empty(op_stack))
  {
    shyd_elem_t* elem = (shyd_elem_t*)stack_peek(op_stack);

    if (elem->kind == SHYD_PAREN_OPEN || 
      elem->kind == SHYD_BRACKET_OPEN || 
      op_precedence(elem->op) > op_precedence(op))
      return;

    queue_offer(out_queue, stack_pop(op_stack));
  }
}

bool shyd_parse_typed_expr(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  switch (parser_current(par)->kind)
  {
  case TOK_KW_IS:
  case TOK_KW_AS:
    if (!*prev_term)
      return false;
    break;
  case TOK_KW_SIZEOF:
  case TOK_KW_ALIGNOF:
    if (*prev_term)
      return false;
  }

  shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
  
  switch (parser_current(par)->kind)
  {
  case TOK_KW_IS:      elem->op = OP_IS; break;
  case TOK_KW_AS:      elem->op = OP_AS; break;
  case TOK_KW_SIZEOF:  elem->op = OP_SIZEOF; break;
  case TOK_KW_ALIGNOF: elem->op = OP_ALIGNOF; break;
  }

  stack_push(op_stack, elem);

  parser_next(par);

  shyd_elem_t* type = shyd_elem_init(par, SHYD_TYPE);
  type->node = parser_parse_type(par);
  queue_offer(out_queue, type);

  *prev_term = true;
  
  return true;
}

bool shyd_parse_paren_open(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  if (*prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(par, SHYD_PAREN_OPEN);
  stack_push(op_stack, elem);

  parser_next(par);
  
  return true;
}

bool shyd_parse_paren_close(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  if (!*prev_term)
    return false;

  if (!shyd_flush_until_kind(out_queue, op_stack, SHYD_PAREN_OPEN))
  {
    crumb_error(parser_current(par)->loc, "Missing opening parenthesis!");
    exit(EXIT_FAILURE);
  }

  parser_next(par);
  
  *prev_term = true;

  return true;
}

bool shyd_parse_bracket_open(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  if (!*prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(par, SHYD_BRACKET_OPEN);
  stack_push(op_stack, elem);
  
  parser_next(par);
  
  *prev_term = false;
  
  return true;
}

bool shyd_parse_bracket_close(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  if (!*prev_term)
    return false;

  if (!shyd_flush_until_kind(out_queue, op_stack, SHYD_BRACKET_OPEN))
  {
    crumb_error(parser_current(par)->loc, "Missing opening brackets!");
    exit(EXIT_FAILURE);
  }

  shyd_flush_for_op(out_queue, op_stack, OP_SUBS);
  
  shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
  elem->op = OP_SUBS;
  stack_push(op_stack, elem);
  
  parser_next(par);
  
  *prev_term = true;
  
  return true;
}

bool shyd_parse_term(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  if (*prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(par, SHYD_TERM);
  queue_offer(out_queue, elem);

  parser_next(par);

  *prev_term = true;

  return true;
}

bool shyd_parse_operator(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  op_kind_t op = OP_UNKNOWN;

  switch (parser_current(par)->kind)
  {
  case TOK_PUNCT_PLUS:                  op = *prev_term ? OP_ARIT_ADD : OP_ARIT_POS; break;
  case TOK_PUNCT_PLUS_PLUS:             op = *prev_term ? OP_ARIT_INC_POST : OP_ARIT_INC_PRE; break;
  case TOK_PUNCT_PLUS_EQUAL:            op = OP_ARIT_ADD_ASSIGN; break;
  case TOK_PUNCT_MINUS:                 op = *prev_term ? OP_ARIT_SUB : OP_ARIT_NEG; break;
  case TOK_PUNCT_MINUS_MINUS:           op = *prev_term ? OP_ARIT_DEC_POST : OP_ARIT_DEC_PRE; break;
  case TOK_PUNCT_MINUS_EQUAL:           op = OP_ARIT_SUB_ASSIGN; break;
  case TOK_PUNCT_ASTERISK:              op = *prev_term ? OP_ARIT_MUL : OP_IND; break;
  case TOK_PUNCT_ASTERISK_EQUAL:        op = OP_ARIT_MUL_ASSIGN; break;
  case TOK_PUNCT_ASTERISK_DOT:          op = OP_IND_MEMBER; break;
  case TOK_PUNCT_SLASH:                 op = OP_ARIT_DIV; break;
  case TOK_PUNCT_SLASH_EQUAL:           op = OP_ARIT_DIV_ASSIGN; break;
  case TOK_PUNCT_PERCENT:               op = OP_ARIT_MOD; break;
  case TOK_PUNCT_PERCENT_EQUAL:         op = OP_ARIT_MOD_ASSIGN; break;
  case TOK_PUNCT_AMPERSAND:             op = *prev_term ? OP_BIT_AND : OP_ADDR; break;
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
  case TOK_PUNCT_COMMA:                 op = OP_COMMA; break;
  default: return false;
  }

  shyd_flush_for_op(out_queue, op_stack, op);

  shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
  elem->op = op;
  stack_push(op_stack, elem);
  
  parser_next(par);

  *prev_term = false;

  return true;
}

bool shyd_postfix_step(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  switch (parser_current(par)->kind)
  {
  case TOK_KW_IS:
  case TOK_KW_AS:
  case TOK_KW_SIZEOF:
  case TOK_KW_ALIGNOF:
    return shyd_parse_typed_expr(par, out_queue, op_stack, prev_term);
  case TOK_PUNCT_PAREN_LEFT:
    return shyd_parse_paren_open(par, out_queue, op_stack, prev_term);
  case TOK_PUNCT_PAREN_RIGHT:
    return shyd_parse_paren_close(par, out_queue, op_stack, prev_term);
  case TOK_PUNCT_BRACKET_LEFT:
    return shyd_parse_bracket_open(par, out_queue, op_stack, prev_term);
  case TOK_PUNCT_BRACKET_RIGHT:
    return shyd_parse_bracket_close(par, out_queue, op_stack, prev_term);
  }

  if (parser_current(par)->kind == TOK_ID || token_is_literal(parser_current(par)))
    return shyd_parse_term(par, out_queue, op_stack, prev_term);
  else if (token_is_punctuation(parser_current(par)))
    return shyd_parse_operator(par, out_queue, op_stack, prev_term);

  return false;
}

queue_t* shyd_postfix(parser_t* par)
{
  queue_t* out_queue = queue_init();
  stack_t* op_stack = stack_init();

  bool prev_term = false;

  while (shyd_postfix_step(par, out_queue, op_stack, &prev_term));

  while (!stack_empty(op_stack))
  {
    shyd_elem_t* elem = (shyd_elem_t*)stack_pop(op_stack);
    
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

    queue_offer(out_queue, elem);
  }

  stack_free(op_stack);

  return out_queue;
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

void shyd_ast_operator(shyd_elem_t* elem, stack_t* node_stack)
{
  ast_node_t* node = ast_node_init(AST_EXPR_OP);
  node->tok = elem->tok;
  node->expr_op.kind = elem->op;
  node->expr_op.params = list_init();

  switch (elem->op)
  {
  case OP_IS:
  case OP_AS:
    if (stack_empty(node_stack))
    {
      crumb_error(node->tok->loc, "Missing argument for binary operation!");
      exit(EXIT_FAILURE);
    }

    list_push_front(node->expr_op.params, stack_pop(node_stack));
  case OP_SIZEOF:
  case OP_ALIGNOF:
  case OP_TYPEOF:
  case OP_ARIT_INC_PRE:
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_PRE:
  case OP_ARIT_DEC_POST:
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
  case OP_LOGIC_NOT:
  case OP_IND:
  case OP_ADDR:
    if (stack_empty(node_stack))
    {
      crumb_error(node->tok->loc, "Missing argument for unary operation!");
      exit(EXIT_FAILURE);
    }
    
    list_push_front(node->expr_op.params, stack_pop(node_stack));
    break;
  
  case OP_IN:
  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
  case OP_BIT_AND:
  case OP_BIT_OR:
  case OP_BIT_XOR:
  case OP_BIT_LSH:
  case OP_BIT_RSH:
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
  case OP_COMP_EQ:
  case OP_COMP_NE:
  case OP_COMP_LT:
  case OP_COMP_LE:
  case OP_COMP_GT:
  case OP_COMP_GE:
  case OP_ASSIGN:
  case OP_ARIT_ADD_ASSIGN:
  case OP_ARIT_SUB_ASSIGN:
  case OP_ARIT_MUL_ASSIGN:
  case OP_ARIT_DIV_ASSIGN:
  case OP_ARIT_MOD_ASSIGN:
  case OP_BIT_AND_ASSIGN:
  case OP_BIT_OR_ASSIGN:
  case OP_BIT_XOR_ASSIGN:
  case OP_BIT_LSH_ASSIGN:
  case OP_BIT_RSH_ASSIGN:
  case OP_SUBS:
  case OP_MEMBER:
  case OP_IND_MEMBER:
  case OP_SAFE_IND_MEMBER:
  case OP_RANGE:
  case OP_COMMA:
    if (stack_empty(node_stack))
    {
      crumb_error(node->tok->loc, "Missing argument for binary operation!");
      exit(EXIT_FAILURE);
    }

    list_push_front(node->expr_op.params, stack_pop(node_stack));
    
    if (stack_empty(node_stack))
    {
      crumb_error(node->tok->loc, "Missing argument for binary operation!");
      exit(EXIT_FAILURE);
    }

    list_push_front(node->expr_op.params, stack_pop(node_stack));
    break;

  case OP_CALL: // TODO
    assert(false);
    break;
  default: unreachable();
  }

  stack_push(node_stack, node);
}

ast_node_t* shyd_ast(parser_t* par)
{
  queue_t* postfix_queue = shyd_postfix(par);
  stack_t* node_stack = stack_init();

  while (!queue_empty(postfix_queue))
  {
    shyd_elem_t* elem = (shyd_elem_t*)queue_poll(postfix_queue);

    switch (elem->kind)
    {
    case SHYD_TERM: shyd_ast_term(elem, node_stack); break;
    case SHYD_TYPE: stack_push(node_stack, elem->node); break;
    case SHYD_OP:   shyd_ast_operator(elem, node_stack); break;
    default: unreachable();
    }

    shyd_elem_free(elem);
  }

  ast_node_t* root = stack_empty(node_stack) ? NULL : (ast_node_t*)stack_pop(node_stack);

  assert(("Dangling nodes on stack!", stack_empty(node_stack)));

  stack_free(node_stack);
  queue_free(postfix_queue);

  return root;
}
