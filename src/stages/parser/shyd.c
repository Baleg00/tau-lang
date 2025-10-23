/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/shyd.h"

#include "ast/ast.h"

static tau_ast_node_t* tau_shyd_parse_expr_term_id(tau_shyd_ctx_t* ctx)
{
  tau_ast_node_t* node = (tau_ast_node_t*)tau_ast_expr_id_init();

  node->tok = tau_parser_next(ctx->par);

  return node;
}

static tau_ast_node_t* tau_shyd_parse_expr_term_lit_int(tau_shyd_ctx_t* ctx)
{
  tau_ast_node_t* node = (tau_ast_node_t*)tau_ast_expr_lit_int_init();

  node->tok = tau_parser_next(ctx->par);

  tau_string_view_t view = tau_token_to_string_view(node->tok);

  uint64_t value = 0;

  errno = 0;

  if (tau_string_view_starts_with_cstr(view, "0x")) value = strtoull(tau_string_view_begin(view) + 2, NULL, 16);
  else if (tau_string_view_starts_with_cstr(view, "0o")) value = strtoull(tau_string_view_begin(view) + 2, NULL, 8 );
  else if (tau_string_view_starts_with_cstr(view, "0b")) value = strtoull(tau_string_view_begin(view) + 2, NULL, 2 );
  else value = strtoull(tau_string_view_begin(view), NULL, 10);

  TAU_ASSERT(errno == 0);

  ((tau_ast_expr_lit_int_t*)node)->value = value;

  return node;
}

static tau_ast_node_t* tau_shyd_parse_expr_term_lit_flt(tau_shyd_ctx_t* ctx)
{
  tau_ast_node_t* node = (tau_ast_node_t*)tau_ast_expr_lit_flt_init();

  node->tok = tau_parser_next(ctx->par);

  tau_location_t loc = tau_token_location(node->tok);

  ((tau_ast_expr_lit_flt_t*)node)->value = strtold(loc.ptr, NULL);

  return node;
}

static tau_ast_node_t* tau_shyd_parse_expr_term_lit_str(tau_shyd_ctx_t* ctx)
{
  tau_ast_node_t* node = (tau_ast_node_t*)tau_ast_expr_lit_str_init();

  node->tok = tau_parser_next(ctx->par);

  tau_location_t loc = tau_token_location(node->tok);

  ((tau_ast_expr_lit_str_t*)node)->value = (char*)malloc(sizeof(char) * (loc.len - 1));
  memset(((tau_ast_expr_lit_str_t*)node)->value, 0, sizeof(char) * (loc.len - 1));
  strncpy(((tau_ast_expr_lit_str_t*)node)->value, loc.ptr + 1, loc.len - 2);

  return node;
}

static tau_ast_node_t* tau_shyd_parse_expr_term_lit_char(tau_shyd_ctx_t* ctx)
{
  tau_ast_node_t* node = (tau_ast_node_t*)tau_ast_expr_lit_char_init();

  node->tok = tau_parser_next(ctx->par);

  ((tau_ast_expr_lit_char_t*)node)->value = '\0';

  return node;
}

static tau_ast_node_t* tau_shyd_parse_expr_term_lit_bool(tau_shyd_ctx_t* ctx)
{
  tau_ast_node_t* node = (tau_ast_node_t*)tau_ast_expr_lit_bool_init();

  node->tok = tau_parser_next(ctx->par);

  tau_location_t loc = tau_token_location(node->tok);

  ((tau_ast_expr_lit_bool_t*)node)->value = strncmp(loc.ptr, "true", 4) == 0;

  return node;
}

static tau_ast_node_t* tau_shyd_parse_expr_term_lit_null(tau_shyd_ctx_t* ctx)
{
  tau_ast_node_t* node = (tau_ast_node_t*)tau_ast_expr_lit_null_init();

  node->tok = tau_parser_next(ctx->par);

  return node;
}

