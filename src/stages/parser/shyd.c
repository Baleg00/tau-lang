/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/shyd.h"

#include "ast/ast.h"

static ast_node_t* shyd_parse_expr_term_id(shyd_ctx_t* ctx)
{
  ast_node_t* node = (ast_node_t*)ast_expr_id_init();

  node->tok = parser_next(ctx->par);

  return node;
}

static ast_node_t* shyd_parse_expr_term_lit_int(shyd_ctx_t* ctx)
{
  ast_node_t* node = (ast_node_t*)ast_expr_lit_int_init();

  node->tok = parser_next(ctx->par);

  string_view_t view = token_to_string_view(node->tok);

  uint64_t value = 0;

  errno = 0;

  if (string_view_starts_with_cstr(view, "0x")) value = strtoull(string_view_begin(view) + 2, NULL, 16);
  else if (string_view_starts_with_cstr(view, "0o")) value = strtoull(string_view_begin(view) + 2, NULL, 8 );
  else if (string_view_starts_with_cstr(view, "0b")) value = strtoull(string_view_begin(view) + 2, NULL, 2 );
  else value = strtoull(string_view_begin(view), NULL, 10);

  ASSERT(errno == 0);

  ((ast_expr_lit_int_t*)node)->value = value;

  return node;
}

static ast_node_t* shyd_parse_expr_term_lit_flt(shyd_ctx_t* ctx)
{
  ast_node_t* node = (ast_node_t*)ast_expr_lit_flt_init();

  node->tok = parser_next(ctx->par);

  location_t loc = token_location(node->tok);

  ((ast_expr_lit_flt_t*)node)->value = strtold(loc.ptr, NULL);

  return node;
}

static ast_node_t* shyd_parse_expr_term_lit_str(shyd_ctx_t* ctx)
{
  ast_node_t* node = (ast_node_t*)ast_expr_lit_str_init();

  node->tok = parser_next(ctx->par);

  location_t loc = token_location(node->tok);

  ((ast_expr_lit_str_t*)node)->value = (char*)malloc(sizeof(char) * (loc.len - 1));
  memset(((ast_expr_lit_str_t*)node)->value, 0, sizeof(char) * (loc.len - 1));
  strncpy(((ast_expr_lit_str_t*)node)->value, loc.ptr + 1, loc.len - 2);

  return node;
}

static ast_node_t* shyd_parse_expr_term_lit_char(shyd_ctx_t* ctx)
{
  ast_node_t* node = (ast_node_t*)ast_expr_lit_char_init();

  node->tok = parser_next(ctx->par);

  ((ast_expr_lit_char_t*)node)->value = '\0';

  return node;
}

static ast_node_t* shyd_parse_expr_term_lit_bool(shyd_ctx_t* ctx)
{
  ast_node_t* node = (ast_node_t*)ast_expr_lit_bool_init();

  node->tok = parser_next(ctx->par);

  location_t loc = token_location(node->tok);

  ((ast_expr_lit_bool_t*)node)->value = strncmp(loc.ptr, "true", 4) == 0;

  return node;
}

static ast_node_t* shyd_parse_expr_term_lit_null(shyd_ctx_t* ctx)
{
  ast_node_t* node = (ast_node_t*)ast_expr_lit_null_init();

  node->tok = parser_next(ctx->par);

  return node;
}

shyd_ctx_t* shyd_ctx_init(parser_t* par)
{
  shyd_ctx_t* ctx = (shyd_ctx_t*)malloc(sizeof(shyd_ctx_t));
  ASSERT(ctx != NULL);

  ctx->par = par;
  ctx->out_queue = queue_init();
  ctx->op_stack = stack_init();
  ctx->node_stack = stack_init();
  ctx->prev_term = false;

  return ctx;
}

void shyd_ctx_free(shyd_ctx_t* ctx)
{
  queue_free(ctx->out_queue);
  stack_free(ctx->op_stack);
  stack_free(ctx->node_stack);
  free(ctx);
}

