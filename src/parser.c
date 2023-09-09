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

struct parser_s
{
  list_t* toks; // List of tokens to be processed.
  list_node_t* cur; // Current token in list.
};

parser_t* parser_init(void)
{
  parser_t* par = (parser_t*)malloc(sizeof(parser_t));
  assert(par != NULL);

  par->toks = NULL;
  par->cur = NULL;

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

  if (token_get_kind(tok) != TOK_EOF)
    par->cur = list_node_next(par->cur);

  return tok;
}

token_t* parser_peek(parser_t* par)
{
  token_t* tok = parser_current(par);
  assert(tok != NULL);

  if (token_get_kind(tok) == TOK_EOF)
    return tok;

  return (token_t*)list_node_get(list_node_next(par->cur));
}

bool parser_consume(parser_t* par, token_kind_t kind)
{
  if (token_get_kind(parser_current(par)) == kind)
  {
    parser_next(par);
    return true;
  }

  return false;
}

token_t* parser_expect(parser_t* par, token_kind_t kind)
{
  token_t* tok = parser_current(par);

  if (token_get_kind(tok) != kind)
    report_error_unexpected_token(token_get_loc(tok));

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
  ast_set_token(node, parser_expect(par, TOK_ID));
  return node;
}

ast_node_t* parser_parse_type_mut(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_TYPE_MUT);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_MUT);
  
  ast_set_base_type(node, parser_parse_type(par));
  
  return node;
}

ast_node_t* parser_parse_type_const(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_TYPE_CONST);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_CONST);
  
  ast_set_base_type(node, parser_parse_type(par));
  
  return node;
}

ast_node_t* parser_parse_type_ptr(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_TYPE_PTR);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_PUNCT_ASTERISK);
  
  ast_set_base_type(node, parser_parse_type(par));
  
  return node;
}

ast_node_t* parser_parse_type_array(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_TYPE_ARRAY);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_PUNCT_BRACKET_LEFT);
  
  ast_set_size(node, token_get_kind(parser_current(par)) == TOK_PUNCT_BRACKET_RIGHT ? NULL : parser_parse_expr(par));
  
  parser_expect(par, TOK_PUNCT_BRACKET_RIGHT);

  ast_set_base_type(node, parser_parse_type(par));
  
  return node;
}

ast_node_t* parser_parse_type_ref(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_TYPE_REF);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_PUNCT_AMPERSAND);
  
  ast_set_base_type(node, parser_parse_type(par));
  
  return node;
}

ast_node_t* parser_parse_type_optional(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_TYPE_OPT);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_PUNCT_QUESTION);
  
  ast_set_base_type(node, parser_parse_type(par));
  
  return node;
}

ast_node_t* parser_parse_type_fun(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_TYPE_FUN);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_FUN);
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);
  
  ast_set_params(node, parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_type));

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  parser_expect(par, TOK_PUNCT_COLON);

  ast_set_return_type(node, parser_parse_type(par));
  
  return node;
}

ast_node_t* parser_parse_type_gen(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_TYPE_GEN);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_FUN);
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);
  
  ast_set_params(node, parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_type));

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  parser_expect(par, TOK_PUNCT_COLON);

  ast_set_yield_type(node, parser_parse_type(par));
  
  return node;
}

ast_node_t* parser_parse_type_member(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_ID);
  ast_set_token(node, parser_next(par));

  if (parser_consume(par, TOK_PUNCT_DOT))
  {
    ast_node_t* type = ast_node_init(AST_TYPE_MEMBER);
    ast_set_token(type, parser_current(par));

    ast_set_owner(type, node);
    ast_set_member(type, parser_parse_type_member(par));

    return type;
  }

  return node;
}