tau_shyd_ctx_t* tau_shyd_ctx_init(tau_parser_t* par)
{
  tau_shyd_ctx_t* ctx = (tau_shyd_ctx_t*)malloc(sizeof(tau_shyd_ctx_t));
  TAU_ASSERT(ctx != NULL);

  ctx->par = par;
  ctx->out_queue = tau_queue_init();
  ctx->op_stack = tau_stack_init();
  ctx->node_stack = tau_stack_init();
  ctx->prev_term = false;

  return ctx;
}

void tau_shyd_ctx_free(tau_shyd_ctx_t* ctx)
{
  tau_queue_free(ctx->out_queue);
  tau_stack_free(ctx->op_stack);
  tau_stack_free(ctx->node_stack);
  free(ctx);
}

tau_shyd_elem_t* tau_shyd_elem_init(tau_shyd_ctx_t* ctx, tau_shyd_elem_kind_t kind)
{
  tau_shyd_elem_t* elem = (tau_shyd_elem_t*)malloc(sizeof(tau_shyd_elem_t));
  TAU_CLEAROBJ(elem);

  elem->kind = kind;
  elem->tok = tau_parser_current(ctx->par);

  return elem;
}

void tau_shyd_elem_free(tau_shyd_elem_t* elem)
{
  free(elem);
}

bool tau_shyd_op_flush_until_elem(tau_shyd_ctx_t* ctx, tau_shyd_elem_kind_t kind)
{
  while (!tau_stack_empty(ctx->op_stack) && ((tau_shyd_elem_t*)tau_stack_top(ctx->op_stack))->kind != kind)
    tau_queue_offer(ctx->out_queue, tau_stack_pop(ctx->op_stack));

  if (tau_stack_empty(ctx->op_stack))
    return false;

  tau_shyd_elem_free((tau_shyd_elem_t*)tau_stack_pop(ctx->op_stack));

  return true;
}

void tau_shyd_op_flush_for_op(tau_shyd_ctx_t* ctx, op_kind_t op)
{
  while (!tau_stack_empty(ctx->op_stack))
  {
    tau_shyd_elem_t* elem = (tau_shyd_elem_t*)tau_stack_top(ctx->op_stack);

    if (elem->kind != TAU_SHYD_PAREN_OPEN && elem->kind != TAU_SHYD_BRACKET_OPEN &&
      (op_precedence(elem->op) < op_precedence(op) ||
      (op_precedence(elem->op) == op_precedence(op) && op_is_left_assoc(op))))
      tau_queue_offer(ctx->out_queue, tau_stack_pop(ctx->op_stack));
    else
      break;
  }
}

bool tau_shyd_parse_expr_typed(tau_shyd_ctx_t* ctx)
{
  switch (tau_parser_current(ctx->par)->kind)
  {
  case TAU_TOK_KW_IS:
  case TAU_TOK_KW_AS:
    if (!ctx->prev_term)
      return false;
    break;
  case TAU_TOK_KW_SIZEOF:
  case TAU_TOK_KW_ALIGNOF:
    if (ctx->prev_term)
      return false;
  default:
    TAU_NOOP();
  }

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_OP);

  switch (tau_parser_current(ctx->par)->kind)
  {
  case TAU_TOK_KW_AS:      elem->op = OP_AS;      break;
  case TAU_TOK_KW_SIZEOF:  elem->op = OP_SIZEOF;  break;
  case TAU_TOK_KW_ALIGNOF: elem->op = OP_ALIGNOF; break;
  default: TAU_UNREACHABLE();
  }

  tau_shyd_op_flush_for_op(ctx, elem->op);

  tau_stack_push(ctx->op_stack, elem);

  tau_parser_next(ctx->par);

  tau_shyd_elem_t* type = tau_shyd_elem_init(ctx, TAU_SHYD_TYPE);
  type->node = tau_parser_parse_type(ctx->par);
  tau_queue_offer(ctx->out_queue, type);

  ctx->prev_term = true;

  return true;
}