shyd_elem_t* shyd_elem_init(shyd_ctx_t* ctx, shyd_elem_kind_t kind)
{
  shyd_elem_t* elem = (shyd_elem_t*)malloc(sizeof(shyd_elem_t));
  CLEAROBJ(elem);

  elem->kind = kind;
  elem->tok = parser_current(ctx->par);

  return elem;
}

void shyd_elem_free(shyd_elem_t* elem)
{
  free(elem);
}

bool shyd_op_flush_until_elem(shyd_ctx_t* ctx, shyd_elem_kind_t kind)
{
  while (!stack_empty(ctx->op_stack) && ((shyd_elem_t*)stack_top(ctx->op_stack))->kind != kind)
    queue_offer(ctx->out_queue, stack_pop(ctx->op_stack));

  if (stack_empty(ctx->op_stack))
    return false;

  shyd_elem_free((shyd_elem_t*)stack_pop(ctx->op_stack));

  return true;
}

void shyd_op_flush_for_op(shyd_ctx_t* ctx, op_kind_t op)
{
  while (!stack_empty(ctx->op_stack))
  {
    shyd_elem_t* elem = (shyd_elem_t*)stack_top(ctx->op_stack);

    if (elem->kind != SHYD_PAREN_OPEN && elem->kind != SHYD_BRACKET_OPEN &&
      (op_precedence(elem->op) < op_precedence(op) ||
      (op_precedence(elem->op) == op_precedence(op) && op_is_left_assoc(op))))
      queue_offer(ctx->out_queue, stack_pop(ctx->op_stack));
    else
      break;
  }
}

bool shyd_parse_expr_typed(shyd_ctx_t* ctx)
{
  switch (parser_current(ctx->par)->kind)
  {
  case TOK_KW_IS:
  case TOK_KW_AS:
    if (!ctx->prev_term)
      return false;
    break;
  case TOK_KW_SIZEOF:
  case TOK_KW_ALIGNOF:
    if (ctx->prev_term)
      return false;
  default:
    NOOP();
  }

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_OP);

  switch (parser_current(ctx->par)->kind)
  {
  case TOK_KW_AS:      elem->op = OP_AS;      break;
  case TOK_KW_SIZEOF:  elem->op = OP_SIZEOF;  break;
  case TOK_KW_ALIGNOF: elem->op = OP_ALIGNOF; break;
  default: UNREACHABLE();
  }

  shyd_op_flush_for_op(ctx, elem->op);

  stack_push(ctx->op_stack, elem);

  parser_next(ctx->par);

  shyd_elem_t* type = shyd_elem_init(ctx, SHYD_TYPE);
  type->node = parser_parse_type(ctx->par);
  queue_offer(ctx->out_queue, type);

  ctx->prev_term = true;

  return true;
}

bool shyd_parse_punct_paren_left(shyd_ctx_t* ctx)
{
  if (ctx->prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_PAREN_OPEN);
  stack_push(ctx->op_stack, elem);

  parser_next(ctx->par);

  ctx->prev_term = false;

  return true;
}

bool shyd_parse_punct_paren_right(shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  if (!shyd_op_flush_until_elem(ctx, SHYD_PAREN_OPEN))
    return false;

  parser_next(ctx->par);

  ctx->prev_term = true;

  return true;
}

bool shyd_parse_punct_bracket_left(shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_BRACKET_OPEN);
  stack_push(ctx->op_stack, elem);

  parser_next(ctx->par);

  ctx->prev_term = false;

  return true;
}

bool shyd_parse_punct_bracket_right(shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  if (!shyd_op_flush_until_elem(ctx, SHYD_BRACKET_OPEN))
    return false;

  shyd_op_flush_for_op(ctx, OP_SUBS);

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_OP);
  elem->op = OP_SUBS;
  stack_push(ctx->op_stack, elem);

  parser_next(ctx->par);

  ctx->prev_term = true;

  return true;
}