ast_node_t* parser_parse_type(parser_t* par)
{
  ast_node_t* node = NULL;

  switch (token_get_kind(parser_current(par)))
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
    ast_set_token(node, parser_current(par));
    break;
  case TOK_KW_I8:
    node = ast_node_init(AST_TYPE_I8);
    ast_set_token(node, parser_expect(par, TOK_KW_I8));
    break;
  case TOK_KW_I16:
    node = ast_node_init(AST_TYPE_I16);
    ast_set_token(node, parser_expect(par, TOK_KW_I16));
    break;
  case TOK_KW_I32:
    node = ast_node_init(AST_TYPE_I32);
    ast_set_token(node, parser_expect(par, TOK_KW_I32));
    break;
  case TOK_KW_I64:
    node = ast_node_init(AST_TYPE_I64);
    ast_set_token(node, parser_expect(par, TOK_KW_I64));
    break;
  case TOK_KW_ISIZE:
    node = ast_node_init(AST_TYPE_ISIZE);
    ast_set_token(node, parser_expect(par, TOK_KW_ISIZE));
    break;
  case TOK_KW_U8:
    node = ast_node_init(AST_TYPE_U8);
    ast_set_token(node, parser_expect(par, TOK_KW_U8));
    break;
  case TOK_KW_U16:
    node = ast_node_init(AST_TYPE_U16);
    ast_set_token(node, parser_expect(par, TOK_KW_U16));
    break;
  case TOK_KW_U32:
    node = ast_node_init(AST_TYPE_U32);
    ast_set_token(node, parser_expect(par, TOK_KW_U32));
    break;
  case TOK_KW_U64:
    node = ast_node_init(AST_TYPE_U64);
    ast_set_token(node, parser_expect(par, TOK_KW_U64));
    break;
  case TOK_KW_USIZE:
    node = ast_node_init(AST_TYPE_USIZE);
    ast_set_token(node, parser_expect(par, TOK_KW_USIZE));
    break;
  case TOK_KW_F32:
    node = ast_node_init(AST_TYPE_F32);
    ast_set_token(node, parser_expect(par, TOK_KW_F32));
    break;
  case TOK_KW_F64:
    node = ast_node_init(AST_TYPE_F64);
    ast_set_token(node, parser_expect(par, TOK_KW_F64));
    break;
  case TOK_KW_BOOL:
    node = ast_node_init(AST_TYPE_BOOL);
    ast_set_token(node, parser_expect(par, TOK_KW_BOOL));
    break;
  case TOK_KW_UNIT:
    node = ast_node_init(AST_TYPE_UNIT);
    ast_set_token(node, parser_expect(par, TOK_KW_UNIT));
    break;
  default: report_error_unexpected_token(token_get_loc(parser_current(par)));
  }

  return node;
}

ast_node_t* parser_parse_expr(parser_t* par)
{
  return shyd_ast(par);
}

ast_node_t* parser_parse_stmt_if(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_IF);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_IF);
  
  ast_set_cond(node, parser_parse_expr(par));

  parser_expect(par, TOK_KW_THEN);

  ast_set_stmt(node, parser_parse_stmt(par));
  ast_set_stmt_else(node, parser_consume(par, TOK_KW_ELSE) ? parser_parse_stmt(par) : NULL);

  return node;
}

ast_node_t* parser_parse_stmt_for(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_FOR);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_FOR);
  
  ast_set_var(node, parser_parse_decl_var(par));
  
  parser_expect(par, TOK_KW_IN);

  ast_set_range(node, parser_parse_expr(par));
  
  parser_expect(par, TOK_KW_DO);

  ast_set_stmt(node, parser_parse_stmt(par));
  
  return node;
}

ast_node_t* parser_parse_stmt_while(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_WHILE);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_WHILE);
  
  ast_set_cond(node, parser_parse_expr(par));
  
  parser_expect(par, TOK_KW_DO);
  
  ast_set_stmt(node, parser_parse_stmt(par));
  
  return node;
}

ast_node_t* parser_parse_stmt_break(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_BREAK);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_BREAK);

  return node;
}

ast_node_t* parser_parse_stmt_continue(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_CONTINUE);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_CONTINUE);

  return node;
}

ast_node_t* parser_parse_stmt_return(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_RETURN);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_RETURN);
  
  ast_set_expr(node, parser_parse_expr(par));
  
  return node;
}

