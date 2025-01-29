/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/parser.h"

#include "stages/parser/shyd.h"

static void parser_skip_newlines(parser_t* par)
{
  token_t* tok = (token_t*)vector_get(par->tokens, par->cur);

  while (tok->kind == TOK_NEWLINE)
  {
    par->cur++;
    tok = (token_t*)vector_get(par->tokens, par->cur);
  }
}

static ast_node_t* parser_cstr_to_prim(const char* cstr)
{
  if (strncmp(cstr, "i8" , 2) == 0) return (ast_node_t*)ast_type_prim_i8_init();
  if (strncmp(cstr, "u8" , 2) == 0) return (ast_node_t*)ast_type_prim_u8_init();
  if (strncmp(cstr, "i16", 3) == 0) return (ast_node_t*)ast_type_prim_i16_init();
  if (strncmp(cstr, "u16", 3) == 0) return (ast_node_t*)ast_type_prim_u16_init();
  if (strncmp(cstr, "i32", 3) == 0) return (ast_node_t*)ast_type_prim_i32_init();
  if (strncmp(cstr, "u32", 3) == 0) return (ast_node_t*)ast_type_prim_u32_init();
  if (strncmp(cstr, "i64", 3) == 0) return (ast_node_t*)ast_type_prim_i64_init();
  if (strncmp(cstr, "u64", 3) == 0) return (ast_node_t*)ast_type_prim_u64_init();
  if (strncmp(cstr, "f32", 3) == 0) return (ast_node_t*)ast_type_prim_f32_init();
  if (strncmp(cstr, "f64", 3) == 0) return (ast_node_t*)ast_type_prim_f64_init();

  UNREACHABLE();
}

parser_t* parser_init(void)
{
  parser_t* par = (parser_t*)malloc(sizeof(parser_t));
  CLEAROBJ(par);

  par->parents = stack_init();

  return par;
}

void parser_free(parser_t* par)
{
  stack_free(par->parents);
  free(par);
}

token_t* parser_current(parser_t* par)
{
  if (par->ignore_newlines)
    parser_skip_newlines(par);

  return (token_t*)vector_get(par->tokens, par->cur);
}

token_t* parser_next(parser_t* par)
{
  token_t* tok = parser_current(par);

  if (tok->kind != TOK_EOF)
    par->cur++;

  return tok;
}

token_t* parser_peek(parser_t* par)
{
  token_t* tok = parser_current(par);

  if (tok->kind == TOK_EOF)
    return tok;

  size_t next = par->cur + 1;

  while (par->ignore_newlines && (tok = (token_t*)vector_get(par->tokens, next))->kind == TOK_NEWLINE)
    next++;

  return tok;
}

bool parser_consume(parser_t* par, token_kind_t kind)
{
  if (parser_current(par)->kind == kind)
  {
    parser_next(par);
    return true;
  }

  return false;
}

token_t* parser_expect(parser_t* par, token_kind_t kind)
{
  token_t* token = parser_next(par);

  if (token->kind != kind)
  {
    error_bag_put_parser_unexpected_token(par->errors, token_location(token));
    return NULL;
  }

  return token;
}

bool parser_get_ignore_newline(parser_t* par)
{
  return par->ignore_newlines;
}

void parser_set_ignore_newline(parser_t* par, bool ignore)
{
  par->ignore_newlines = ignore;
}

void parser_decl_context_clear(parser_t* par)
{
  par->decl_ctx.is_pub    = false;
  par->decl_ctx.is_extern = false;
  par->decl_ctx.callconv  = CALLCONV_TAU;
}

void parser_parse_decl_context_pub(parser_t* par)
{
  parser_expect(par, TOK_KW_PUB);

  par->decl_ctx.is_pub = true;
}

void parser_parse_decl_context_extern(parser_t* par)
{
  parser_expect(par, TOK_KW_EXTERN);

  par->decl_ctx.is_extern = true;
  par->decl_ctx.callconv = CALLCONV_CDECL;

  if (parser_current(par)->kind == TOK_LIT_STR)
    par->decl_ctx.callconv = parser_parse_callconv(par);
}

void parser_parse_delimited_list(parser_t* par, vector_t* dest, token_kind_t delim, parse_func_t parse_func)
{
  for (;;)
  {
    vector_push(dest, parse_func(par));

    if (!parser_consume(par, delim))
      break;
  }
}