bool shyd_parse_lit_vec_or_mat(shyd_ctx_t* ctx)
{
  if (ctx->prev_term)
    return false;

  token_t* token = parser_current(ctx->par);

  vector_t* values = vector_init();

  bool is_matrix = false;
  size_t cols = 0;
  size_t last_cols = 0;

  parser_expect(ctx->par, TOK_PUNCT_BRACKET_ANGLE_LEFT);

  for (;;)
  {
    vector_push(values, parser_parse_expr(ctx->par));

    if (is_matrix)
      last_cols++;
    else
      cols++;

    if (!parser_consume(ctx->par, TOK_PUNCT_COMMA))
    {
      if (parser_consume(ctx->par, TOK_PUNCT_SEMICOLON))
      {
        if (!is_matrix)
          is_matrix = true;
        else if (last_cols != cols)
          error_bag_put_parser_inconsistent_matrix_dimensions(ctx->par->errors, token_location(token));

        last_cols = 0;
      }
      else
        break;
    }
  }

  if (is_matrix && last_cols != cols)
    error_bag_put_parser_inconsistent_matrix_dimensions(ctx->par->errors, token_location(token));

  parser_expect(ctx->par, TOK_PUNCT_BRACKET_ANGLE_RIGHT);

  ASSERT(vector_size(values) > 0);

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_TERM);

  if (is_matrix)
  {
    ast_expr_lit_mat_t* mat_node = ast_expr_lit_mat_init();
    mat_node->tok = token;
    mat_node->rows = vector_size(values) / cols;
    mat_node->cols = cols;

    vector_extend(mat_node->values, values);

    elem->node = (ast_node_t*)mat_node;
  }
  else
  {
    ast_expr_lit_vec_t* vec_node = ast_expr_lit_vec_init();
    vec_node->tok = token;

    vector_extend(vec_node->values, values);

    elem->node = (ast_node_t*)vec_node;
  }

  vector_free(values);

  queue_offer(ctx->out_queue, elem);

  ctx->prev_term = true;

  return true;
}

bool shyd_parse_expr_op_call(shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  ast_expr_op_call_t* node = ast_expr_op_call_init();
  node->tok = parser_current(ctx->par);
  node->op_kind = OP_CALL;

  parser_expect(ctx->par, TOK_PUNCT_PAREN_LEFT);

  if (!parser_consume(ctx->par, TOK_PUNCT_PAREN_RIGHT))
  {
    parser_parse_delimited_list(ctx->par, node->params, TOK_PUNCT_COMMA, parser_parse_expr);

    parser_expect(ctx->par, TOK_PUNCT_PAREN_RIGHT);
  }

  shyd_op_flush_for_op(ctx, OP_CALL);

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_OP);
  elem->op = OP_CALL;
  elem->node = (ast_node_t*)node;

  stack_push(ctx->op_stack, elem);

  return true;
}

bool shyd_parse_expr_op_spec(shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  ast_expr_op_spec_t* node = ast_expr_op_spec_init();
  node->tok = parser_current(ctx->par);

  parser_expect(ctx->par, TOK_PUNCT_DOT_LESS);

  if (!parser_consume(ctx->par, TOK_PUNCT_GREATER))
  {
    parser_parse_delimited_list(ctx->par, node->params, TOK_PUNCT_COMMA, parser_parse_type);

    parser_expect(ctx->par, TOK_PUNCT_GREATER);
  }

  shyd_op_flush_for_op(ctx, OP_SPEC);

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_OP);
  elem->op = OP_SPEC;
  elem->node = (ast_node_t*)node;

  stack_push(ctx->op_stack, elem);

  return true;
}

