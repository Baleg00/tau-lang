/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/parser.h"

#include "stages/parser/shyd.h"

static void tau_parser_skip_newlines(tau_parser_t* par)
{
  tau_token_t* tok = (tau_token_t*)tau_vector_get(par->tokens, par->cur);

  while (tok->kind == TAU_TOK_NEWLINE)
  {
    par->cur++;
    tok = (tau_token_t*)tau_vector_get(par->tokens, par->cur);
  }
}

static tau_ast_node_t* tau_parser_cstr_to_prim(const char* cstr)
{
  if (strncmp(cstr, "i8" , 2) == 0) return (tau_ast_node_t*)tau_ast_type_prim_i8_init();
  if (strncmp(cstr, "u8" , 2) == 0) return (tau_ast_node_t*)tau_ast_type_prim_u8_init();
  if (strncmp(cstr, "i16", 3) == 0) return (tau_ast_node_t*)tau_ast_type_prim_i16_init();
  if (strncmp(cstr, "u16", 3) == 0) return (tau_ast_node_t*)tau_ast_type_prim_u16_init();
  if (strncmp(cstr, "i32", 3) == 0) return (tau_ast_node_t*)tau_ast_type_prim_i32_init();
  if (strncmp(cstr, "u32", 3) == 0) return (tau_ast_node_t*)tau_ast_type_prim_u32_init();
  if (strncmp(cstr, "i64", 3) == 0) return (tau_ast_node_t*)tau_ast_type_prim_i64_init();
  if (strncmp(cstr, "u64", 3) == 0) return (tau_ast_node_t*)tau_ast_type_prim_u64_init();
  if (strncmp(cstr, "f32", 3) == 0) return (tau_ast_node_t*)tau_ast_type_prim_f32_init();
  if (strncmp(cstr, "f64", 3) == 0) return (tau_ast_node_t*)tau_ast_type_prim_f64_init();

  TAU_UNREACHABLE();
}

tau_parser_t* tau_parser_init(void)
{
  tau_parser_t* par = (tau_parser_t*)malloc(sizeof(tau_parser_t));
  TAU_CLEAROBJ(par);

  par->parents = tau_stack_init();

  return par;
}

void tau_parser_free(tau_parser_t* par)
{
  tau_stack_free(par->parents);
  free(par);
}

tau_token_t* tau_parser_current(tau_parser_t* par)
{
  if (par->ignore_newlines)
    tau_parser_skip_newlines(par);

  return (tau_token_t*)tau_vector_get(par->tokens, par->cur);
}

tau_token_t* tau_parser_next(tau_parser_t* par)
{
  tau_token_t* tok = tau_parser_current(par);

  if (tok->kind != TAU_TOK_EOF)
    par->cur++;

  return tok;
}

tau_token_t* tau_parser_peek(tau_parser_t* par)
{
  tau_token_t* tok = tau_parser_current(par);

  if (tok->kind == TAU_TOK_EOF)
    return tok;

  size_t next = par->cur + 1;

  while (par->ignore_newlines && (tok = (tau_token_t*)tau_vector_get(par->tokens, next))->kind == TAU_TOK_NEWLINE)
    next++;

  return tok;
}

bool tau_parser_consume(tau_parser_t* par, tau_token_kind_t kind)
{
  if (tau_parser_current(par)->kind == kind)
  {
    tau_parser_next(par);
    return true;
  }

  return false;
}

tau_token_t* tau_parser_expect(tau_parser_t* par, tau_token_kind_t kind)
{
  tau_token_t* token = tau_parser_next(par);

  if (token->kind != kind)
  {
    tau_error_bag_put_parser_unexpected_token(par->errors, tau_token_location(token));
    return NULL;
  }

  return token;
}

bool tau_parser_get_ignore_newline(tau_parser_t* par)
{
  return par->ignore_newlines;
}

void tau_parser_set_ignore_newline(tau_parser_t* par, bool ignore)
{
  par->ignore_newlines = ignore;
}

void tau_parser_decl_context_clear(tau_parser_t* par)
{
  par->decl_ctx.is_pub    = false;
  par->decl_ctx.is_extern = false;
  par->decl_ctx.callconv  = TAU_CALLCONV_TAU;
}

void tau_parser_parse_decl_context_pub(tau_parser_t* par)
{
  tau_parser_expect(par, TAU_TOK_KW_PUB);

  par->decl_ctx.is_pub = true;
}