bool tau_shyd_parse_punct_paren_left(tau_shyd_ctx_t* ctx)
{
  if (ctx->prev_term)
    return false;

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_PAREN_OPEN);
  tau_stack_push(ctx->op_stack, elem);

  tau_parser_next(ctx->par);

  ctx->prev_term = false;

  return true;
}

bool tau_shyd_parse_punct_paren_right(tau_shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  if (!tau_shyd_op_flush_until_elem(ctx, TAU_SHYD_PAREN_OPEN))
    return false;

  tau_parser_next(ctx->par);

  ctx->prev_term = true;

  return true;
}

bool tau_shyd_parse_punct_bracket_left(tau_shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_BRACKET_OPEN);
  tau_stack_push(ctx->op_stack, elem);

  tau_parser_next(ctx->par);

  ctx->prev_term = false;

  return true;
}

bool tau_shyd_parse_punct_bracket_right(tau_shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  if (!tau_shyd_op_flush_until_elem(ctx, TAU_SHYD_BRACKET_OPEN))
    return false;

  tau_shyd_op_flush_for_op(ctx, OP_SUBS);

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_OP);
  elem->op = OP_SUBS;
  tau_stack_push(ctx->op_stack, elem);

  tau_parser_next(ctx->par);

  ctx->prev_term = true;

  return true;
}

bool tau_shyd_parse_lit_vec_or_mat(tau_shyd_ctx_t* ctx)
{
  if (ctx->prev_term)
    return false;

  tau_token_t* token = tau_parser_current(ctx->par);

  tau_vector_t* values = tau_vector_init();

  bool is_matrix = false;
  size_t cols = 0;
  size_t last_cols = 0;

  tau_parser_expect(ctx->par, TAU_TOK_PUNCT_BRACKET_ANGLE_LEFT);

  for (;;)
  {
    tau_vector_push(values, tau_parser_parse_expr(ctx->par));

    if (is_matrix)
      last_cols++;
    else
      cols++;

    if (!tau_parser_consume(ctx->par, TAU_TOK_PUNCT_COMMA))
    {
      if (tau_parser_consume(ctx->par, TAU_TOK_PUNCT_SEMICOLON))
      {
        if (!is_matrix)
          is_matrix = true;
        else if (last_cols != cols)
          tau_error_bag_put_parser_inconsistent_matrix_dimensions(ctx->par->errors, tau_token_location(token));

        last_cols = 0;
      }
      else
        break;
    }
  }

  if (is_matrix && last_cols != cols)
    tau_error_bag_put_parser_inconsistent_matrix_dimensions(ctx->par->errors, tau_token_location(token));

  tau_parser_expect(ctx->par, TAU_TOK_PUNCT_BRACKET_ANGLE_RIGHT);

  TAU_ASSERT(tau_vector_size(values) > 0);

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_TERM);

  if (is_matrix)
  {
    tau_ast_expr_lit_mat_t* mat_node = tau_ast_expr_lit_mat_init();
    mat_node->tok = token;
    mat_node->rows = tau_vector_size(values) / cols;
    mat_node->cols = cols;

    tau_vector_extend(mat_node->values, values);

    elem->node = (tau_ast_node_t*)mat_node;
  }
  else
  {
    tau_ast_expr_lit_vec_t* vec_node = tau_ast_expr_lit_vec_init();
    vec_node->tok = token;

    tau_vector_extend(vec_node->values, values);

    elem->node = (tau_ast_node_t*)vec_node;
  }

  tau_vector_free(values);

  tau_queue_offer(ctx->out_queue, elem);

  ctx->prev_term = true;

  return true;
}