ast_node_t* parser_parse_stmt_yield(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_YIELD);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_YIELD);
  
  ast_set_expr(node, parser_parse_expr(par));
  
  return node;
}

ast_node_t* parser_parse_stmt_block(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_BLOCK);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  ast_set_stmts(node, parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_stmt));
  
  return node;
}

ast_node_t* parser_parse_stmt_expr(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_STMT_EXPR);
  ast_set_token(node, parser_current(par));

  ast_set_expr(node, parser_parse_expr(par));
  
  return node;
}

ast_node_t* parser_parse_stmt(parser_t* par)
{
  switch (token_get_kind(parser_current(par)))
  {
  case TOK_ID:
    if (token_get_kind(parser_peek(par)) == TOK_PUNCT_COLON)
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
  ast_node_t* node = ast_node_init(AST_DECL_VAR);
  ast_set_token(node, parser_current(par));

  ast_set_id(node, parser_parse_id(par));
  
  parser_expect(par, TOK_PUNCT_COLON);
  
  ast_set_type(node, parser_parse_type(par));
  ast_set_expr(node, parser_consume(par, TOK_PUNCT_EQUAL) ? parser_parse_expr(par) : NULL);
  
  return node;
}

ast_node_t* parser_parse_decl_fun(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_DECL_FUN);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_FUN);

  ast_set_id(node, parser_parse_id(par));
  
  // Parse parameters.
  ast_set_params(node, NULL);

  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  if (!parser_consume(par, TOK_PUNCT_PAREN_RIGHT))
  {
    ast_set_params(node, list_init());

    for (bool seen_default = false;;)
    {
      // Parse variadic parameter.
      // If a variadic parameter is present it must be the last one in the
      // parameter list.
      if (parser_consume(par, TOK_PUNCT_DOT_DOT_DOT))
      {
        ast_node_t* variadic_param = parser_parse_decl_param(par);
        assert(ast_get_expr(variadic_param) == NULL);

        // Default parameters must be followed only by default parameters.
        if (seen_default)
          report_error_missing_default_parameter(
            token_get_loc(ast_get_token(variadic_param)),
            token_get_loc(ast_get_token((ast_node_t*)list_back(ast_get_params(node))))
          );

        list_push_back(ast_get_params(node), variadic_param);
        break;
      }

      // Parse non-variadic parameter.
      ast_node_t* param = parser_parse_decl_param(par);

      // Set flag variable if parameter is a default parameter.
      seen_default = seen_default || ast_get_expr(param) != NULL;

      // Default parameters must be followed only by default parameters.
      if (seen_default && ast_get_expr(param) != NULL)
        report_error_missing_default_parameter(
          token_get_loc(ast_get_token(param)),
          token_get_loc(ast_get_token((ast_node_t*)list_back(ast_get_params(node))))
        );

      list_push_back(ast_get_params(node), param);

      if (!parser_consume(par, TOK_PUNCT_COMMA))
        break;
    }

    parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  }
  
  parser_expect(par, TOK_PUNCT_COLON);

  ast_set_return_type(node, parser_parse_type(par));
  ast_set_stmt(node, parser_parse_stmt(par));
  
  return node;
}