bool shyd_parse_expr_term(shyd_ctx_t* ctx)
{
  if (ctx->prev_term)
    return false;

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_TERM);
  queue_offer(ctx->out_queue, elem);

  switch (parser_current(ctx->par)->kind)
  {
  case TOK_ID:       elem->node = shyd_parse_expr_term_id      (ctx); break;
  case TOK_LIT_INT:  elem->node = shyd_parse_expr_term_lit_int (ctx); break;
  case TOK_LIT_FLT:  elem->node = shyd_parse_expr_term_lit_flt (ctx); break;
  case TOK_LIT_STR:  elem->node = shyd_parse_expr_term_lit_str (ctx); break;
  case TOK_LIT_CHAR: elem->node = shyd_parse_expr_term_lit_char(ctx); break;
  case TOK_LIT_BOOL: elem->node = shyd_parse_expr_term_lit_bool(ctx); break;
  case TOK_LIT_NULL: elem->node = shyd_parse_expr_term_lit_null(ctx); break;
  default:
  {
    error_bag_put_parser_unexpected_token(ctx->par->errors, token_location(parser_current(ctx->par)));
    return false;
  }
  }

  ctx->prev_term = true;

  return true;
}

bool shyd_parse_expr_op(shyd_ctx_t* ctx)
{
  op_kind_t op;

  switch (parser_current(ctx->par)->kind)
  {
  case TOK_PUNCT_PLUS:                  op = ctx->prev_term ? OP_ARIT_ADD      : OP_ARIT_POS;     break;
  case TOK_PUNCT_PLUS_PLUS:             op = ctx->prev_term ? OP_ARIT_INC_POST : OP_ARIT_INC_PRE; break;
  case TOK_PUNCT_MINUS:                 op = ctx->prev_term ? OP_ARIT_SUB      : OP_ARIT_NEG;     break;
  case TOK_PUNCT_MINUS_MINUS:           op = ctx->prev_term ? OP_ARIT_DEC_POST : OP_ARIT_DEC_PRE; break;
  case TOK_PUNCT_ASTERISK:              op = ctx->prev_term ? OP_ARIT_MUL      : OP_IND;          break;
  case TOK_PUNCT_AMPERSAND:             op = ctx->prev_term ? OP_BIT_AND       : OP_ADDR;         break;
  case TOK_PUNCT_BANG:                  op = ctx->prev_term ? OP_UNWRAP_UNSAFE : OP_LOGIC_NOT;    break;
  case TOK_PUNCT_PLUS_EQUAL:            op = OP_ASSIGN_ARIT_ADD;  break;
  case TOK_PUNCT_MINUS_EQUAL:           op = OP_ASSIGN_ARIT_SUB;  break;
  case TOK_PUNCT_ASTERISK_EQUAL:        op = OP_ASSIGN_ARIT_MUL;  break;
  case TOK_PUNCT_ASTERISK_DOT:          op = OP_ACCESS_IND;       break;
  case TOK_PUNCT_SLASH:                 op = OP_ARIT_DIV;         break;
  case TOK_PUNCT_SLASH_EQUAL:           op = OP_ASSIGN_ARIT_DIV;  break;
  case TOK_PUNCT_PERCENT:               op = OP_ARIT_MOD;         break;
  case TOK_PUNCT_PERCENT_EQUAL:         op = OP_ASSIGN_ARIT_MOD;  break;
  case TOK_PUNCT_AMPERSAND_AMPERSAND:   op = OP_LOGIC_AND;        break;
  case TOK_PUNCT_AMPERSAND_EQUAL:       op = OP_ASSIGN_BIT_AND;   break;
  case TOK_PUNCT_BAR:                   op = OP_BIT_OR;           break;
  case TOK_PUNCT_BAR_BAR:               op = OP_LOGIC_OR;         break;
  case TOK_PUNCT_BAR_EQUAL:             op = OP_ASSIGN_BIT_OR;    break;
  case TOK_PUNCT_HAT:                   op = OP_BIT_XOR;          break;
  case TOK_PUNCT_HAT_EQUAL:             op = OP_ASSIGN_BIT_XOR;   break;
  case TOK_PUNCT_TILDE:                 op = OP_BIT_NOT;          break;
  case TOK_PUNCT_LESS:                  op = OP_CMP_LT;           break;
  case TOK_PUNCT_LESS_LESS:             op = OP_BIT_LSH;          break;
  case TOK_PUNCT_LESS_LESS_EQUAL:       op = OP_ASSIGN_BIT_LSH;   break;
  case TOK_PUNCT_LESS_EQUAL:            op = OP_CMP_LE;           break;
  case TOK_PUNCT_GREATER:               op = OP_CMP_GT;           break;
  case TOK_PUNCT_GREATER_GREATER:       op = OP_BIT_RSH;          break;
  case TOK_PUNCT_GREATER_GREATER_EQUAL: op = OP_ASSIGN_BIT_RSH;   break;
  case TOK_PUNCT_GREATER_EQUAL:         op = OP_CMP_GE;           break;
  case TOK_PUNCT_BANG_EQUAL:            op = OP_CMP_NE;           break;
  case TOK_PUNCT_DOT:                   op = OP_ACCESS;           break;
  case TOK_PUNCT_DOT_DOT:               op = OP_RANGE;            break;
  case TOK_PUNCT_QUESTION_DOT:          op = OP_ACCESS_OPT;       break;
  case TOK_PUNCT_EQUAL:                 op = OP_ASSIGN;           break;
  case TOK_PUNCT_EQUAL_EQUAL:           op = OP_CMP_EQ;           break;
  case TOK_PUNCT_QUESTION:              op = OP_UNWRAP_SAFE;      break;
  default: return false;
  }

  shyd_op_flush_for_op(ctx, op);

  shyd_elem_t* elem = shyd_elem_init(ctx, SHYD_OP);
  elem->op = op;
  stack_push(ctx->op_stack, elem);

  parser_next(ctx->par);

  ctx->prev_term = false;

  return true;
}