void parser_parse_terminated_list(parser_t* par, vector_t* dest, token_kind_t termin, parse_func_t parse_func)
{
  while (!parser_consume(par, termin))
    vector_push(dest, parse_func(par));
}

callconv_kind_t parser_parse_callconv(parser_t* par)
{
  static struct {
    const char* str;
    callconv_kind_t callconv;
  } str_callconv_map[] = {
    { "\"Tau\"",        CALLCONV_TAU        },
    { "\"C\"",          CALLCONV_CDECL      },
    { "\"cdecl\"",      CALLCONV_CDECL      },
    { "\"stdcall\"",    CALLCONV_STDCALL    },
    { "\"win64\"",      CALLCONV_WIN64      },
    { "\"sysv64\"",     CALLCONV_SYSV64     },
    { "\"aapcs\"",      CALLCONV_AAPCS      },
    { "\"fastcall\"",   CALLCONV_FASTCALL   },
    { "\"vectorcall\"", CALLCONV_VECTORCALL },
    { "\"thiscall\"",   CALLCONV_THISCALL   },
  };

  token_t* callconv_token = parser_next(par);

  if (callconv_token->kind != TOK_LIT_STR)
  {
    error_bag_put_parser_expected_calling_convention(par->errors, token_location(callconv_token));
    return CALLCONV_UNKNOWN;
  }

  string_view_t callconv_str_view = token_to_string_view(callconv_token);

  for (size_t i = 0; i < COUNTOF(str_callconv_map); i++)
    if (string_view_compare_cstr(callconv_str_view, str_callconv_map[i].str) == 0)
      return str_callconv_map[i].callconv;

  error_bag_put_parser_unknown_calling_convention(par->errors, token_location(callconv_token));

  return CALLCONV_UNKNOWN;
}