void tau_parser_parse_decl_context_extern(tau_parser_t* par)
{
  tau_parser_expect(par, TAU_TOK_KW_EXTERN);

  par->decl_ctx.is_extern = true;
  par->decl_ctx.callconv = TAU_CALLCONV_CDECL;

  if (tau_parser_current(par)->kind == TAU_TOK_LIT_STR)
    par->decl_ctx.callconv = tau_parser_parse_callconv(par);
}

void tau_parser_parse_delimited_list(tau_parser_t* par, tau_vector_t* dest, tau_token_kind_t delim, parse_func_t parse_func)
{
  for (;;)
  {
    tau_vector_push(dest, parse_func(par));

    if (!tau_parser_consume(par, delim))
      break;
  }
}

void tau_parser_parse_terminated_list(tau_parser_t* par, tau_vector_t* dest, tau_token_kind_t termin, parse_func_t parse_func)
{
  while (!tau_parser_consume(par, termin))
    tau_vector_push(dest, parse_func(par));
}

tau_callconv_kind_t tau_parser_parse_callconv(tau_parser_t* par)
{
  static struct {
    const char* str;
    tau_callconv_kind_t callconv;
  } str_callconv_map[] = {
    { "\"Tau\"",        TAU_CALLCONV_TAU        },
    { "\"C\"",          TAU_CALLCONV_CDECL      },
    { "\"cdecl\"",      TAU_CALLCONV_CDECL      },
    { "\"stdcall\"",    TAU_CALLCONV_STDCALL    },
    { "\"win64\"",      TAU_CALLCONV_WIN64      },
    { "\"sysv64\"",     TAU_CALLCONV_SYSV64     },
    { "\"aapcs\"",      TAU_CALLCONV_AAPCS      },
    { "\"fastcall\"",   TAU_CALLCONV_FASTCALL   },
    { "\"vectorcall\"", TAU_CALLCONV_VECTORCALL },
    { "\"thiscall\"",   TAU_CALLCONV_THISCALL   },
  };

  tau_token_t* tau_callconv_token = tau_parser_next(par);

  if (tau_callconv_token->kind != TAU_TOK_LIT_STR)
  {
    tau_error_bag_put_parser_expected_calling_convention(par->errors, tau_token_location(tau_callconv_token));
    return TAU_CALLCONV_UNKNOWN;
  }

  tau_string_view_t tau_callconv_str_view = tau_token_to_string_view(tau_callconv_token);

  for (size_t i = 0; i < TAU_COUNTOF(str_callconv_map); i++)
    if (tau_string_view_compare_cstr(tau_callconv_str_view, str_callconv_map[i].str) == 0)
      return str_callconv_map[i].callconv;

  tau_error_bag_put_parser_unknown_calling_convention(par->errors, tau_token_location(tau_callconv_token));

  return TAU_CALLCONV_UNKNOWN;
}

