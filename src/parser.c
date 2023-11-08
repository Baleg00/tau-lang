/**
 * \file parser.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "crumb.h"
#include "diagnostics.h"
#include "location.h"
#include "log.h"
#include "memtrace.h"
#include "shyd.h"
#include "util.h"

struct parser_t
{
  list_t* toks; // List of tokens to be processed.
  list_node_t* cur; // Current token in list.
};

static abi_kind_t parser_parse_abi(parser_t* par)
{
  static struct {
    const char* str;
    abi_kind_t abi;
  } str_abi_map[] = {
    { "\"Tau\"",        ABI_TAU        },
    { "\"cdecl\"",      ABI_CDECL      },
    { "\"stdcall\"",    ABI_STDCALL    },
    { "\"win64\"",      ABI_WIN64      },
    { "\"sysv64\"",     ABI_SYSV64     },
    { "\"aapcs\"",      ABI_AAPCS      },
    { "\"fastcall\"",   ABI_FASTCALL   },
    { "\"vectorcall\"", ABI_VECTORCALL },
    { "\"thiscall\"",   ABI_THISCALL   },
  };

  token_t* abi_tok = parser_expect(par, TOK_LIT_STR);
  string_view_t abi_str_view = token_to_string_view(abi_tok);

  for (size_t i = 0; i < countof(str_abi_map); i++)
    if (string_view_compare_cstr(abi_str_view, str_abi_map[i].str) == 0)
      return str_abi_map[i].abi;

  report_error_unknown_abi(abi_tok->loc);

  return 0;
}

parser_t* parser_init(void)
{
  parser_t* par = (parser_t*)malloc(sizeof(parser_t));
  memset(par, 0, sizeof(parser_t));
  return par;
}

void parser_free(parser_t* par)
{
  free(par);
}

token_t* parser_current(parser_t* par)
{
  return (token_t*)list_node_get(par->cur);
}

token_t* parser_next(parser_t* par)
{
  token_t* tok = parser_current(par);
  assert(tok != NULL);

  if (tok->kind != TOK_EOF)
    par->cur = list_node_next(par->cur);

  return tok;
}

token_t* parser_peek(parser_t* par)
{
  token_t* tok = parser_current(par);
  assert(tok != NULL);

  if (tok->kind == TOK_EOF)
    return tok;

  return (token_t*)list_node_get(list_node_next(par->cur));
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
  token_t* tok = parser_current(par);

  if (tok->kind != kind)
    report_error_unexpected_token(tok->loc);

  return parser_next(par);
}

list_t* parser_parse_delimited_list(parser_t* par, token_kind_t delim, parse_func_t parse_func)
{
  list_t* list = list_init();

  for (;;)
  {
    list_push_back(list, parse_func(par));

    if (!parser_consume(par, delim))
      break;
  }

  return list;
}

list_t* parser_parse_terminated_list(parser_t* par, token_kind_t termin, parse_func_t parse_func)
{
  list_t* list = list_init();

  while (!parser_consume(par, termin))
    list_push_back(list, parse_func(par));

  return list;
}

ast_node_t* parser_parse_id(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_ID);
  node->tok = parser_expect(par, TOK_ID);
  return node;
}

ast_node_t* parser_parse_attr(parser_t* par)
{
  ast_attr_t* node = (ast_attr_t*)ast_node_init(AST_ATTR);
  node->tok = parser_current(par);

  node->id = parser_parse_id(par);

  if (parser_consume(par, TOK_PUNCT_PAREN_LEFT))
  {
    node->params = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_expr);

    parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  }

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_mut(parser_t* par)
{
  ast_type_mut_t* node = (ast_type_mut_t*)ast_node_init(AST_TYPE_MUT);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_MUT);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_const(parser_t* par)
{
  ast_type_const_t* node = (ast_type_const_t*)ast_node_init(AST_TYPE_CONST);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_CONST);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_ptr(parser_t* par)
{
  ast_type_ptr_t* node = (ast_type_ptr_t*)ast_node_init(AST_TYPE_PTR);
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_ASTERISK);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_array(parser_t* par)
{
  ast_type_array_t* node = (ast_type_array_t*)ast_node_init(AST_TYPE_ARRAY);
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_BRACKET_LEFT);
  
  node->size = parser_current(par)->kind == TOK_PUNCT_BRACKET_RIGHT ? NULL : parser_parse_expr(par);
  
  parser_expect(par, TOK_PUNCT_BRACKET_RIGHT);

  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_ref(parser_t* par)
{
  ast_type_ref_t* node = (ast_type_ref_t*)ast_node_init(AST_TYPE_REF);
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_AMPERSAND);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_optional(parser_t* par)
{
  ast_type_opt_t* node = (ast_type_opt_t*)ast_node_init(AST_TYPE_OPT);
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_QUESTION);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_fun(parser_t* par)
{
  ast_type_fun_t* node = (ast_type_fun_t*)ast_node_init(AST_TYPE_FUN);
  node->tok = parser_current(par);
  
  node->abi = ABI_TAU;

  if (parser_consume(par, TOK_KW_EXTERN))
    node->abi = parser_parse_abi(par);

  parser_expect(par, TOK_KW_FUN);
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);
  
  node->params = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_type);

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  parser_expect(par, TOK_PUNCT_COLON);

  node->return_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_gen(parser_t* par)
{
  ast_type_gen_t* node = (ast_type_gen_t*)ast_node_init(AST_TYPE_GEN);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_FUN);
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);
  
  node->params = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_type);

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  parser_expect(par, TOK_PUNCT_COLON);

  node->yield_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_member(parser_t* par)
{
  ast_id_t* node = (ast_id_t*)ast_node_init(AST_ID);
  node->tok = parser_next(par);

  if (parser_consume(par, TOK_PUNCT_DOT))
  {
    ast_type_member_t* type = (ast_type_member_t*)ast_node_init(AST_TYPE_MEMBER);
    type->tok = parser_current(par);

    type->owner = (ast_node_t*)node;
    type->member = parser_parse_type_member(par);

    return (ast_node_t*)type;
  }

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type(parser_t* par)
{
  ast_node_t* node = NULL;

  switch (parser_current(par)->kind)
  {
  case TOK_ID:                 return parser_parse_type_member  (par);                      
  case TOK_KW_MUT:             return parser_parse_type_mut     (par);
  case TOK_KW_CONST:           return parser_parse_type_const   (par);
  case TOK_PUNCT_ASTERISK:     return parser_parse_type_ptr     (par);
  case TOK_PUNCT_BRACKET_LEFT: return parser_parse_type_array   (par);
  case TOK_PUNCT_AMPERSAND:    return parser_parse_type_ref     (par);
  case TOK_PUNCT_QUESTION:     return parser_parse_type_optional(par);
  case TOK_KW_FUN:             return parser_parse_type_fun     (par);
  case TOK_KW_GEN:             return parser_parse_type_gen     (par);
  case TOK_KW_SELF:
    node = ast_node_init(AST_TYPE_SELF);
    node->tok = parser_expect(par, AST_TYPE_SELF);
    break;
  case TOK_KW_I8:
    node = ast_node_init(AST_TYPE_I8);
    node->tok = parser_expect(par, TOK_KW_I8);
    break;
  case TOK_KW_I16:
    node = ast_node_init(AST_TYPE_I16);
    node->tok = parser_expect(par, TOK_KW_I16);
    break;
  case TOK_KW_I32:
    node = ast_node_init(AST_TYPE_I32);
    node->tok = parser_expect(par, TOK_KW_I32);
    break;
  case TOK_KW_I64:
    node = ast_node_init(AST_TYPE_I64);
    node->tok = parser_expect(par, TOK_KW_I64);
    break;
  case TOK_KW_ISIZE:
    node = ast_node_init(AST_TYPE_ISIZE);
    node->tok = parser_expect(par, TOK_KW_ISIZE);
    break;
  case TOK_KW_U8:
    node = ast_node_init(AST_TYPE_U8);
    node->tok = parser_expect(par, TOK_KW_U8);
    break;
  case TOK_KW_U16:
    node = ast_node_init(AST_TYPE_U16);
    node->tok = parser_expect(par, TOK_KW_U16);
    break;
  case TOK_KW_U32:
    node = ast_node_init(AST_TYPE_U32);
    node->tok = parser_expect(par, TOK_KW_U32);
    break;
  case TOK_KW_U64:
    node = ast_node_init(AST_TYPE_U64);
    node->tok = parser_expect(par, TOK_KW_U64);
    break;
  case TOK_KW_USIZE:
    node = ast_node_init(AST_TYPE_USIZE);
    node->tok = parser_expect(par, TOK_KW_USIZE);
    break;
  case TOK_KW_F32:
    node = ast_node_init(AST_TYPE_F32);
    node->tok = parser_expect(par, TOK_KW_F32);
    break;
  case TOK_KW_F64:
    node = ast_node_init(AST_TYPE_F64);
    node->tok = parser_expect(par, TOK_KW_F64);
    break;
  case TOK_KW_BOOL:
    node = ast_node_init(AST_TYPE_BOOL);
    node->tok = parser_expect(par, TOK_KW_BOOL);
    break;
  case TOK_KW_UNIT:
    node = ast_node_init(AST_TYPE_UNIT);
    node->tok = parser_expect(par, TOK_KW_UNIT);
    break;
  default: report_error_unexpected_token(parser_current(par)->loc);
  }

  return node;
}

ast_node_t* parser_parse_expr(parser_t* par)
{
  return shyd_ast(par);
}

ast_node_t* parser_parse_stmt_if(parser_t* par)
{
  ast_stmt_if_t* node = (ast_stmt_if_t*)ast_node_init(AST_STMT_IF);
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
  ast_stmt_for_t* node = (ast_stmt_for_t*)ast_node_init(AST_STMT_FOR);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_FOR);
  
  node->var = parser_parse_decl_var(par);
  
  parser_expect(par, TOK_KW_IN);

  node->range = parser_parse_expr(par);
  
  parser_expect(par, TOK_KW_DO);

  node->stmt = parser_parse_stmt(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_while(parser_t* par)
{
  ast_stmt_while_t* node = (ast_stmt_while_t*)ast_node_init(AST_STMT_WHILE);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_WHILE);
  
  node->cond = parser_parse_expr(par);
  
  parser_expect(par, TOK_KW_DO);
  
  node->stmt = parser_parse_stmt(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_break(parser_t* par)
{
  ast_stmt_break_t* node = (ast_stmt_break_t*)ast_node_init(AST_STMT_BREAK);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_BREAK);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_continue(parser_t* par)
{
  ast_stmt_continue_t* node = (ast_stmt_continue_t*)ast_node_init(AST_STMT_CONTINUE);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_CONTINUE);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_return(parser_t* par)
{
  ast_stmt_return_t* node = (ast_stmt_return_t*)ast_node_init(AST_STMT_RETURN);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_RETURN);
  
  node->expr = parser_parse_expr(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_yield(parser_t* par)
{
  ast_stmt_yield_t* node = (ast_stmt_yield_t*)ast_node_init(AST_STMT_YIELD);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_YIELD);
  
  node->expr = parser_parse_expr(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_block(parser_t* par)
{
  ast_stmt_block_t* node = (ast_stmt_block_t*)ast_node_init(AST_STMT_BLOCK);
  node->tok = parser_current(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  node->stmts = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_stmt);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_expr(parser_t* par)
{
  ast_stmt_expr_t* node = (ast_stmt_expr_t*)ast_node_init(AST_STMT_EXPR);
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
  case TOK_KW_BREAK:         return parser_parse_stmt_break(par);
  case TOK_KW_CONTINUE:      return parser_parse_stmt_continue(par);
  case TOK_KW_RETURN:        return parser_parse_stmt_return(par);
  case TOK_KW_YIELD:         return parser_parse_stmt_yield(par);
  case TOK_PUNCT_BRACE_LEFT: return parser_parse_stmt_block(par);
  default: noop();
  }

  return parser_parse_stmt_expr(par);
}

ast_node_t* parser_parse_decl_var(parser_t* par)
{
  ast_decl_var_t* node = (ast_decl_var_t*)ast_node_init(AST_DECL_VAR);
  node->tok = parser_current(par);

  node->id = parser_parse_id(par);
  
  parser_expect(par, TOK_PUNCT_COLON);
  
  node->type = parser_parse_type(par);
  node->expr = parser_consume(par, TOK_PUNCT_EQUAL) ? parser_parse_expr(par) : NULL;
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_fun(parser_t* par, bool is_extern, abi_kind_t abi)
{
  ast_decl_fun_t* node = (ast_decl_fun_t*)ast_node_init(AST_DECL_FUN);
  node->tok = parser_current(par);
  
  node->is_extern = is_extern;
  node->abi = abi;

  parser_expect(par, TOK_KW_FUN);

  node->id = parser_parse_id(par);
  
  // Parse parameters.
  node->params = list_init();

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
          assert(variadic_param->expr == NULL);

          variadic_param->is_variadic = true;

          // Default parameters must be followed only by default parameters.
          if (seen_default)
            report_error_missing_default_parameter(
              variadic_param->tok->loc,
              ((ast_node_t*)list_back(node->params))->tok->loc
            );

          list_push_back(node->params, variadic_param);
        }
        // C-style variadic parameter.
        else
        {
          // Function must be extern "cdecl".
          assert(node->is_extern && node->abi == ABI_CDECL);
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
        report_error_missing_default_parameter(
          param->tok->loc,
          ((ast_node_t*)list_back(node->params))->tok->loc
        );

      list_push_back(node->params, param);

      if (!parser_consume(par, TOK_PUNCT_COMMA))
        break;
    }

    parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  }
  
  parser_expect(par, TOK_PUNCT_COLON);

  node->return_type = parser_parse_type(par);

  if (!is_extern)
    node->stmt = parser_parse_stmt(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_gen(parser_t* par)
{
  ast_decl_gen_t* node = (ast_decl_gen_t*)ast_node_init(AST_DECL_GEN);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_GEN);

  node->id = parser_parse_id(par);
  
  // Parse parameters.
  node->params = list_init();

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
        ast_decl_param_t* variadic_param = (ast_decl_param_t*)parser_parse_decl_param(par);
        assert(variadic_param->expr == NULL);

        // Default parameters must be followed only by default parameters.
        if (seen_default)
          report_error_missing_default_parameter(
            variadic_param->tok->loc,
            ((ast_node_t*)list_back(node->params))->tok->loc
          );

        list_push_back(node->params, variadic_param);
        break;
      }

      // Parse non-variadic parameter.
      ast_decl_param_t* param = (ast_decl_param_t*)parser_parse_decl_param(par);

      // Set flag variable if parameter is a default parameter.
      seen_default = seen_default || param->expr != NULL;

      // Default parameters must be followed only by default parameters.
      if (seen_default && param->expr != NULL)
        report_error_missing_default_parameter(
          param->tok->loc,
          ((ast_node_t*)list_back(node->params))->tok->loc
        );

      list_push_back(node->params, param);

      if (!parser_consume(par, TOK_PUNCT_COMMA))
        break;
    }

    parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  }
  
  parser_expect(par, TOK_PUNCT_COLON);

  node->yield_type = parser_parse_type(par);
  node->stmt = parser_parse_stmt(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_struct(parser_t* par)
{
  ast_decl_struct_t* node = (ast_decl_struct_t*)ast_node_init(AST_DECL_STRUCT);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_STRUCT);
  
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  // TODO: Struct member variables cannot have initializers
  node->members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_var);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_union(parser_t* par)
{
  ast_decl_union_t* node = (ast_decl_union_t*)ast_node_init(AST_DECL_UNION);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_UNION);
  
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  // TODO: Union member variables cannot have initializers
  node->members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_var);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_enum(parser_t* par)
{
  ast_decl_enum_t* node = (ast_decl_enum_t*)ast_node_init(AST_DECL_ENUM);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_ENUM);
  
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  node->members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_enum_constant);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_mod(parser_t* par)
{
  ast_decl_mod_t* node = (ast_decl_mod_t*)ast_node_init(AST_DECL_MOD);
  node->tok = parser_current(par);

  parser_expect(par, TOK_KW_MOD);
  
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  node->decls = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_with_attrs(parser_t* par)
{
  parser_expect(par, TOK_PUNCT_HASH);
  parser_expect(par, TOK_PUNCT_BRACKET_LEFT);

  list_t* attrs = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_attr);

  parser_expect(par, TOK_PUNCT_BRACKET_RIGHT);

  ast_decl_t* node = (ast_decl_t*)parser_parse_decl(par);
  
  if (node->attrs != NULL)
  {
    while (!list_empty(attrs))
      list_push_back(node->attrs, list_pop_front(attrs));

    list_free(attrs);
  }
  else
    node->attrs = attrs;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_extern(parser_t* par)
{
  parser_expect(par, TOK_KW_EXTERN);
  
  abi_kind_t abi = ABI_CDECL;

  if (parser_current(par)->kind == TOK_LIT_STR)
    abi = parser_parse_abi(par);

  ast_decl_fun_t* node = NULL;

  switch (parser_current(par)->kind)
  {
  case TOK_KW_FUN: node = (ast_decl_fun_t*)parser_parse_decl_fun(par, true, abi); break;
  default:         report_error_unexpected_token(parser_current(par)->loc);
  }

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl(parser_t* par)
{
  switch (parser_current(par)->kind)
  {
  case TOK_ID:         return parser_parse_decl_var       (par);
  case TOK_KW_EXTERN:  return parser_parse_decl_extern    (par);
  case TOK_KW_FUN:     return parser_parse_decl_fun       (par, false, ABI_TAU);
  case TOK_KW_GEN:     return parser_parse_decl_gen       (par);
  case TOK_KW_STRUCT:  return parser_parse_decl_struct    (par);
  case TOK_KW_UNION:   return parser_parse_decl_union     (par);
  case TOK_KW_ENUM:    return parser_parse_decl_enum      (par);
  case TOK_KW_MOD:     return parser_parse_decl_mod       (par);
  case TOK_PUNCT_HASH: return parser_parse_decl_with_attrs(par);
  default:             report_error_unexpected_token(parser_current(par)->loc);
  }

  return NULL;
}

ast_node_t* parser_parse_decl_param(parser_t* par)
{
  ast_decl_param_t* node = (ast_decl_param_t*)ast_node_init(AST_DECL_PARAM);
  node->tok = parser_current(par);

  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type = parser_parse_type(par);

  if (parser_consume(par, TOK_PUNCT_EQUAL))
    node->expr = parser_parse_expr(par);

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_enum_constant(parser_t* par)
{
  ast_decl_enum_constant_t* node = (ast_decl_enum_constant_t*)ast_node_init(AST_DECL_ENUM_CONSTANT);
  node->tok = parser_current(par);

  node->id = parser_parse_id(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse(parser_t* par, list_t* toks)
{
  par->toks = toks;
  par->cur = list_front_node(toks);

  ast_node_t* root = ast_node_init(AST_PROG);
  root->tok = parser_current(par);

  ((ast_prog_t*)root)->decls = parser_parse_terminated_list(par, TOK_EOF, parser_parse_decl);

  return root;
}