bool tau_shyd_parse_expr_op_call(tau_shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  tau_ast_expr_op_call_t* node = tau_ast_expr_op_call_init();
  node->tok = tau_parser_current(ctx->par);
  node->op_kind = OP_CALL;

  tau_parser_expect(ctx->par, TAU_TOK_PUNCT_PAREN_LEFT);

  if (!tau_parser_consume(ctx->par, TAU_TOK_PUNCT_PAREN_RIGHT))
  {
    tau_parser_parse_delimited_list(ctx->par, node->params, TAU_TOK_PUNCT_COMMA, tau_parser_parse_expr);

    tau_parser_expect(ctx->par, TAU_TOK_PUNCT_PAREN_RIGHT);
  }

  tau_shyd_op_flush_for_op(ctx, OP_CALL);

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_OP);
  elem->op = OP_CALL;
  elem->node = (tau_ast_node_t*)node;

  tau_stack_push(ctx->op_stack, elem);

  return true;
}

bool tau_shyd_parse_expr_op_spec(tau_shyd_ctx_t* ctx)
{
  if (!ctx->prev_term)
    return false;

  tau_ast_expr_op_spec_t* node = tau_ast_expr_op_spec_init();
  node->tok = tau_parser_current(ctx->par);

  tau_parser_expect(ctx->par, TAU_TOK_PUNCT_DOT_LESS);

  if (!tau_parser_consume(ctx->par, TAU_TOK_PUNCT_GREATER))
  {
    tau_parser_parse_delimited_list(ctx->par, node->params, TAU_TOK_PUNCT_COMMA, tau_parser_parse_type);

    tau_parser_expect(ctx->par, TAU_TOK_PUNCT_GREATER);
  }

  tau_shyd_op_flush_for_op(ctx, OP_SPEC);

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_OP);
  elem->op = OP_SPEC;
  elem->node = (tau_ast_node_t*)node;

  tau_stack_push(ctx->op_stack, elem);

  return true;
}

bool tau_shyd_parse_expr_term(tau_shyd_ctx_t* ctx)
{
  if (ctx->prev_term)
    return false;

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_TERM);
  tau_queue_offer(ctx->out_queue, elem);

  switch (tau_parser_current(ctx->par)->kind)
  {
  case TAU_TOK_ID:       elem->node = tau_shyd_parse_expr_term_id      (ctx); break;
  case TAU_TOK_LIT_INT:  elem->node = tau_shyd_parse_expr_term_lit_int (ctx); break;
  case TAU_TOK_LIT_FLT:  elem->node = tau_shyd_parse_expr_term_lit_flt (ctx); break;
  case TAU_TOK_LIT_STR:  elem->node = tau_shyd_parse_expr_term_lit_str (ctx); break;
  case TAU_TOK_LIT_CHAR: elem->node = tau_shyd_parse_expr_term_lit_char(ctx); break;
  case TAU_TOK_LIT_BOOL: elem->node = tau_shyd_parse_expr_term_lit_bool(ctx); break;
  case TAU_TOK_LIT_NULL: elem->node = tau_shyd_parse_expr_term_lit_null(ctx); break;
  default:
  {
    tau_error_bag_put_parser_unexpected_token(ctx->par->errors, tau_token_location(tau_parser_current(ctx->par)));
    return false;
  }
  }

  ctx->prev_term = true;

  return true;
}