tau_ast_node_t* tau_parser_parse_id(tau_parser_t* par)
{
  tau_ast_id_t* node = tau_ast_id_init();

  node->tok = tau_parser_expect(par, TAU_TOK_ID);

  if (node->tok == NULL)
    return (tau_ast_node_t*)tau_ast_poison_init();

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_id(tau_parser_t* par)
{
  tau_ast_type_id_t* node = tau_ast_type_id_init();

  node->tok = tau_parser_expect(par, TAU_TOK_ID);

  if (node->tok == NULL)
    return (tau_ast_node_t*)tau_ast_poison_init();

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_mut(tau_parser_t* par)
{
  tau_ast_type_mut_t* node = tau_ast_type_mut_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_MUT);

  node->base_type = tau_parser_parse_type(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_ptr(tau_parser_t* par)
{
  tau_ast_type_ptr_t* node = tau_ast_type_ptr_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_ASTERISK);

  node->base_type = tau_parser_parse_type(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_array(tau_parser_t* par)
{
  tau_ast_type_array_t* node = tau_ast_type_array_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACKET_LEFT);

  node->size = tau_parser_current(par)->kind == TAU_TOK_PUNCT_BRACKET_RIGHT ? NULL : tau_parser_parse_expr(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACKET_RIGHT);

  node->base_type = tau_parser_parse_type(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_ref(tau_parser_t* par)
{
  tau_ast_type_ref_t* node = tau_ast_type_ref_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_AMPERSAND);

  node->base_type = tau_parser_parse_type(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_opt(tau_parser_t* par)
{
  tau_ast_type_opt_t* node = tau_ast_type_opt_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_QUESTION);

  node->base_type = tau_parser_parse_type(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_fun(tau_parser_t* par)
{
  tau_ast_type_fun_t* node = tau_ast_type_fun_init();
  node->tok = tau_parser_current(par);

  node->callconv = TAU_CALLCONV_TAU;

  if (tau_parser_consume(par, TAU_TOK_KW_EXTERN))
    node->callconv = tau_parser_parse_callconv(par);

  tau_parser_expect(par, TAU_TOK_KW_FUN);
  tau_parser_expect(par, TAU_TOK_PUNCT_PAREN_LEFT);

  tau_parser_parse_delimited_list(par, node->params, TAU_TOK_PUNCT_COMMA, tau_parser_parse_type);

  tau_parser_expect(par, TAU_TOK_PUNCT_PAREN_RIGHT);
  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->return_type = tau_parser_parse_type(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_vec(tau_parser_t* par)
{
  tau_ast_type_vec_t* node = tau_ast_type_vec_init();
  node->tok = tau_parser_next(par);

  tau_string_view_t tok_view = tau_token_to_string_view(node->tok);
  const char* tok_cstr = tau_string_view_begin(tok_view);

  const char* size_end = NULL;
  size_t size = strtoull(tok_cstr + 3, &size_end, 10);

  TAU_ASSERT(errno != ERANGE);
  TAU_ASSERT(size > 0);

  node->size = size;

  node->base_type = tau_parser_cstr_to_prim(size_end);
  node->base_type->tok = node->tok;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_mat(tau_parser_t* par)
{
  tau_ast_type_mat_t* node = tau_ast_type_mat_init();
  node->tok = tau_parser_next(par);

  tau_string_view_t tok_view = tau_token_to_string_view(node->tok);
  const char* tok_cstr = tau_string_view_begin(tok_view);

  const char* rows_end = NULL;
  size_t rows = strtoull(tok_cstr + 3, &rows_end, 10);

  TAU_ASSERT(errno != ERANGE);
  TAU_ASSERT(rows > 0);

  node->rows = rows;

  const char* size_end = rows_end;

  if (*rows_end == 'x')
  {
    const char* cols_end = NULL;
    size_t cols = strtoull(rows_end + 1, &cols_end, 10);

    TAU_ASSERT(errno != ERANGE);
    TAU_ASSERT(cols > 0);

    node->cols = cols;

    size_end = cols_end;
  }
  else
    node->cols = rows;

  node->base_type = tau_parser_cstr_to_prim(size_end);
  node->base_type->tok = node->tok;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_type_member(tau_parser_t* par)
{
  tau_ast_node_t* node = tau_parser_parse_type_id(par);

  while (tau_parser_consume(par, TAU_TOK_PUNCT_DOT))
  {
    tau_ast_type_mbr_t* member_node = tau_ast_type_mbr_init();
    member_node->tok = tau_parser_current(par);

    member_node->parent = node;
    member_node->member = tau_parser_parse_type_id(par);

    node = (tau_ast_node_t*)member_node;
  }

  return node;
}

tau_ast_node_t* tau_parser_parse_type(tau_parser_t* par)
{
  tau_ast_node_t* node = NULL;

  switch (tau_parser_current(par)->kind)
  {
  case TAU_TOK_ID:                 return tau_parser_parse_type_member  (par);
  case TAU_TOK_KW_MUT:             return tau_parser_parse_type_mut     (par);
  case TAU_TOK_PUNCT_ASTERISK:     return tau_parser_parse_type_ptr     (par);
  case TAU_TOK_PUNCT_BRACKET_LEFT: return tau_parser_parse_type_array   (par);
  case TAU_TOK_PUNCT_AMPERSAND:    return tau_parser_parse_type_ref     (par);
  case TAU_TOK_PUNCT_QUESTION:     return tau_parser_parse_type_opt     (par);
  case TAU_TOK_KW_FUN:             return tau_parser_parse_type_fun     (par);
  case TAU_TOK_KW_VEC:             return tau_parser_parse_type_vec     (par);
  case TAU_TOK_KW_MAT:             return tau_parser_parse_type_mat     (par);
  case TAU_TOK_KW_I8:
    node = (tau_ast_node_t*)tau_ast_type_prim_i8_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_I8);
    break;
  case TAU_TOK_KW_I16:
    node = (tau_ast_node_t*)tau_ast_type_prim_i16_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_I16);
    break;
  case TAU_TOK_KW_I32:
    node = (tau_ast_node_t*)tau_ast_type_prim_i32_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_I32);
    break;
  case TAU_TOK_KW_I64:
    node = (tau_ast_node_t*)tau_ast_type_prim_i64_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_I64);
    break;
  case TAU_TOK_KW_ISIZE:
    node = (tau_ast_node_t*)tau_ast_type_prim_isize_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_ISIZE);
    break;
  case TAU_TOK_KW_U8:
    node = (tau_ast_node_t*)tau_ast_type_prim_u8_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_U8);
    break;
  case TAU_TOK_KW_U16:
    node = (tau_ast_node_t*)tau_ast_type_prim_u16_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_U16);
    break;
  case TAU_TOK_KW_U32:
    node = (tau_ast_node_t*)tau_ast_type_prim_u32_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_U32);
    break;
  case TAU_TOK_KW_U64:
    node = (tau_ast_node_t*)tau_ast_type_prim_u64_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_U64);
    break;
  case TAU_TOK_KW_USIZE:
    node = (tau_ast_node_t*)tau_ast_type_prim_usize_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_USIZE);
    break;
  case TAU_TOK_KW_F32:
    node = (tau_ast_node_t*)tau_ast_type_prim_f32_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_F32);
    break;
  case TAU_TOK_KW_F64:
    node = (tau_ast_node_t*)tau_ast_type_prim_f64_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_F64);
    break;
  case TAU_TOK_KW_C64:
    node = (tau_ast_node_t*)tau_ast_type_prim_c64_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_C64);
    break;
  case TAU_TOK_KW_C128:
    node = (tau_ast_node_t*)tau_ast_type_prim_c128_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_C128);
    break;
  case TAU_TOK_KW_CHAR:
    node = (tau_ast_node_t*)tau_ast_type_prim_char_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_CHAR);
    break;
  case TAU_TOK_KW_BOOL:
    node = (tau_ast_node_t*)tau_ast_type_prim_bool_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_BOOL);
    break;
  case TAU_TOK_KW_UNIT:
    node = (tau_ast_node_t*)tau_ast_type_prim_unit_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_UNIT);
    break;
  case TAU_TOK_KW_TYPE:
    node = (tau_ast_node_t*)tau_ast_type_type_init();
    node->tok = tau_parser_expect(par, TAU_TOK_KW_TYPE);
    break;
  default:
  {
    tau_error_bag_put_parser_unexpected_token(par->errors, tau_token_location(tau_parser_current(par)));

    return (tau_ast_node_t*)tau_ast_poison_init();
  }
  }

  if (node->tok == NULL)
    return (tau_ast_node_t*)tau_ast_poison_init();

  return node;
}