bool shyd_parse_postfix_next(shyd_ctx_t* ctx)
{
  switch (parser_current(ctx->par)->kind)
  {
  case TOK_KW_IS:
  case TOK_KW_AS:
  case TOK_KW_SIZEOF:
  case TOK_KW_ALIGNOF:                return shyd_parse_expr_typed(ctx);
  case TOK_PUNCT_DOT_LESS:            return shyd_parse_expr_op_spec(ctx);
  case TOK_PUNCT_PAREN_LEFT:          return ctx->prev_term ? shyd_parse_expr_op_call(ctx) : shyd_parse_punct_paren_left(ctx);
  case TOK_PUNCT_PAREN_RIGHT:         return shyd_parse_punct_paren_right(ctx);
  case TOK_PUNCT_BRACKET_LEFT:        return shyd_parse_punct_bracket_left(ctx);
  case TOK_PUNCT_BRACKET_RIGHT:       return shyd_parse_punct_bracket_right(ctx);
  case TOK_PUNCT_BRACKET_ANGLE_LEFT:  return shyd_parse_lit_vec_or_mat(ctx);
  default: NOOP();
  }

  if (parser_current(ctx->par)->kind == TOK_ID || token_is_literal(parser_current(ctx->par)))
    return shyd_parse_expr_term(ctx);

  if (token_is_punctuation(parser_current(ctx->par)))
    return shyd_parse_expr_op(ctx);

  return false;
}

void shyd_parse_postfix(shyd_ctx_t* ctx)
{
  while (shyd_parse_postfix_next(ctx))
  {}

  while (!stack_empty(ctx->op_stack))
  {
    shyd_elem_t* elem = (shyd_elem_t*)stack_pop(ctx->op_stack);

    if (elem->kind == SHYD_PAREN_OPEN)
    {
      error_bag_put_parser_missing_paren(ctx->par->errors, token_location(elem->tok));
      continue;
    }

    if (elem->kind == SHYD_BRACKET_OPEN)
    {
      error_bag_put_parser_missing_bracket(ctx->par->errors, token_location(elem->tok));
      continue;
    }

    queue_offer(ctx->out_queue, elem);
  }
}

void shyd_ast_expr_op_un(shyd_ctx_t* ctx, shyd_elem_t* elem)
{
  ast_expr_op_un_t* node = ast_expr_op_un_init();
  node->tok = elem->tok;
  node->op_kind = elem->op;

  if (stack_empty(ctx->node_stack))
  {
    error_bag_put_parser_missing_unary_argument(ctx->par->errors, token_location(node->tok));
    return;
  }

  node->expr = (ast_node_t*)stack_pop(ctx->node_stack);

  stack_push(ctx->node_stack, node);
}