bool tau_shyd_parse_expr_op(tau_shyd_ctx_t* ctx)
{
  op_kind_t op;

  switch (tau_parser_current(ctx->par)->kind)
  {
  case TAU_TOK_PUNCT_PLUS:                  op = ctx->prev_term ? OP_ARIT_ADD      : OP_ARIT_POS;     break;
  case TAU_TOK_PUNCT_PLUS_PLUS:             op = ctx->prev_term ? OP_ARIT_INC_POST : OP_ARIT_INC_PRE; break;
  case TAU_TOK_PUNCT_MINUS:                 op = ctx->prev_term ? OP_ARIT_SUB      : OP_ARIT_NEG;     break;
  case TAU_TOK_PUNCT_MINUS_MINUS:           op = ctx->prev_term ? OP_ARIT_DEC_POST : OP_ARIT_DEC_PRE; break;
  case TAU_TOK_PUNCT_ASTERISK:              op = ctx->prev_term ? OP_ARIT_MUL      : OP_IND;          break;
  case TAU_TOK_PUNCT_AMPERSAND:             op = ctx->prev_term ? OP_BIT_AND       : OP_ADDR;         break;
  case TAU_TOK_PUNCT_BANG:                  op = ctx->prev_term ? OP_UNWRAP_UNSAFE : OP_LOGIC_NOT;    break;
  case TAU_TOK_PUNCT_PLUS_EQUAL:            op = OP_ASSIGN_ARIT_ADD;  break;
  case TAU_TOK_PUNCT_MINUS_EQUAL:           op = OP_ASSIGN_ARIT_SUB;  break;
  case TAU_TOK_PUNCT_MINUS_GREATER:         op = OP_ACCESS_IND;       break;
  case TAU_TOK_PUNCT_ASTERISK_EQUAL:        op = OP_ASSIGN_ARIT_MUL;  break;
  case TAU_TOK_PUNCT_SLASH:                 op = OP_ARIT_DIV;         break;
  case TAU_TOK_PUNCT_SLASH_EQUAL:           op = OP_ASSIGN_ARIT_DIV;  break;
  case TAU_TOK_PUNCT_PERCENT:               op = OP_ARIT_MOD;         break;
  case TAU_TOK_PUNCT_PERCENT_EQUAL:         op = OP_ASSIGN_ARIT_MOD;  break;
  case TAU_TOK_PUNCT_AMPERSAND_AMPERSAND:   op = OP_LOGIC_AND;        break;
  case TAU_TOK_PUNCT_AMPERSAND_EQUAL:       op = OP_ASSIGN_BIT_AND;   break;
  case TAU_TOK_PUNCT_BAR:                   op = OP_BIT_OR;           break;
  case TAU_TOK_PUNCT_BAR_BAR:               op = OP_LOGIC_OR;         break;
  case TAU_TOK_PUNCT_BAR_EQUAL:             op = OP_ASSIGN_BIT_OR;    break;
  case TAU_TOK_PUNCT_HAT:                   op = OP_BIT_XOR;          break;
  case TAU_TOK_PUNCT_HAT_EQUAL:             op = OP_ASSIGN_BIT_XOR;   break;
  case TAU_TOK_PUNCT_TILDE:                 op = OP_BIT_NOT;          break;
  case TAU_TOK_PUNCT_LESS:                  op = OP_CMP_LT;           break;
  case TAU_TOK_PUNCT_LESS_LESS:             op = OP_BIT_LSH;          break;
  case TAU_TOK_PUNCT_LESS_LESS_EQUAL:       op = OP_ASSIGN_BIT_LSH;   break;
  case TAU_TOK_PUNCT_LESS_EQUAL:            op = OP_CMP_LE;           break;
  case TAU_TOK_PUNCT_GREATER:               op = OP_CMP_GT;           break;
  case TAU_TOK_PUNCT_GREATER_GREATER:       op = OP_BIT_RSH;          break;
  case TAU_TOK_PUNCT_GREATER_GREATER_EQUAL: op = OP_ASSIGN_BIT_RSH;   break;
  case TAU_TOK_PUNCT_GREATER_EQUAL:         op = OP_CMP_GE;           break;
  case TAU_TOK_PUNCT_BANG_EQUAL:            op = OP_CMP_NE;           break;
  case TAU_TOK_PUNCT_DOT:                   op = OP_ACCESS_DIRECT;    break;
  case TAU_TOK_PUNCT_DOT_DOT:               op = OP_RANGE;            break;
  case TAU_TOK_PUNCT_QUESTION_DOT:          op = OP_ACCESS_OPT;       break;
  case TAU_TOK_PUNCT_EQUAL:                 op = OP_ASSIGN;           break;
  case TAU_TOK_PUNCT_EQUAL_EQUAL:           op = OP_CMP_EQ;           break;
  case TAU_TOK_PUNCT_QUESTION:              op = OP_UNWRAP_SAFE;      break;
  default: return false;
  }

  tau_shyd_op_flush_for_op(ctx, op);

  tau_shyd_elem_t* elem = tau_shyd_elem_init(ctx, TAU_SHYD_OP);
  elem->op = op;
  tau_stack_push(ctx->op_stack, elem);

  tau_parser_next(ctx->par);

  ctx->prev_term = false;

  return true;
}