ast_node_t* parser_parse_id(parser_t* par)
{
  ast_id_t* node = ast_id_init();

  node->tok = parser_expect(par, TOK_ID);

  if (node->tok == NULL)
    return (ast_node_t*)ast_poison_init();

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_id(parser_t* par)
{
  ast_type_id_t* node = ast_type_id_init();

  node->tok = parser_expect(par, TOK_ID);

  if (node->tok == NULL)
    return (ast_node_t*)ast_poison_init();

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_mut(parser_t* par)
{
  ast_type_mut_t* node = ast_type_mut_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_MUT);

  node->base_type = parser_parse_type(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_ptr(parser_t* par)
{
  ast_type_ptr_t* node = ast_type_ptr_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_ASTERISK);

  node->base_type = parser_parse_type(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_array(parser_t* par)
{
  ast_type_array_t* node = ast_type_array_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_BRACKET_LEFT);

  node->size = parser_current(par)->kind == TOK_PUNCT_BRACKET_RIGHT ? NULL : parser_parse_expr(par);

  parser_expect(par, TOK_PUNCT_BRACKET_RIGHT);

  node->base_type = parser_parse_type(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_ref(parser_t* par)
{
  ast_type_ref_t* node = ast_type_ref_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_AMPERSAND);

  node->base_type = parser_parse_type(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_opt(parser_t* par)
{
  ast_type_opt_t* node = ast_type_opt_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_QUESTION);

  node->base_type = parser_parse_type(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_fun(parser_t* par)
{
  ast_type_fun_t* node = ast_type_fun_init();
  node->tok = parser_current(par);

  node->callconv = CALLCONV_TAU;

  if (parser_consume(par, TOK_KW_EXTERN))
    node->callconv = parser_parse_callconv(par);

  parser_expect(par, TOK_KW_FUN);
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  parser_parse_delimited_list(par, node->params, TOK_PUNCT_COMMA, parser_parse_type);

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  parser_expect(par, TOK_PUNCT_COLON);

  node->return_type = parser_parse_type(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_vec(parser_t* par)
{
  ast_type_vec_t* node = ast_type_vec_init();
  node->tok = parser_next(par);

  string_view_t tok_view = token_to_string_view(node->tok);
  const char* tok_cstr = string_view_begin(tok_view);

  const char* size_end = NULL;
  size_t size = strtoull(tok_cstr + 3, &size_end, 10);

  ASSERT(errno != ERANGE);
  ASSERT(size > 0);

  node->size = size;

  node->base_type = parser_cstr_to_prim(size_end);
  node->base_type->tok = node->tok;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_mat(parser_t* par)
{
  ast_type_mat_t* node = ast_type_mat_init();
  node->tok = parser_next(par);

  string_view_t tok_view = token_to_string_view(node->tok);
  const char* tok_cstr = string_view_begin(tok_view);

  const char* rows_end = NULL;
  size_t rows = strtoull(tok_cstr + 3, &rows_end, 10);

  ASSERT(errno != ERANGE);
  ASSERT(rows > 0);

  node->rows = rows;

  const char* size_end = rows_end;

  if (*rows_end == 'x')
  {
    const char* cols_end = NULL;
    size_t cols = strtoull(rows_end + 1, &cols_end, 10);

    ASSERT(errno != ERANGE);
    ASSERT(cols > 0);

    node->cols = cols;

    size_end = cols_end;
  }
  else
    node->cols = rows;

  node->base_type = parser_cstr_to_prim(size_end);
  node->base_type->tok = node->tok;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_member(parser_t* par)
{
  ast_node_t* node = parser_parse_type_id(par);

  while (parser_consume(par, TOK_PUNCT_DOT))
  {
    ast_type_mbr_t* member_node = ast_type_mbr_init();
    member_node->tok = parser_current(par);

    member_node->parent = node;
    member_node->member = parser_parse_type_id(par);

    node = (ast_node_t*)member_node;
  }

  return node;
}

ast_node_t* parser_parse_type(parser_t* par)
{
  ast_node_t* node = NULL;

  switch (parser_current(par)->kind)
  {
  case TOK_ID:                 return parser_parse_type_member  (par);
  case TOK_KW_MUT:             return parser_parse_type_mut     (par);
  case TOK_PUNCT_ASTERISK:     return parser_parse_type_ptr     (par);
  case TOK_PUNCT_BRACKET_LEFT: return parser_parse_type_array   (par);
  case TOK_PUNCT_AMPERSAND:    return parser_parse_type_ref     (par);
  case TOK_PUNCT_QUESTION:     return parser_parse_type_opt     (par);
  case TOK_KW_FUN:             return parser_parse_type_fun     (par);
  case TOK_KW_VEC:             return parser_parse_type_vec     (par);
  case TOK_KW_MAT:             return parser_parse_type_mat     (par);
  case TOK_KW_I8:
    node = (ast_node_t*)ast_type_prim_i8_init();
    node->tok = parser_expect(par, TOK_KW_I8);
    break;
  case TOK_KW_I16:
    node = (ast_node_t*)ast_type_prim_i16_init();
    node->tok = parser_expect(par, TOK_KW_I16);
    break;
  case TOK_KW_I32:
    node = (ast_node_t*)ast_type_prim_i32_init();
    node->tok = parser_expect(par, TOK_KW_I32);
    break;
  case TOK_KW_I64:
    node = (ast_node_t*)ast_type_prim_i64_init();
    node->tok = parser_expect(par, TOK_KW_I64);
    break;
  case TOK_KW_ISIZE:
    node = (ast_node_t*)ast_type_prim_isize_init();
    node->tok = parser_expect(par, TOK_KW_ISIZE);
    break;
  case TOK_KW_U8:
    node = (ast_node_t*)ast_type_prim_u8_init();
    node->tok = parser_expect(par, TOK_KW_U8);
    break;
  case TOK_KW_U16:
    node = (ast_node_t*)ast_type_prim_u16_init();
    node->tok = parser_expect(par, TOK_KW_U16);
    break;
  case TOK_KW_U32:
    node = (ast_node_t*)ast_type_prim_u32_init();
    node->tok = parser_expect(par, TOK_KW_U32);
    break;
  case TOK_KW_U64:
    node = (ast_node_t*)ast_type_prim_u64_init();
    node->tok = parser_expect(par, TOK_KW_U64);
    break;
  case TOK_KW_USIZE:
    node = (ast_node_t*)ast_type_prim_usize_init();
    node->tok = parser_expect(par, TOK_KW_USIZE);
    break;
  case TOK_KW_F32:
    node = (ast_node_t*)ast_type_prim_f32_init();
    node->tok = parser_expect(par, TOK_KW_F32);
    break;
  case TOK_KW_F64:
    node = (ast_node_t*)ast_type_prim_f64_init();
    node->tok = parser_expect(par, TOK_KW_F64);
    break;
  case TOK_KW_C64:
    node = (ast_node_t*)ast_type_prim_c64_init();
    node->tok = parser_expect(par, TOK_KW_C64);
    break;
  case TOK_KW_C128:
    node = (ast_node_t*)ast_type_prim_c128_init();
    node->tok = parser_expect(par, TOK_KW_C128);
    break;
  case TOK_KW_CHAR:
    node = (ast_node_t*)ast_type_prim_char_init();
    node->tok = parser_expect(par, TOK_KW_CHAR);
    break;
  case TOK_KW_BOOL:
    node = (ast_node_t*)ast_type_prim_bool_init();
    node->tok = parser_expect(par, TOK_KW_BOOL);
    break;
  case TOK_KW_UNIT:
    node = (ast_node_t*)ast_type_prim_unit_init();
    node->tok = parser_expect(par, TOK_KW_UNIT);
    break;
  case TOK_KW_TYPE:
    node = (ast_node_t*)ast_type_type_init();
    node->tok = parser_expect(par, TOK_KW_TYPE);
    break;
  default:
  {
    error_bag_put_parser_unexpected_token(par->errors, token_location(parser_current(par)));

    return (ast_node_t*)ast_poison_init();
  }
  }

  if (node->tok == NULL)
    return (ast_node_t*)ast_poison_init();

  return node;
}

ast_node_t* parser_parse_expr(parser_t* par)
{
  return shyd_parse_expr(par);
}

ast_node_t* parser_parse_stmt_if(parser_t* par)
{
  ast_stmt_if_t* node = ast_stmt_if_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_IF);

  node->cond = parser_parse_expr(par);

  parser_expect(par, TOK_KW_THEN);

  node->stmt = parser_parse_stmt(par);
  node->stmt_else = parser_consume(par, TOK_KW_ELSE) ? parser_parse_stmt(par) : NULL;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_for(parser_t* par)
{
  ast_stmt_for_t* node = ast_stmt_for_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_FOR);

  node->var = parser_parse_stmt_for_var(par);

  parser_expect(par, TOK_KW_IN);

  node->range = parser_parse_expr(par);

  parser_expect(par, TOK_KW_DO);

  node->stmt = parser_parse_stmt(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_for_var(parser_t* par)
{
  ast_decl_var_t* node = ast_decl_var_init();
  node->tok = parser_current(par);

  node->is_pub    = false;
  node->is_extern = false;

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type = parser_parse_type(par);
  node->expr = NULL;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_while(parser_t* par)
{
  ast_stmt_while_t* node = ast_stmt_while_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_WHILE);

  node->cond = parser_parse_expr(par);

  parser_expect(par, TOK_KW_DO);

  node->stmt = parser_parse_stmt(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_do_while(parser_t* par)
{
  ast_stmt_do_while_t* node = ast_stmt_do_while_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_DO);

  node->stmt = parser_parse_stmt(par);

  parser_expect(par, TOK_KW_WHILE);

  node->cond = parser_parse_expr(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_loop(parser_t* par)
{
  ast_stmt_loop_t* node = ast_stmt_loop_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_LOOP);

  node->stmt = parser_parse_stmt(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_break(parser_t* par)
{
  ast_stmt_break_t* node = ast_stmt_break_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_BREAK);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_continue(parser_t* par)
{
  ast_stmt_continue_t* node = ast_stmt_continue_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_CONTINUE);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_return(parser_t* par)
{
  ast_stmt_return_t* node = ast_stmt_return_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_RETURN);

  node->expr = parser_parse_expr(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_defer(parser_t* par)
{
  ast_stmt_defer_t* node = ast_stmt_defer_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_DEFER);

  node->stmt = parser_parse_stmt(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_block(parser_t* par)
{
  ast_stmt_block_t* node = ast_stmt_block_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  parser_parse_terminated_list(par, node->stmts, TOK_PUNCT_BRACE_RIGHT, parser_parse_stmt);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_expr(parser_t* par)
{
  ast_stmt_expr_t* node = ast_stmt_expr_init();
  node->tok = parser_current(par);

  node->expr = parser_parse_expr(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt(parser_t* par)
{
  switch (parser_current(par)->kind)
  {
  case TOK_ID:
    if (parser_peek(par)->kind == TOK_PUNCT_COLON)
      return parser_parse_decl_var(par);
    break;
  case TOK_KW_STRUCT:        return parser_parse_decl_struct(par);
  case TOK_KW_UNION:         return parser_parse_decl_union(par);
  case TOK_KW_ENUM:          return parser_parse_decl_enum(par);
  case TOK_KW_IF:            return parser_parse_stmt_if(par);
  case TOK_KW_FOR:           return parser_parse_stmt_for(par);
  case TOK_KW_WHILE:         return parser_parse_stmt_while(par);
  case TOK_KW_DO:            return parser_parse_stmt_do_while(par);
  case TOK_KW_LOOP:          return parser_parse_stmt_loop(par);
  case TOK_KW_BREAK:         return parser_parse_stmt_break(par);
  case TOK_KW_CONTINUE:      return parser_parse_stmt_continue(par);
  case TOK_KW_RETURN:        return parser_parse_stmt_return(par);
  case TOK_KW_DEFER:         return parser_parse_stmt_defer(par);
  case TOK_PUNCT_BRACE_LEFT: return parser_parse_stmt_block(par);
  default: NOOP();
  }

  return parser_parse_stmt_expr(par);
}

ast_node_t* parser_parse_decl_var(parser_t* par)
{
  ast_decl_var_t* node = ast_decl_var_init();
  node->tok = parser_current(par);

  node->is_pub    = par->decl_ctx.is_pub;
  node->is_extern = par->decl_ctx.is_extern;

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type = parser_parse_type(par);

  parser_expect(par, TOK_PUNCT_EQUAL);

  node->expr = parser_consume(par, TOK_KW_UNDEF) ? NULL : parser_parse_expr(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_fun(parser_t* par)
{
  if (parser_peek(par)->kind == TOK_PUNCT_LESS)
  {
    return parser_parse_decl_generic_fun(par);
  }

  ast_decl_fun_t* node = ast_decl_fun_init();
  node->tok = parser_current(par);
  node->parent = stack_top(par->parents);

  node->is_pub    = par->decl_ctx.is_pub;
  node->is_extern = par->decl_ctx.is_extern;
  node->callconv  = par->decl_ctx.callconv;

  parser_expect(par, TOK_KW_FUN);

  node->id = parser_parse_id(par);

  // Parse parameters.
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  if (!parser_consume(par, TOK_PUNCT_PAREN_RIGHT))
  {
    for (bool seen_default = false;;)
    {
      // Parse variadic parameter.
      // If a variadic parameter is present it must be the last one in the
      // parameter list.
      if (parser_consume(par, TOK_PUNCT_DOT_DOT_DOT))
      {
        // Tau-style variadic parameter.
        if (parser_current(par)->kind == TOK_ID)
        {
          ast_decl_param_t* variadic_param = (ast_decl_param_t*)parser_parse_decl_param(par);
          ASSERT(variadic_param->expr == NULL);

          variadic_param->is_vararg = true;

          // Default parameters must be followed only by default parameters.
          if (seen_default)
          {
            error_bag_put_parser_default_parameter_order(
              par->errors,
              token_location(((ast_decl_param_t*)vector_front(node->params))->tok),
              token_location(variadic_param->tok)
            );
          }

          vector_push(node->params, variadic_param);
        }
        // C-style variadic parameter.
        else
        {
          // Function must be extern "cdecl".
          ASSERT(node->is_extern && node->callconv == CALLCONV_CDECL);
          node->is_vararg = true;
        }

        break;
      }

      // Parse non-variadic parameter.
      ast_decl_param_t* param = (ast_decl_param_t*)parser_parse_decl_param(par);

      // Set flag variable if parameter is a default parameter.
      seen_default = seen_default || param->expr != NULL;

      // Default parameters must be followed only by default parameters.
      if (seen_default && param->expr == NULL)
      {
        error_bag_put_parser_default_parameter_order(
          par->errors,
          token_location(((ast_decl_param_t*)vector_front(node->params))->tok),
          token_location(param->tok)
        );
      }

      vector_push(node->params, param);

      if (!parser_consume(par, TOK_PUNCT_COMMA))
        break;
    }

    parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  }

  parser_expect(par, TOK_PUNCT_COLON);

  node->return_type = parser_parse_type(par);

  node->stmt = !node->is_extern ? parser_parse_stmt(par) : NULL;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_generic_fun(parser_t* par)
{
  ast_decl_generic_fun_t* node = ast_decl_generic_fun_init();
  node->tok = parser_current(par);
  node->is_pub = par->decl_ctx.is_pub;

  ASSERT(!par->decl_ctx.is_extern);
  ASSERT(par->decl_ctx.callconv == CALLCONV_TAU);

  parser_expect(par, TOK_KW_FUN);
  parser_expect(par, TOK_PUNCT_LESS);

  parser_parse_delimited_list(par, node->generic_params, TOK_PUNCT_COMMA, parser_parse_decl_generic_param);

  parser_expect(par, TOK_PUNCT_GREATER);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  parser_parse_delimited_list(par, node->params, TOK_PUNCT_COMMA, parser_parse_decl_param);

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  parser_expect(par, TOK_PUNCT_COLON);

  node->return_type = parser_parse_type(par);
  node->stmt = parser_parse_stmt(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_struct(parser_t* par)
{
  ast_decl_struct_t* node = ast_decl_struct_init();
  node->tok = parser_current(par);
  node->parent = stack_top(par->parents);

  ASSERT(!par->decl_ctx.is_extern);

  node->is_pub = par->decl_ctx.is_pub;

  parser_expect(par, TOK_KW_STRUCT);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  parser_parse_terminated_list(par, node->members, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_struct_member);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_struct_member(parser_t* par)
{
  parser_decl_context_clear(par);

  if (parser_current(par)->kind == TOK_KW_PUB)
    parser_parse_decl_context_pub(par);

  ast_decl_var_t* node = ast_decl_var_init();
  node->tok = parser_current(par);

  node->is_pub    = par->decl_ctx.is_pub;
  node->is_extern = false;

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type = parser_parse_type(par);
  node->expr = NULL;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_union(parser_t* par)
{
  ast_decl_union_t* node = ast_decl_union_init();
  node->tok = parser_current(par);
  node->parent = stack_top(par->parents);

  ASSERT(!par->decl_ctx.is_extern);

  node->is_pub = par->decl_ctx.is_pub;

  parser_expect(par, TOK_KW_UNION);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  parser_parse_terminated_list(par, node->members, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_union_member);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_union_member(parser_t* par)
{
  ast_decl_var_t* node = ast_decl_var_init();
  node->tok = parser_current(par);

  node->is_pub    = true;
  node->is_extern = false;

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type = parser_parse_type(par);
  node->expr = NULL;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_enum(parser_t* par)
{
  ast_decl_enum_t* node = ast_decl_enum_init();
  node->tok = parser_current(par);
  node->parent = stack_top(par->parents);

  ASSERT(!par->decl_ctx.is_extern);

  node->is_pub = par->decl_ctx.is_pub;

  parser_expect(par, TOK_KW_ENUM);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  stack_push(par->parents, node);

  parser_parse_terminated_list(par, node->members, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_enum_constant);

  stack_pop(par->parents);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_mod(parser_t* par)
{
  ast_decl_mod_t* node = ast_decl_mod_init();
  node->tok = parser_current(par);
  node->parent = stack_top(par->parents);

  ASSERT(!par->decl_ctx.is_extern);

  node->is_pub = par->decl_ctx.is_pub;

  parser_expect(par, TOK_KW_MOD);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  stack_push(par->parents, node);

  parser_parse_terminated_list(par, node->members, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_in_mod);

  stack_pop(par->parents);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_type_alias(parser_t* par)
{
  ast_decl_type_alias_t* node = ast_decl_type_alias_init();
  node->tok = parser_current(par);
  node->parent = stack_top(par->parents);

  parser_expect(par, TOK_KW_TYPE);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_EQUAL);

  node->type = parser_parse_type(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl(parser_t* par)
{
  switch (parser_current(par)->kind)
  {
  case TOK_ID:         return parser_parse_decl_var       (par);
  case TOK_KW_FUN:     return parser_parse_decl_fun       (par);
  case TOK_KW_STRUCT:  return parser_parse_decl_struct    (par);
  case TOK_KW_UNION:   return parser_parse_decl_union     (par);
  case TOK_KW_ENUM:    return parser_parse_decl_enum      (par);
  case TOK_KW_MOD:     return parser_parse_decl_mod       (par);
  case TOK_KW_TYPE:    return parser_parse_decl_type_alias(par);
  default:
  {
    error_bag_put_parser_unexpected_token(par->errors, token_location(parser_current(par)));

    return (ast_node_t*)ast_poison_init();
  }
  }

  return NULL;
}

ast_node_t* parser_parse_decl_in_mod(parser_t* par)
{
  parser_decl_context_clear(par);

  if (parser_current(par)->kind == TOK_KW_PUB)
    parser_parse_decl_context_pub(par);

  if (parser_current(par)->kind == TOK_KW_EXTERN)
    parser_parse_decl_context_extern(par);

  return parser_parse_decl(par);
}

ast_node_t* parser_parse_decl_top_level(parser_t* par)
{
  if (parser_current(par)->kind == TOK_KW_USE)
    return parser_parse_use_directive(par);

  parser_decl_context_clear(par);

  if (parser_current(par)->kind == TOK_KW_PUB)
    parser_parse_decl_context_pub(par);

  if (parser_current(par)->kind == TOK_KW_EXTERN)
    parser_parse_decl_context_extern(par);

  return parser_parse_decl(par);
}

ast_node_t* parser_parse_decl_param(parser_t* par)
{
  ast_decl_param_t* node = ast_decl_param_init();
  node->tok = parser_current(par);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type = parser_parse_type(par);

  node->expr = parser_consume(par, TOK_PUNCT_EQUAL) ? parser_parse_expr(par) : NULL;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_generic_param(parser_t* par)
{
  ast_decl_generic_param_t* node = ast_decl_generic_param_init();
  node->tok = parser_current(par);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type = parser_parse_type(par);

  node->expr = parser_consume(par, TOK_PUNCT_EQUAL) ? parser_parse_expr(par) : NULL;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_enum_constant(parser_t* par)
{
  ast_decl_enum_constant_t* node = ast_decl_enum_constant_init();
  node->tok = parser_current(par);
  node->parent = stack_top(par->parents);

  node->id = parser_parse_id(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_path_segment(parser_t* par)
{
  ast_path_segment_t* node = ast_path_segment_init();
  node->tok = parser_current(par);

  node->id = parser_parse_id(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_path_wildcard(parser_t* par)
{
  ast_path_wildcard_t* node = ast_path_wildcard_init();
  node->tok = parser_expect(par, TOK_PUNCT_ASTERISK);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_path_list(parser_t* par)
{
  ast_path_list_t* node = ast_path_list_init();
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_BRACKET_LEFT);

  parser_parse_delimited_list(par, node->paths, TOK_PUNCT_COMMA, parser_parse_path);

  parser_expect(par, TOK_PUNCT_BRACKET_RIGHT);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_path(parser_t* par)
{
  ast_node_t* node = parser_parse_path_segment(par);

  while (parser_consume(par, TOK_PUNCT_DOT))
  {
    ast_path_access_t* access_node = ast_path_access_init();
    access_node->tok = parser_current(par);

    access_node->lhs = node;
    node = (ast_node_t*)access_node;

    switch (parser_current(par)->kind)
    {
    case TOK_ID:                 access_node->rhs = parser_parse_path_segment (par); break;
    case TOK_PUNCT_ASTERISK:     access_node->rhs = parser_parse_path_wildcard(par); return node;
    case TOK_PUNCT_BRACKET_LEFT: access_node->rhs = parser_parse_path_list    (par); return node;
    default:
    {
      error_bag_put_parser_unexpected_token(par->errors, token_location(parser_current(par)));

      return (ast_node_t*)ast_poison_init();
    }
    }
  }

  if (parser_consume(par, TOK_KW_AS))
  {
    ast_path_alias_t* alias_node = ast_path_alias_init();
    alias_node->tok = parser_current(par);

    alias_node->path = node;
    alias_node->id = parser_parse_id(par);

    return (ast_node_t*)alias_node;
  }

  return node;
}

ast_node_t* parser_parse_use_directive(parser_t* par)
{
  ast_use_t* node = ast_use_init();
  node->tok = parser_expect(par, TOK_KW_USE);

  node->path = parser_parse_path(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse(parser_t* par, vector_t* tokens, error_bag_t* errors)
{
  par->tokens = tokens;
  par->cur = 0;
  par->errors = errors;

  stack_clear(par->parents);
  stack_push(par->parents, NULL);

  parser_set_ignore_newline(par, true);
  parser_decl_context_clear(par);

  ast_prog_t* root = ast_prog_init();
  root->tok = parser_current(par);

  while (!parser_consume(par, TOK_EOF))
  {
    vector_push(root->decls, parser_parse_decl_top_level(par));

    if (error_bag_full(par->errors))
      break;
  }

  return (ast_node_t*)root;
}