tau_ast_node_t* tau_parser_parse_expr(tau_parser_t* par)
{
  return tau_shyd_parse_expr(par);
}

tau_ast_node_t* tau_parser_parse_stmt_if(tau_parser_t* par)
{
  tau_ast_stmt_if_t* node = tau_ast_stmt_if_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_IF);

  node->cond = tau_parser_parse_expr(par);

  tau_parser_expect(par, TAU_TOK_KW_THEN);

  node->stmt = tau_parser_parse_stmt(par);
  node->stmt_else = tau_parser_consume(par, TAU_TOK_KW_ELSE) ? tau_parser_parse_stmt(par) : NULL;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_for(tau_parser_t* par)
{
  tau_ast_stmt_for_t* node = tau_ast_stmt_for_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_FOR);

  node->var = tau_parser_parse_stmt_for_var(par);

  tau_parser_expect(par, TAU_TOK_KW_IN);

  node->range = tau_parser_parse_expr(par);

  tau_parser_expect(par, TAU_TOK_KW_DO);

  node->stmt = tau_parser_parse_stmt(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_for_var(tau_parser_t* par)
{
  tau_ast_decl_var_t* node = tau_ast_decl_var_init();
  node->tok = tau_parser_current(par);

  node->is_pub    = false;
  node->is_extern = false;

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->type = tau_parser_parse_type(par);
  node->expr = NULL;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_while(tau_parser_t* par)
{
  tau_ast_stmt_while_t* node = tau_ast_stmt_while_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_WHILE);

  node->cond = tau_parser_parse_expr(par);

  tau_parser_expect(par, TAU_TOK_KW_DO);

  node->stmt = tau_parser_parse_stmt(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_do_while(tau_parser_t* par)
{
  tau_ast_stmt_do_while_t* node = tau_ast_stmt_do_while_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_DO);

  node->stmt = tau_parser_parse_stmt(par);

  tau_parser_expect(par, TAU_TOK_KW_WHILE);

  node->cond = tau_parser_parse_expr(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_loop(tau_parser_t* par)
{
  tau_ast_stmt_loop_t* node = tau_ast_stmt_loop_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_LOOP);

  node->stmt = tau_parser_parse_stmt(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_break(tau_parser_t* par)
{
  tau_ast_stmt_break_t* node = tau_ast_stmt_break_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_BREAK);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_continue(tau_parser_t* par)
{
  tau_ast_stmt_continue_t* node = tau_ast_stmt_continue_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_CONTINUE);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_return(tau_parser_t* par)
{
  tau_ast_stmt_return_t* node = tau_ast_stmt_return_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_RETURN);

  node->expr = tau_parser_parse_expr(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_defer(tau_parser_t* par)
{
  tau_ast_stmt_defer_t* node = tau_ast_stmt_defer_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_KW_DEFER);

  node->stmt = tau_parser_parse_stmt(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_block(tau_parser_t* par)
{
  tau_ast_stmt_block_t* node = tau_ast_stmt_block_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACE_LEFT);

  tau_parser_parse_terminated_list(par, node->stmts, TAU_TOK_PUNCT_BRACE_RIGHT, tau_parser_parse_stmt);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt_expr(tau_parser_t* par)
{
  tau_ast_stmt_expr_t* node = tau_ast_stmt_expr_init();
  node->tok = tau_parser_current(par);

  node->expr = tau_parser_parse_expr(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_stmt(tau_parser_t* par)
{
  switch (tau_parser_current(par)->kind)
  {
  case TAU_TOK_ID:
    if (tau_parser_peek(par)->kind == TAU_TOK_PUNCT_COLON)
      return tau_parser_parse_decl_var(par);
    break;
  case TAU_TOK_KW_STRUCT:        return tau_parser_parse_decl_struct(par);
  case TAU_TOK_KW_UNION:         return tau_parser_parse_decl_union(par);
  case TAU_TOK_KW_ENUM:          return tau_parser_parse_decl_enum(par);
  case TAU_TOK_KW_IF:            return tau_parser_parse_stmt_if(par);
  case TAU_TOK_KW_FOR:           return tau_parser_parse_stmt_for(par);
  case TAU_TOK_KW_WHILE:         return tau_parser_parse_stmt_while(par);
  case TAU_TOK_KW_DO:            return tau_parser_parse_stmt_do_while(par);
  case TAU_TOK_KW_LOOP:          return tau_parser_parse_stmt_loop(par);
  case TAU_TOK_KW_BREAK:         return tau_parser_parse_stmt_break(par);
  case TAU_TOK_KW_CONTINUE:      return tau_parser_parse_stmt_continue(par);
  case TAU_TOK_KW_RETURN:        return tau_parser_parse_stmt_return(par);
  case TAU_TOK_KW_DEFER:         return tau_parser_parse_stmt_defer(par);
  case TAU_TOK_PUNCT_BRACE_LEFT: return tau_parser_parse_stmt_block(par);
  default: TAU_NOOP();
  }

  return tau_parser_parse_stmt_expr(par);
}

tau_ast_node_t* tau_parser_parse_decl_var(tau_parser_t* par)
{
  tau_ast_decl_var_t* node = tau_ast_decl_var_init();
  node->tok = tau_parser_current(par);

  node->is_pub    = par->decl_ctx.is_pub;
  node->is_extern = par->decl_ctx.is_extern;

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->type = tau_parser_parse_type(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_EQUAL);

  node->expr = tau_parser_consume(par, TAU_TOK_KW_UNDEF) ? NULL : tau_parser_parse_expr(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_fun(tau_parser_t* par)
{
  if (tau_parser_peek(par)->kind == TAU_TOK_PUNCT_LESS)
  {
    return tau_parser_parse_decl_generic_fun(par);
  }

  tau_ast_decl_fun_t* node = tau_ast_decl_fun_init();
  node->tok = tau_parser_current(par);
  node->parent = tau_stack_top(par->parents);

  node->is_pub    = par->decl_ctx.is_pub;
  node->is_extern = par->decl_ctx.is_extern;
  node->callconv  = par->decl_ctx.callconv;

  tau_parser_expect(par, TAU_TOK_KW_FUN);

  node->id = tau_parser_parse_id(par);

  // Parse parameters.
  tau_parser_expect(par, TAU_TOK_PUNCT_PAREN_LEFT);

  if (!tau_parser_consume(par, TAU_TOK_PUNCT_PAREN_RIGHT))
  {
    for (bool seen_default = false;;)
    {
      // Parse variadic parameter.
      // If a variadic parameter is present it must be the last one in the
      // parameter list.
      if (tau_parser_consume(par, TAU_TOK_PUNCT_DOT_DOT_DOT))
      {
        // Tau-style variadic parameter.
        if (tau_parser_current(par)->kind == TAU_TOK_ID)
        {
          tau_ast_decl_param_t* variadic_param = (tau_ast_decl_param_t*)tau_parser_parse_decl_param(par);
          TAU_ASSERT(variadic_param->expr == NULL);

          variadic_param->is_vararg = true;

          // Default parameters must be followed only by default parameters.
          if (seen_default)
          {
            tau_error_bag_put_parser_default_parameter_order(
              par->errors,
              tau_token_location(((tau_ast_decl_param_t*)tau_vector_front(node->params))->tok),
              tau_token_location(variadic_param->tok)
            );
          }

          tau_vector_push(node->params, variadic_param);
        }
        // C-style variadic parameter.
        else
        {
          // Function must be extern "cdecl".
          TAU_ASSERT(node->is_extern && node->callconv == TAU_CALLCONV_CDECL);
          node->is_vararg = true;
        }

        break;
      }

      // Parse non-variadic parameter.
      tau_ast_decl_param_t* param = (tau_ast_decl_param_t*)tau_parser_parse_decl_param(par);

      // Set flag variable if parameter is a default parameter.
      seen_default = seen_default || param->expr != NULL;

      // Default parameters must be followed only by default parameters.
      if (seen_default && param->expr == NULL)
      {
        tau_error_bag_put_parser_default_parameter_order(
          par->errors,
          tau_token_location(((tau_ast_decl_param_t*)tau_vector_front(node->params))->tok),
          tau_token_location(param->tok)
        );
      }

      tau_vector_push(node->params, param);

      if (!tau_parser_consume(par, TAU_TOK_PUNCT_COMMA))
        break;
    }

    tau_parser_expect(par, TAU_TOK_PUNCT_PAREN_RIGHT);
  }

  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->return_type = tau_parser_parse_type(par);

  node->stmt = !node->is_extern ? tau_parser_parse_stmt(par) : NULL;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_generic_fun(tau_parser_t* par)
{
  tau_ast_decl_generic_fun_t* node = tau_ast_decl_generic_fun_init();
  node->tok = tau_parser_current(par);
  node->is_pub = par->decl_ctx.is_pub;

  TAU_ASSERT(!par->decl_ctx.is_extern);
  TAU_ASSERT(par->decl_ctx.callconv == TAU_CALLCONV_TAU);

  tau_parser_expect(par, TAU_TOK_KW_FUN);
  tau_parser_expect(par, TAU_TOK_PUNCT_LESS);

  tau_parser_parse_delimited_list(par, node->generic_params, TAU_TOK_PUNCT_COMMA, tau_parser_parse_decl_generic_param);

  tau_parser_expect(par, TAU_TOK_PUNCT_GREATER);

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_PAREN_LEFT);

  tau_parser_parse_delimited_list(par, node->params, TAU_TOK_PUNCT_COMMA, tau_parser_parse_decl_param);

  tau_parser_expect(par, TAU_TOK_PUNCT_PAREN_RIGHT);
  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->return_type = tau_parser_parse_type(par);
  node->stmt = tau_parser_parse_stmt(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_struct(tau_parser_t* par)
{
  tau_ast_decl_struct_t* node = tau_ast_decl_struct_init();
  node->tok = tau_parser_current(par);
  node->parent = tau_stack_top(par->parents);

  TAU_ASSERT(!par->decl_ctx.is_extern);

  node->is_pub = par->decl_ctx.is_pub;

  tau_parser_expect(par, TAU_TOK_KW_STRUCT);

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACE_LEFT);

  tau_parser_parse_terminated_list(par, node->members, TAU_TOK_PUNCT_BRACE_RIGHT, tau_parser_parse_decl_struct_member);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_struct_member(tau_parser_t* par)
{
  tau_parser_decl_context_clear(par);

  if (tau_parser_current(par)->kind == TAU_TOK_KW_PUB)
    tau_parser_parse_decl_context_pub(par);

  tau_ast_decl_var_t* node = tau_ast_decl_var_init();
  node->tok = tau_parser_current(par);

  node->is_pub    = par->decl_ctx.is_pub;
  node->is_extern = false;

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->type = tau_parser_parse_type(par);
  node->expr = NULL;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_union(tau_parser_t* par)
{
  tau_ast_decl_union_t* node = tau_ast_decl_union_init();
  node->tok = tau_parser_current(par);
  node->parent = tau_stack_top(par->parents);

  TAU_ASSERT(!par->decl_ctx.is_extern);

  node->is_pub = par->decl_ctx.is_pub;

  tau_parser_expect(par, TAU_TOK_KW_UNION);

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACE_LEFT);

  tau_parser_parse_terminated_list(par, node->members, TAU_TOK_PUNCT_BRACE_RIGHT, tau_parser_parse_decl_union_member);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_union_member(tau_parser_t* par)
{
  tau_ast_decl_var_t* node = tau_ast_decl_var_init();
  node->tok = tau_parser_current(par);

  node->is_pub    = true;
  node->is_extern = false;

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->type = tau_parser_parse_type(par);
  node->expr = NULL;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_enum(tau_parser_t* par)
{
  tau_ast_decl_enum_t* node = tau_ast_decl_enum_init();
  node->tok = tau_parser_current(par);
  node->parent = tau_stack_top(par->parents);

  TAU_ASSERT(!par->decl_ctx.is_extern);

  node->is_pub = par->decl_ctx.is_pub;

  tau_parser_expect(par, TAU_TOK_KW_ENUM);

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACE_LEFT);

  tau_stack_push(par->parents, node);

  tau_parser_parse_terminated_list(par, node->members, TAU_TOK_PUNCT_BRACE_RIGHT, tau_parser_parse_decl_enum_constant);

  tau_stack_pop(par->parents);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_mod(tau_parser_t* par)
{
  tau_ast_decl_mod_t* node = tau_ast_decl_mod_init();
  node->tok = tau_parser_current(par);
  node->parent = tau_stack_top(par->parents);

  TAU_ASSERT(!par->decl_ctx.is_extern);

  node->is_pub = par->decl_ctx.is_pub;

  tau_parser_expect(par, TAU_TOK_KW_MOD);

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACE_LEFT);

  tau_stack_push(par->parents, node);

  tau_parser_parse_terminated_list(par, node->members, TAU_TOK_PUNCT_BRACE_RIGHT, tau_parser_parse_decl_in_mod);

  tau_stack_pop(par->parents);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_type_alias(tau_parser_t* par)
{
  tau_ast_decl_type_alias_t* node = tau_ast_decl_type_alias_init();
  node->tok = tau_parser_current(par);
  node->parent = tau_stack_top(par->parents);

  tau_parser_expect(par, TAU_TOK_KW_TYPE);

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_EQUAL);

  node->type = tau_parser_parse_type(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl(tau_parser_t* par)
{
  switch (tau_parser_current(par)->kind)
  {
  case TAU_TOK_ID:         return tau_parser_parse_decl_var       (par);
  case TAU_TOK_KW_FUN:     return tau_parser_parse_decl_fun       (par);
  case TAU_TOK_KW_STRUCT:  return tau_parser_parse_decl_struct    (par);
  case TAU_TOK_KW_UNION:   return tau_parser_parse_decl_union     (par);
  case TAU_TOK_KW_ENUM:    return tau_parser_parse_decl_enum      (par);
  case TAU_TOK_KW_MOD:     return tau_parser_parse_decl_mod       (par);
  case TAU_TOK_KW_TYPE:    return tau_parser_parse_decl_type_alias(par);
  default:
  {
    tau_error_bag_put_parser_unexpected_token(par->errors, tau_token_location(tau_parser_current(par)));

    return (tau_ast_node_t*)tau_ast_poison_init();
  }
  }

  return NULL;
}

tau_ast_node_t* tau_parser_parse_decl_in_mod(tau_parser_t* par)
{
  tau_parser_decl_context_clear(par);

  if (tau_parser_current(par)->kind == TAU_TOK_KW_PUB)
    tau_parser_parse_decl_context_pub(par);

  if (tau_parser_current(par)->kind == TAU_TOK_KW_EXTERN)
    tau_parser_parse_decl_context_extern(par);

  return tau_parser_parse_decl(par);
}

tau_ast_node_t* tau_parser_parse_decl_top_level(tau_parser_t* par)
{
  if (tau_parser_current(par)->kind == TAU_TOK_KW_USE)
    return tau_parser_parse_use_directive(par);

  tau_parser_decl_context_clear(par);

  if (tau_parser_current(par)->kind == TAU_TOK_KW_PUB)
    tau_parser_parse_decl_context_pub(par);

  if (tau_parser_current(par)->kind == TAU_TOK_KW_EXTERN)
    tau_parser_parse_decl_context_extern(par);

  return tau_parser_parse_decl(par);
}

tau_ast_node_t* tau_parser_parse_decl_param(tau_parser_t* par)
{
  tau_ast_decl_param_t* node = tau_ast_decl_param_init();
  node->tok = tau_parser_current(par);

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->type = tau_parser_parse_type(par);

  node->expr = tau_parser_consume(par, TAU_TOK_PUNCT_EQUAL) ? tau_parser_parse_expr(par) : NULL;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_generic_param(tau_parser_t* par)
{
  tau_ast_decl_generic_param_t* node = tau_ast_decl_generic_param_init();
  node->tok = tau_parser_current(par);

  node->id = tau_parser_parse_id(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_COLON);

  node->type = tau_parser_parse_type(par);

  node->expr = tau_parser_consume(par, TAU_TOK_PUNCT_EQUAL) ? tau_parser_parse_expr(par) : NULL;

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_decl_enum_constant(tau_parser_t* par)
{
  tau_ast_decl_enum_constant_t* node = tau_ast_decl_enum_constant_init();
  node->tok = tau_parser_current(par);
  node->parent = tau_stack_top(par->parents);

  node->id = tau_parser_parse_id(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_path_segment(tau_parser_t* par)
{
  tau_ast_path_segment_t* node = tau_ast_path_segment_init();
  node->tok = tau_parser_current(par);

  node->id = tau_parser_parse_id(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_path_wildcard(tau_parser_t* par)
{
  tau_ast_path_wildcard_t* node = tau_ast_path_wildcard_init();
  node->tok = tau_parser_expect(par, TAU_TOK_PUNCT_ASTERISK);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_path_list(tau_parser_t* par)
{
  tau_ast_path_list_t* node = tau_ast_path_list_init();
  node->tok = tau_parser_current(par);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACKET_LEFT);

  tau_parser_parse_delimited_list(par, node->paths, TAU_TOK_PUNCT_COMMA, tau_parser_parse_path);

  tau_parser_expect(par, TAU_TOK_PUNCT_BRACKET_RIGHT);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse_path(tau_parser_t* par)
{
  tau_ast_node_t* node = tau_parser_parse_path_segment(par);

  while (tau_parser_consume(par, TAU_TOK_PUNCT_DOT))
  {
    tau_ast_path_access_t* access_node = tau_ast_path_access_init();
    access_node->tok = tau_parser_current(par);

    access_node->lhs = node;
    node = (tau_ast_node_t*)access_node;

    switch (tau_parser_current(par)->kind)
    {
    case TAU_TOK_ID:                 access_node->rhs = tau_parser_parse_path_segment (par); break;
    case TAU_TOK_PUNCT_ASTERISK:     access_node->rhs = tau_parser_parse_path_wildcard(par); return node;
    case TAU_TOK_PUNCT_BRACKET_LEFT: access_node->rhs = tau_parser_parse_path_list    (par); return node;
    default:
    {
      tau_error_bag_put_parser_unexpected_token(par->errors, tau_token_location(tau_parser_current(par)));

      return (tau_ast_node_t*)tau_ast_poison_init();
    }
    }
  }

  if (tau_parser_consume(par, TAU_TOK_KW_AS))
  {
    tau_ast_path_alias_t* alias_node = tau_ast_path_alias_init();
    alias_node->tok = tau_parser_current(par);

    alias_node->path = node;
    alias_node->id = tau_parser_parse_id(par);

    return (tau_ast_node_t*)alias_node;
  }

  return node;
}

tau_ast_node_t* tau_parser_parse_use_directive(tau_parser_t* par)
{
  tau_ast_use_t* node = tau_ast_use_init();
  node->tok = tau_parser_expect(par, TAU_TOK_KW_USE);

  node->path = tau_parser_parse_path(par);

  return (tau_ast_node_t*)node;
}

tau_ast_node_t* tau_parser_parse(tau_parser_t* par, tau_vector_t* tokens, tau_error_bag_t* errors)
{
  par->tokens = tokens;
  par->cur = 0;
  par->errors = errors;

  tau_stack_clear(par->parents);
  tau_stack_push(par->parents, NULL);

  tau_parser_set_ignore_newline(par, true);
  tau_parser_decl_context_clear(par);

  tau_ast_prog_t* root = tau_ast_prog_init();
  root->tok = tau_parser_current(par);

  while (!tau_parser_consume(par, TAU_TOK_EOF))
  {
    tau_vector_push(root->decls, tau_parser_parse_decl_top_level(par));

    if (tau_error_bag_full(par->errors))
      break;
  }

  return (tau_ast_node_t*)root;
}