bool tau_shyd_parse_postfix_next(tau_shyd_ctx_t* ctx)
{
  switch (tau_parser_current(ctx->par)->kind)
  {
  case TAU_TOK_KW_IS:
  case TAU_TOK_KW_AS:
  case TAU_TOK_KW_SIZEOF:
  case TAU_TOK_KW_ALIGNOF:                return tau_shyd_parse_expr_typed(ctx);
  case TAU_TOK_PUNCT_DOT_LESS:            return tau_shyd_parse_expr_op_spec(ctx);
  case TAU_TOK_PUNCT_PAREN_LEFT:          return ctx->prev_term ? tau_shyd_parse_expr_op_call(ctx) : tau_shyd_parse_punct_paren_left(ctx);
  case TAU_TOK_PUNCT_PAREN_RIGHT:         return tau_shyd_parse_punct_paren_right(ctx);
  case TAU_TOK_PUNCT_BRACKET_LEFT:        return tau_shyd_parse_punct_bracket_left(ctx);
  case TAU_TOK_PUNCT_BRACKET_RIGHT:       return tau_shyd_parse_punct_bracket_right(ctx);
  case TAU_TOK_PUNCT_BRACKET_ANGLE_LEFT:  return tau_shyd_parse_lit_vec_or_mat(ctx);
  default: TAU_NOOP();
  }

  if (tau_parser_current(ctx->par)->kind == TAU_TOK_ID || tau_token_is_literal(tau_parser_current(ctx->par)))
    return tau_shyd_parse_expr_term(ctx);

  if (tau_token_is_punctuation(tau_parser_current(ctx->par)))
    return tau_shyd_parse_expr_op(ctx);

  return false;
}

void tau_shyd_parse_postfix(tau_shyd_ctx_t* ctx)
{
  while (tau_shyd_parse_postfix_next(ctx))
  {}

  while (!tau_stack_empty(ctx->op_stack))
  {
    tau_shyd_elem_t* elem = (tau_shyd_elem_t*)tau_stack_pop(ctx->op_stack);

    if (elem->kind == TAU_SHYD_PAREN_OPEN)
    {
      tau_error_bag_put_parser_missing_paren(ctx->par->errors, tau_token_location(elem->tok));
      continue;
    }

    if (elem->kind == TAU_SHYD_BRACKET_OPEN)
    {
      tau_error_bag_put_parser_missing_bracket(ctx->par->errors, tau_token_location(elem->tok));
      continue;
    }

    tau_queue_offer(ctx->out_queue, elem);
  }
}

void tau_shyd_ast_expr_op_un(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem)
{
  tau_ast_expr_op_un_t* node = tau_ast_expr_op_un_init();
  node->tok = elem->tok;
  node->op_kind = elem->op;

  if (tau_stack_empty(ctx->node_stack))
  {
    tau_error_bag_put_parser_missing_unary_argument(ctx->par->errors, tau_token_location(node->tok));
    return;
  }

  node->expr = (tau_ast_node_t*)tau_stack_pop(ctx->node_stack);

  tau_stack_push(ctx->node_stack, node);
}