ast_node_t* parser_parse_decl_gen(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_DECL_GEN);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_GEN);

  ast_set_id(node, parser_parse_id(par));
  
  // Parse parameters.
  ast_set_params(node, NULL);

  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  if (!parser_consume(par, TOK_PUNCT_PAREN_RIGHT))
  {
    ast_set_params(node, list_init());

    for (bool seen_default = false;;)
    {
      // Parse variadic parameter.
      // If a variadic parameter is present it must be the last one in the
      // parameter list.
      if (parser_consume(par, TOK_PUNCT_DOT_DOT_DOT))
      {
        ast_node_t* variadic_param = parser_parse_decl_param(par);
        assert(ast_get_expr(variadic_param) == NULL);

        // Default parameters must be followed only by default parameters.
        if (seen_default)
          report_error_missing_default_parameter(
            token_get_loc(ast_get_token(variadic_param)),
            token_get_loc(ast_get_token((ast_node_t*)list_back(ast_get_params(node))))
          );

        list_push_back(ast_get_params(node), variadic_param);
        break;
      }

      // Parse non-variadic parameter.
      ast_node_t* param = parser_parse_decl_param(par);

      // Set flag variable if parameter is a default parameter.
      seen_default = seen_default || ast_get_expr(param) != NULL;

      // Default parameters must be followed only by default parameters.
      if (seen_default && ast_get_expr(param) != NULL)
        report_error_missing_default_parameter(
          token_get_loc(ast_get_token(param)),
          token_get_loc(ast_get_token((ast_node_t*)list_back(ast_get_params(node))))
        );

      list_push_back(ast_get_params(node), param);

      if (!parser_consume(par, TOK_PUNCT_COMMA))
        break;
    }

    parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  }
  
  parser_expect(par, TOK_PUNCT_COLON);

  ast_set_yield_type(node, parser_parse_type(par));
  ast_set_stmt(node, parser_parse_stmt(par));
  
  return node;
}

ast_node_t* parser_parse_decl_struct(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_DECL_STRUCT);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_STRUCT);
  
  ast_set_id(node, parser_parse_id(par));

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  // TODO: Struct member variables cannot have initializers
  ast_set_members(node, parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_var));
  
  return node;
}

ast_node_t* parser_parse_decl_union(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_DECL_UNION);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_UNION);
  
  ast_set_id(node, parser_parse_id(par));

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  // TODO: Union member variables cannot have initializers
  ast_set_members(node, parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_var));
  
  return node;
}

ast_node_t* parser_parse_decl_enum(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_DECL_ENUM);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_ENUM);
  
  ast_set_id(node, parser_parse_id(par));

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  ast_set_members(node, parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_enum_constant));
  
  return node;
}

ast_node_t* parser_parse_decl_mod(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_DECL_MOD);
  ast_set_token(node, parser_current(par));

  parser_expect(par, TOK_KW_MOD);
  
  ast_set_id(node, parser_parse_id(par));

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);
  
  ast_set_decls(node, parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl));
  
  return node;
}

ast_node_t* parser_parse_decl(parser_t* par)
{
  switch (token_get_kind(parser_current(par)))
  {
  case TOK_ID:        return parser_parse_decl_var   (par);
  case TOK_KW_FUN:    return parser_parse_decl_fun   (par);
  case TOK_KW_GEN:    return parser_parse_decl_gen   (par);
  case TOK_KW_STRUCT: return parser_parse_decl_struct(par);
  case TOK_KW_UNION:  return parser_parse_decl_union (par);
  case TOK_KW_ENUM:   return parser_parse_decl_enum  (par);
  case TOK_KW_MOD:    return parser_parse_decl_mod   (par);
  default:            report_error_unexpected_token(token_get_loc(parser_current(par)));
  }

  return NULL;
}

ast_node_t* parser_parse_decl_param(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_DECL_PARAM);
  ast_set_token(node, parser_current(par));

  ast_set_id(node, parser_parse_id(par));

  parser_expect(par, TOK_PUNCT_COLON);

  ast_set_type(node, parser_parse_type(par));

  if (parser_consume(par, TOK_PUNCT_EQUAL))
    ast_set_expr(node, parser_parse_expr(par));

  return node;
}

ast_node_t* parser_parse_decl_enum_constant(parser_t* par)
{
  ast_node_t* node = ast_node_init(AST_DECL_ENUM_CONSTANT);
  ast_set_token(node, parser_current(par));

  ast_set_id(node, parser_parse_id(par));
  
  return node;
}

void parser_parse(parser_t* par, list_t* toks, ast_node_t* root)
{
  par->toks = toks;
  par->cur = list_front_node(toks);
  
  assert(ast_get_kind(root) == AST_PROG);
  ast_set_token(root, parser_current(par));
  ast_set_decls(root, parser_parse_terminated_list(par, TOK_EOF, parser_parse_decl));
}
