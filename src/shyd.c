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

bool shyd_to_postfix_step(parser_t* par, queue_t* out_queue, stack_t* op_stack, bool* prev_term)
{
  if (parser_current(par)->kind == TOK_ID || token_is_literal(parser_current(par)))
  {
    if (*prev_term)
      return false;

    shyd_elem_t* elem = shyd_elem_init(par, SHYD_TERM);
    queue_offer(out_queue, elem);
    *prev_term = true;
  }
  else if (parser_current(par)->kind == TOK_KW_IS)
  {
    if (!*prev_term)
      return false;

    shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
    elem->op = OP_IS;
    parser_next(par);
    elem->node = parser_parse_type(par);
    stack_push(op_stack, elem);
    return true;
  }
  else if (parser_current(par)->kind == TOK_KW_AS)
  {
    if (!*prev_term)
      return false;

    shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
    elem->op = OP_AS;
    parser_next(par);
    elem->node = parser_parse_type(par);
    stack_push(op_stack, elem);
    return true;
  }
  else if (parser_current(par)->kind == TOK_KW_SIZEOF)
  {
    if (!*prev_term)
      return false;

    shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
    elem->op = OP_SIZEOF;
    parser_next(par);
    elem->node = parser_parse_type(par);
    stack_push(op_stack, elem);
    return true;
  }
  else if (parser_current(par)->kind == TOK_KW_ALIGNOF)
  {
    if (!*prev_term)
      return false;

    shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
    elem->op = OP_ALIGNOF;
    parser_next(par);
    elem->node = parser_parse_type(par);
    stack_push(op_stack, elem);
    return true;
  }
  else if (parser_current(par)->kind == TOK_PUNCT_PAREN_LEFT)
  {
    shyd_elem_t* elem = shyd_elem_init(par, SHYD_PAREN_OPEN);
    stack_push(op_stack, elem);
    *prev_term = false;
  }
  else if (parser_current(par)->kind == TOK_PUNCT_PAREN_RIGHT)
  {
    while (!stack_empty(op_stack) && ((shyd_elem_t*)stack_peek(op_stack))->kind != SHYD_PAREN_OPEN)
      queue_offer(out_queue, stack_pop(op_stack));

    if (stack_empty(op_stack))
      return false;

    stack_pop(op_stack);

    *prev_term = true;
  }
  else if (parser_current(par)->kind == TOK_PUNCT_BRACKET_LEFT)
  {
    shyd_elem_t* elem = shyd_elem_init(par, SHYD_BRACKET_OPEN);
    stack_push(op_stack, elem);
    *prev_term = false;
  }
  else if (parser_current(par)->kind == TOK_PUNCT_BRACKET_RIGHT)
  {
    while (!stack_empty(op_stack) && ((shyd_elem_t*)stack_peek(op_stack))->kind != SHYD_BRACKET_OPEN)
      queue_offer(out_queue, stack_pop(op_stack));

    if (stack_empty(op_stack))
      return false;

    stack_pop(op_stack);

    while (!stack_empty(op_stack))
    {
      shyd_elem_t* elem = (shyd_elem_t*)stack_peek(op_stack);

      if (elem->kind == SHYD_PAREN_OPEN || elem->kind == SHYD_BRACKET_OPEN)
        break;

      if (op_precedence(elem->op) > op_precedence(OP_SUBS))
        break;
        
      queue_offer(out_queue, stack_pop(op_stack));
    }

    shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
    elem->op = OP_SUBS;
    stack_push(op_stack, elem);

    *prev_term = true;
  }
  else if (token_is_punctuation(parser_current(par)))
  {
    op_kind_t op;

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

    while (!stack_empty(op_stack))
    {
      shyd_elem_t* elem = (shyd_elem_t*)stack_peek(op_stack);

      if (elem->kind == SHYD_PAREN_OPEN || elem->kind == SHYD_BRACKET_OPEN)
        break;

      if (op_precedence(elem->op) > op_precedence(op))
        break;

      queue_offer(out_queue, stack_pop(op_stack));
    }

    shyd_elem_t* elem = shyd_elem_init(par, SHYD_OP);
    elem->op = op;
    stack_push(op_stack, elem);

    *prev_term = false;
  }
  else
    return false;

  parser_next(par);

  return true;
}

queue_t* shyd_to_postfix(parser_t* par)
{
  queue_t* out_queue = queue_init();
  stack_t* op_stack = stack_init();

  bool prev_term = false;

  while (shyd_to_postfix_step(par, out_queue, op_stack, &prev_term));

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

ast_node_t* shyd_to_ast(parser_t* par)
{
  queue_t* postfix_queue = shyd_to_postfix(par);
  stack_t* node_stack = stack_init();

  while (!queue_empty(postfix_queue))
  {
    shyd_elem_t* elem = (shyd_elem_t*)queue_poll(postfix_queue);

    if (elem->kind == SHYD_TERM)
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
    else if (elem->kind == SHYD_OP)
    {
      ast_node_t* node = ast_node_init(AST_EXPR_OP);
      node->tok = elem->tok;
      node->expr_op.kind = elem->op;
      node->expr_op.params = list_init();

      switch (elem->op)
      {
      case OP_IS:
      case OP_AS:
      case OP_SIZEOF:
      case OP_ALIGNOF:
        list_push_back(node->expr_op.params, elem->node);
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

      case OP_CALL: break; // TODO
      default: unreachable();
      }

      stack_push(node_stack, node);
    }

    shyd_elem_free(elem);
  }

  ast_node_t* root = stack_empty(node_stack) ? NULL : (ast_node_t*)stack_pop(node_stack);

  assert(("Dangling nodes on stack!", stack_empty(node_stack)));

  stack_free(node_stack);
  queue_free(postfix_queue);

  return root;
}
