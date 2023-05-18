#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "log.h"
#include "crumb.h"

#include "list.h"

#include "token.h"
#include "ast.h"
#include "shyd.h"

#include "diagnostics.h"

#include "memtrace.h"

parser_t* parser_init(list_t* toks)
{
  parser_t* par = (parser_t*)malloc(sizeof(parser_t));
  assert(par != NULL);
  par->root = NULL;
  par->toks = toks;
  par->cur = list_front_node(toks);
  return par;
}

void parser_free(parser_t* par)
{
  ast_node_free(par->root);
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

list_t* parser_parse_generic_param_list(parser_t* par)
{
  parser_expect(par, TOK_PUNCT_LESS);

  list_t* params = NULL;

  if (!parser_consume(par, TOK_PUNCT_GREATER))
  {
    params = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_generic_param);
    parser_expect(par, TOK_PUNCT_GREATER);
  }

  return params;
}

ast_node_t* parser_parse_id(parser_t* par)
{
  ast_id_t* id = ast_id_init(parser_current(par));
  parser_expect(par, TOK_ID);
  return (ast_node_t*)id;
}

ast_node_t* parser_parse_type_mut(parser_t* par)
{
  ast_type_mut_t* node = ast_type_mut_init(parser_current(par));

  parser_expect(par, TOK_KW_MUT);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_const(parser_t* par)
{
  ast_type_const_t* node = ast_type_const_init(parser_current(par));

  parser_expect(par, TOK_KW_CONST);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_ptr(parser_t* par)
{
  ast_type_ptr_t* node = ast_type_ptr_init(parser_current(par));

  parser_expect(par, TOK_PUNCT_ASTERISK);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_array(parser_t* par)
{
  ast_type_array_t* node = ast_type_array_init(parser_current(par));

  parser_expect(par, TOK_PUNCT_BRACKET_LEFT);
  
  node->size = parser_current(par)->kind == TOK_PUNCT_BRACKET_RIGHT ? NULL : parser_parse_expr(par);
  
  parser_expect(par, TOK_PUNCT_BRACKET_RIGHT);

  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_ref(parser_t* par)
{
  ast_type_ref_t* node = ast_type_ref_init(parser_current(par));

  parser_expect(par, TOK_PUNCT_AMPERSAND);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_optional(parser_t* par)
{
  ast_type_opt_t* node = ast_type_opt_init(parser_current(par));

  parser_expect(par, TOK_PUNCT_QUESTION);
  
  node->base_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_fun(parser_t* par)
{
  ast_type_fun_t* node = ast_type_fun_init(parser_current(par));

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
  ast_type_gen_t* node = ast_type_gen_init(parser_current(par));

  parser_expect(par, TOK_KW_GEN);
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);
  
  node->params = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_type);

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  parser_expect(par, TOK_PUNCT_COLON);
  
  node->yield_type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_type_member(parser_t* par)
{
  ast_id_t* owner = ast_id_init(parser_expect(par, TOK_ID));

  if (parser_current(par)->kind == TOK_PUNCT_DOT)
  {
    ast_type_member_t* type = ast_type_member_init(parser_expect(par, TOK_PUNCT_DOT));
    type->owner = (ast_node_t*)owner;
    type->member = parser_parse_type_member(par);
    return (ast_node_t*)type;
  }

  return (ast_node_t*)owner;
}

ast_node_t* parser_parse_type(parser_t* par)
{
  ast_node_t* node;

  switch (parser_current(par)->kind)
  {
  case TOK_KW_MUT:             return parser_parse_type_mut(par);
  case TOK_KW_CONST:           return parser_parse_type_const(par);
  case TOK_PUNCT_ASTERISK:     return parser_parse_type_ptr(par);
  case TOK_PUNCT_BRACKET_LEFT: return parser_parse_type_array(par);
  case TOK_PUNCT_AMPERSAND:    return parser_parse_type_ref(par);
  case TOK_PUNCT_QUESTION:     return parser_parse_type_optional(par);
  case TOK_KW_FUN:             return parser_parse_type_fun(par);
  case TOK_KW_GEN:             return parser_parse_type_gen(par);
  case TOK_KW_SELF:            node = (ast_node_t*)ast_type_self_init( parser_expect(par, TOK_KW_SELF )); return node;
  case TOK_KW_I8:              node = (ast_node_t*)ast_type_i8_init(   parser_expect(par, TOK_KW_I8   )); return node;
  case TOK_KW_I16:             node = (ast_node_t*)ast_type_i16_init(  parser_expect(par, TOK_KW_I16  )); return node;
  case TOK_KW_I32:             node = (ast_node_t*)ast_type_i32_init(  parser_expect(par, TOK_KW_I32  )); return node;
  case TOK_KW_I64:             node = (ast_node_t*)ast_type_i64_init(  parser_expect(par, TOK_KW_I64  )); return node;
  case TOK_KW_ISIZE:           node = (ast_node_t*)ast_type_isize_init(parser_expect(par, TOK_KW_ISIZE)); return node;
  case TOK_KW_U8:              node = (ast_node_t*)ast_type_u8_init(   parser_expect(par, TOK_KW_U8   )); return node;
  case TOK_KW_U16:             node = (ast_node_t*)ast_type_u16_init(  parser_expect(par, TOK_KW_U16  )); return node;
  case TOK_KW_U32:             node = (ast_node_t*)ast_type_u32_init(  parser_expect(par, TOK_KW_U32  )); return node;
  case TOK_KW_U64:             node = (ast_node_t*)ast_type_u64_init(  parser_expect(par, TOK_KW_U64  )); return node;
  case TOK_KW_USIZE:           node = (ast_node_t*)ast_type_usize_init(parser_expect(par, TOK_KW_USIZE)); return node;  
  case TOK_KW_F32:             node = (ast_node_t*)ast_type_f32_init(  parser_expect(par, TOK_KW_F32  )); return node;
  case TOK_KW_F64:             node = (ast_node_t*)ast_type_f64_init(  parser_expect(par, TOK_KW_F64  )); return node;
  case TOK_KW_BOOL:            node = (ast_node_t*)ast_type_bool_init( parser_expect(par, TOK_KW_BOOL )); return node;
  case TOK_KW_UNIT:            node = (ast_node_t*)ast_type_unit_init( parser_expect(par, TOK_KW_UNIT )); return node;
  case TOK_ID:                 return parser_parse_type_member(par);                      
  default:                     report_error_unexpected_token(parser_current(par)->loc);
  }

  return NULL;
}

ast_node_t* parser_parse_expr(parser_t* par)
{
  return shyd_ast(par);
}

ast_node_t* parser_parse_stmt_if(parser_t* par)
{
  ast_stmt_if_t* node = ast_stmt_if_init(parser_current(par));

  parser_expect(par, TOK_KW_IF);
  
  node->cond = parser_parse_expr(par);

  parser_expect(par, TOK_KW_THEN);
  
  node->stmt = parser_parse_stmt(par);
  node->stmt_else = parser_consume(par, TOK_KW_ELSE) ? parser_parse_stmt(par) : NULL;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_for(parser_t* par)
{
  ast_stmt_for_t* node = ast_stmt_for_init(parser_current(par));

  parser_expect(par, TOK_KW_FOR);
  
  node->var = parser_parse_decl_loop_var(par);
  
  parser_expect(par, TOK_KW_IN);
  
  node->range = parser_parse_expr(par);
  
  parser_expect(par, TOK_KW_DO);
  
  node->stmt = parser_parse_stmt(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_while(parser_t* par)
{
  ast_stmt_while_t* node = ast_stmt_while_init(parser_current(par));

  parser_expect(par, TOK_KW_WHILE);
  
  node->cond = parser_parse_expr(par);
  
  parser_expect(par, TOK_KW_DO);
  
  node->stmt = parser_parse_stmt(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_break(parser_t* par)
{
  ast_stmt_break_t* node = ast_stmt_break_init(parser_current(par));
  parser_expect(par, TOK_KW_BREAK);
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_continue(parser_t* par)
{
  ast_stmt_continue_t* node = ast_stmt_continue_init(parser_current(par));
  parser_expect(par, TOK_KW_CONTINUE);
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_return(parser_t* par)
{
  ast_stmt_return_t* node = ast_stmt_return_init(parser_current(par));

  parser_expect(par, TOK_KW_RETURN);
  
  node->expr = parser_parse_expr(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_yield(parser_t* par)
{
  ast_stmt_yield_t* node = ast_stmt_yield_init(parser_current(par));

  parser_expect(par, TOK_KW_YIELD);
  
  node->expr = parser_parse_expr(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_block(parser_t* par)
{
  ast_stmt_block_t* node = ast_stmt_block_init(parser_current(par));

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  node->stmts = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_stmt);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_stmt_expr(parser_t* par)
{
  ast_stmt_expr_t* node = ast_stmt_expr_init(parser_current(par));

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
  default: fallthrough();
  }

  return parser_parse_stmt_expr(par);
}

ast_node_t* parser_parse_decl_var(parser_t* par)
{
  ast_decl_var_t* node = ast_decl_var_init(parser_current(par));

  node->id = parser_parse_id(par);
  
  parser_expect(par, TOK_PUNCT_COLON);
  
  node->type = parser_parse_type(par);
  node->init = parser_consume(par, TOK_PUNCT_EQUAL) ? parser_parse_expr(par) : NULL;
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_loop_var(parser_t* par)
{
  ast_decl_loop_var_t* node = ast_decl_loop_var_init(parser_current(par));
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);
  
  node->type = parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_fun(parser_t* par)
{
  ast_decl_generic_t* generic_node = NULL;
  ast_decl_fun_t* node = ast_decl_fun_init(parser_current(par));
  
  parser_expect(par, TOK_KW_FUN);
  
  // Parse generic parameters.
  if (parser_current(par)->kind == TOK_PUNCT_LESS)
  {
    generic_node = ast_decl_generic_init(node->tok);
    generic_node->decl = (ast_node_t*)node;
    generic_node->params = parser_parse_generic_param_list(par);
  }

  node->id = parser_parse_id(par);
  
  // Parse parameters.
  node->params = NULL;

  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  if (!parser_consume(par, TOK_PUNCT_PAREN_RIGHT))
  {
    node->params = list_init();

    for (bool seen_default = false;;)
    {
      // Parse variadic parameter.
      // If a variadic parameter is present it must be the last one in the
      // parameter list.
      if (parser_consume(par, TOK_PUNCT_DOT_DOT_DOT))
      {
        ast_node_t* variadic_param = parser_parse_variadic_param(par);

        // Default parameters must be followed only by default parameters.
        if (seen_default)
          report_error_missing_default_parameter(variadic_param->tok->loc, ((ast_param_t*)list_back(node->params))->tok->loc);

        list_push_back(node->params, variadic_param);
        break;
      }

      // Parse non-variadic parameter.
      ast_node_t* param = parser_parse_param(par);

      // Set flag variable if parameter is a default parameter.
      seen_default |= param->kind == AST_PARAM_DEFAULT;

      // Default parameters must be followed only by default parameters.
      if (seen_default && param->kind != AST_PARAM_DEFAULT)
        report_error_missing_default_parameter(param->tok->loc, ((ast_param_t*)list_back(node->params))->tok->loc);

      list_push_back(node->params, param);

      if (!parser_consume(par, TOK_PUNCT_COMMA))
        break;
    }

    parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  }
  
  parser_expect(par, TOK_PUNCT_COLON);

  node->return_type = parser_parse_type(par);
  node->stmt = parser_parse_stmt(par);
  
  return generic_node == NULL ? (ast_node_t*)node : (ast_node_t*)generic_node;
}

ast_node_t* parser_parse_decl_gen(parser_t* par)
{
  ast_decl_generic_t* generic_node = NULL;
  ast_decl_gen_t* node = ast_decl_gen_init(parser_current(par));
  
  parser_expect(par, TOK_KW_FUN);
  
  // Parse generic parameters.
  if (parser_current(par)->kind == TOK_PUNCT_LESS)
  {
    generic_node = ast_decl_generic_init(node->tok);
    generic_node->decl = (ast_node_t*)node;
    generic_node->params = parser_parse_generic_param_list(par);
  }

  node->id = parser_parse_id(par);
  
  // Parse parameters.
  node->params = NULL;

  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  if (!parser_consume(par, TOK_PUNCT_PAREN_RIGHT))
  {
    node->params = list_init();

    for (bool seen_default = false;;)
    {
      // Parse variadic parameter.
      // If a variadic parameter is present it must be the last one in the
      // parameter list.
      if (parser_consume(par, TOK_PUNCT_DOT_DOT_DOT))
      {
        ast_node_t* variadic_param = parser_parse_variadic_param(par);

        // Default parameters must be followed only by default parameters.
        if (seen_default)
          report_error_missing_default_parameter(variadic_param->tok->loc, ((ast_param_t*)list_back(node->params))->tok->loc);

        list_push_back(node->params, variadic_param);
        break;
      }

      // Parse non-variadic parameter.
      ast_node_t* param = parser_parse_param(par);

      // Set flag variable if parameter is a default parameter.
      seen_default |= param->kind == AST_PARAM_DEFAULT;

      // Default parameters must be followed only by default parameters.
      if (seen_default && param->kind != AST_PARAM_DEFAULT)
        report_error_missing_default_parameter(param->tok->loc, ((ast_param_t*)list_back(node->params))->tok->loc);

      list_push_back(node->params, param);

      if (!parser_consume(par, TOK_PUNCT_COMMA))
        break;
    }

    parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  }
  
  parser_expect(par, TOK_PUNCT_COLON);

  node->yield_type = parser_parse_type(par);
  node->stmt = parser_parse_stmt(par);
  
  return generic_node == NULL ? (ast_node_t*)node : (ast_node_t*)generic_node;
}

ast_node_t* parser_parse_decl_struct(parser_t* par)
{
  ast_decl_generic_t* generic_node = NULL;
  ast_decl_struct_t* node = ast_decl_struct_init(parser_current(par));

  parser_expect(par, TOK_KW_STRUCT);
  
  // Parse generic parameters.
  if (parser_current(par)->kind == TOK_PUNCT_LESS)
  {
    generic_node = ast_decl_generic_init(node->tok);
    generic_node->decl = (ast_node_t*)node;
    generic_node->params = parser_parse_generic_param_list(par);
  }
  
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  // TODO: Struct member variables cannot have initializers
  node->members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_var);
  
  return generic_node == NULL ? (ast_node_t*)node : (ast_node_t*)generic_node;
}

ast_node_t* parser_parse_decl_union(parser_t* par)
{
  ast_decl_union_t* node = ast_decl_union_init(parser_current(par));

  parser_expect(par, TOK_KW_UNION);
  
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  // TODO: Union member variables cannot have initializers
  node->members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_var);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_enum(parser_t* par)
{
  ast_decl_enum_t* node = ast_decl_enum_init(parser_current(par));

  parser_expect(par, TOK_KW_ENUM);
  
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  node->values = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_enumerator);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl_mod(parser_t* par)
{
  ast_decl_mod_t* node = ast_decl_mod_init(parser_current(par));

  parser_expect(par, TOK_KW_MOD);
  
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  node->decls = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_decl(parser_t* par)
{
  switch (parser_current(par)->kind)
  {
  case TOK_ID:        return parser_parse_decl_var(par);
  case TOK_KW_FUN:    return parser_parse_decl_fun(par);
  case TOK_KW_GEN:    return parser_parse_decl_gen(par);
  case TOK_KW_STRUCT: return parser_parse_decl_struct(par);
  case TOK_KW_UNION:  return parser_parse_decl_union(par);
  case TOK_KW_ENUM:   return parser_parse_decl_enum(par);
  case TOK_KW_MOD:    return parser_parse_decl_mod(par);
  default:            report_error_unexpected_token(parser_current(par)->loc);
  }

  return NULL;
}

ast_node_t* parser_parse_param(parser_t* par)
{
  token_t* tok = parser_current(par);
  ast_node_t* id = parser_parse_id(par);
  parser_expect(par, TOK_PUNCT_COLON);
  ast_node_t* type = parser_parse_type(par);

  if (parser_consume(par, TOK_PUNCT_EQUAL))
  {
    ast_node_t* init = parser_parse_expr(par);

    ast_param_default_t* node = ast_param_default_init(tok);
    node->id = id;
    node->type = type;
    node->init = init;

    return (ast_node_t*)node;
  }

  ast_param_t* node = ast_param_init(tok);
  node->id = id;
  node->type = type;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_variadic_param(parser_t* par)
{
  ast_param_variadic_t* node = ast_param_variadic_init(parser_current(par));
  node->id = parser_parse_id(par);
  
  parser_expect(par, TOK_PUNCT_COLON);
  
  ast_type_array_t* type = ast_type_array_init(parser_current(par));
  type->base_type = parser_parse_type(par);
  type->size = NULL;
  node->type = (ast_node_t*)type;

  return (ast_node_t*)node;
}

ast_node_t* parser_parse_generic_param(parser_t* par)
{
  ast_param_generic_t* node = ast_param_generic_init(parser_current(par));
  node->id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type = parser_consume(par, TOK_KW_TYPE) ? (ast_node_t*)ast_type_type_init(parser_current(par)) : parser_parse_type(par);
  
  return (ast_node_t*)node;
}

ast_node_t* parser_parse_enumerator(parser_t* par)
{
  ast_enumerator_t* node = ast_enumerator_init(parser_current(par));
  node->id = parser_parse_id(par);
  return (ast_node_t*)node;
}

void parser_parse(parser_t* par)
{
  ast_prog_t* node = ast_prog_init(parser_current(par));

  node->decls = parser_parse_terminated_list(par, TOK_EOF, parser_parse_decl);

  par->root = (ast_node_t*)node;
}