void tau_shyd_ast_expr_op_bin(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem)
{
  tau_ast_expr_op_bin_t* node = NULL;

  switch (elem->op)
  {
  case OP_ACCESS_DIRECT: node = (tau_ast_expr_op_bin_t*)tau_ast_expr_op_bin_access_direct_init(); break;
  default: node = tau_ast_expr_op_bin_init(); break;
  }

  node->tok = elem->tok;
  node->op_kind = elem->op;

  if (tau_stack_empty(ctx->node_stack))
  {
    tau_error_bag_put_parser_missing_binary_argument(ctx->par->errors, tau_token_location(node->tok));
    return;
  }

  node->rhs = (tau_ast_node_t*)tau_stack_pop(ctx->node_stack);

  if (tau_stack_empty(ctx->node_stack))
  {
    tau_error_bag_put_parser_missing_binary_argument(ctx->par->errors, tau_token_location(node->tok));
    return;
  }

  node->lhs = (tau_ast_node_t*)tau_stack_pop(ctx->node_stack);

  tau_stack_push(ctx->node_stack, node);
}

void tau_shyd_ast_expr_op_call(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem)
{
  if (tau_stack_empty(ctx->node_stack))
  {
    tau_error_bag_put_parser_missing_callee(ctx->par->errors, tau_token_location(elem->tok));
    return;
  }

  ((tau_ast_expr_op_call_t*)elem->node)->callee = (tau_ast_node_t*)tau_stack_pop(ctx->node_stack);

  tau_stack_push(ctx->node_stack, elem->node);
}

void tau_shyd_ast_expr_op_spec(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem)
{
  if (tau_stack_empty(ctx->node_stack))
  {
    // TODO: create new report error function for specialization operator
    tau_error_bag_put_parser_missing_callee(ctx->par->errors, tau_token_location(elem->node->tok));
    return;
  }

  ((tau_ast_expr_op_spec_t*)elem->node)->generic = (tau_ast_node_t*)tau_stack_pop(ctx->node_stack);

  tau_stack_push(ctx->node_stack, elem->node);
}

void tau_shyd_ast_expr_term(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem)
{
  tau_stack_push(ctx->node_stack, elem->node);
}

void tau_shyd_ast_expr_op(tau_shyd_ctx_t* ctx, tau_shyd_elem_t* elem)
{
  if (op_is_unary(elem->op))
    tau_shyd_ast_expr_op_un(ctx, elem);
  else if (op_is_binary(elem->op))
    tau_shyd_ast_expr_op_bin(ctx, elem);
  else if (elem->op == OP_CALL)
    tau_shyd_ast_expr_op_call(ctx, elem);
  else if (elem->op == OP_SPEC)
    tau_shyd_ast_expr_op_spec(ctx, elem);
  else
    TAU_UNREACHABLE();
}

tau_ast_node_t* tau_shyd_parse_expr(tau_parser_t* par)
{
  bool old_ignore_newline = tau_parser_get_ignore_newline(par);

  tau_parser_set_ignore_newline(par, false);

  tau_shyd_ctx_t* ctx = tau_shyd_ctx_init(par);

  tau_shyd_parse_postfix(ctx);

  while (!tau_queue_empty(ctx->out_queue))
  {
    tau_shyd_elem_t* elem = (tau_shyd_elem_t*)tau_queue_poll(ctx->out_queue);

    switch (elem->kind)
    {
    case TAU_SHYD_TERM: tau_shyd_ast_expr_term(ctx, elem); break;
    case TAU_SHYD_TYPE: tau_stack_push(ctx->node_stack, elem->node); break;
    case TAU_SHYD_OP:   tau_shyd_ast_expr_op(ctx, elem); break;
    default: TAU_UNREACHABLE();
    }

    tau_shyd_elem_free(elem);
  }

  tau_ast_node_t* root = tau_stack_empty(ctx->node_stack) ? NULL : (tau_ast_node_t*)tau_stack_pop(ctx->node_stack);

  TAU_ASSERT(tau_stack_empty(ctx->node_stack));

  tau_shyd_ctx_free(ctx);

  tau_parser_set_ignore_newline(par, old_ignore_newline);

  return root;
}