void shyd_ast_expr_op_bin(shyd_ctx_t* ctx, shyd_elem_t* elem)
{
  ast_expr_op_bin_t* node = NULL;

  switch (elem->op)
  {
  case OP_ACCESS: node = (ast_expr_op_bin_t*)ast_expr_op_bin_access_init(); break;
  default: node = ast_expr_op_bin_init(); break;
  }

  node->tok = elem->tok;
  node->op_kind = elem->op;

  if (stack_empty(ctx->node_stack))
  {
    error_bag_put_parser_missing_binary_argument(ctx->par->errors, token_location(node->tok));
    return;
  }

  node->rhs = (ast_node_t*)stack_pop(ctx->node_stack);

  if (stack_empty(ctx->node_stack))
  {
    error_bag_put_parser_missing_binary_argument(ctx->par->errors, token_location(node->tok));
    return;
  }

  node->lhs = (ast_node_t*)stack_pop(ctx->node_stack);

  stack_push(ctx->node_stack, node);
}

void shyd_ast_expr_op_call(shyd_ctx_t* ctx, shyd_elem_t* elem)
{
  if (stack_empty(ctx->node_stack))
  {
    error_bag_put_parser_missing_callee(ctx->par->errors, token_location(elem->tok));
    return;
  }

  ((ast_expr_op_call_t*)elem->node)->callee = (ast_node_t*)stack_pop(ctx->node_stack);

  stack_push(ctx->node_stack, elem->node);
}

void shyd_ast_expr_op_spec(shyd_ctx_t* ctx, shyd_elem_t* elem)
{
  if (stack_empty(ctx->node_stack))
  {
    // TODO: create new report error function for specialization operator
    error_bag_put_parser_missing_callee(ctx->par->errors, token_location(elem->node->tok));
    return;
  }

  ((ast_expr_op_spec_t*)elem->node)->generic = (ast_node_t*)stack_pop(ctx->node_stack);

  stack_push(ctx->node_stack, elem->node);
}

void shyd_ast_expr_term(shyd_ctx_t* ctx, shyd_elem_t* elem)
{
  stack_push(ctx->node_stack, elem->node);
}

void shyd_ast_expr_op(shyd_ctx_t* ctx, shyd_elem_t* elem)
{
  if (op_is_unary(elem->op))
    shyd_ast_expr_op_un(ctx, elem);
  else if (op_is_binary(elem->op))
    shyd_ast_expr_op_bin(ctx, elem);
  else if (elem->op == OP_CALL)
    shyd_ast_expr_op_call(ctx, elem);
  else if (elem->op == OP_SPEC)
    shyd_ast_expr_op_spec(ctx, elem);
  else
    UNREACHABLE();
}

ast_node_t* shyd_parse_expr(parser_t* par)
{
  bool old_ignore_newline = parser_get_ignore_newline(par);

  parser_set_ignore_newline(par, false);

  shyd_ctx_t* ctx = shyd_ctx_init(par);

  shyd_parse_postfix(ctx);

  while (!queue_empty(ctx->out_queue))
  {
    shyd_elem_t* elem = (shyd_elem_t*)queue_poll(ctx->out_queue);

    switch (elem->kind)
    {
    case SHYD_TERM: shyd_ast_expr_term(ctx, elem); break;
    case SHYD_TYPE: stack_push(ctx->node_stack, elem->node); break;
    case SHYD_OP:   shyd_ast_expr_op(ctx, elem); break;
    default: UNREACHABLE();
    }

    shyd_elem_free(elem);
  }

  ast_node_t* root = stack_empty(ctx->node_stack) ? NULL : (ast_node_t*)stack_pop(ctx->node_stack);

  ASSERT(stack_empty(ctx->node_stack));

  shyd_ctx_free(ctx);

  parser_set_ignore_newline(par, old_ignore_newline);

  return root;
}
