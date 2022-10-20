#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

parser_t* parser_init(list_t* toks)
{
  parser_t* par = (parser_t*)malloc(sizeof(parser_t));
  assert(par != NULL);
  par->root = NULL;
  par->toks = toks;
  return par;
}

void parser_free(parser_t* par)
{
  ast_node_free(par->root);
  free(par);
}

token_t* parser_current(parser_t* par)
{
  return (token_t*)list_front(par->toks);
}

token_t* parser_next(parser_t* par)
{
  token_t* tok = (token_t*)list_front(par->toks);
  assert(tok != NULL);

  if (tok->kind != TOK_EOF)
    return (token_t*)list_pop_front(par->toks);

  return tok;
}

token_t* parser_peek(parser_t* par)
{
  token_t* tok = (token_t*)list_front(par->toks);
  assert(tok != NULL);

  if (tok->kind == TOK_EOF)
    return tok;

  return (token_t*)list_elem_data(list_elem_next(list_front_elem(par->toks)));
}

bool parser_consume(parser_t* par, token_kind_t kind)
{
  if (((token_t*)list_front(par->toks))->kind == kind)
  {
    parser_next(par);
    return true;
  }

  return false;
}

token_t* parser_expect(parser_t* par, token_kind_t kind)
{
  assert(((token_t*)list_front(par->toks))->kind == kind);
  return parser_next(par);
}

ast_node_t* parser_node_init(parser_t* par, ast_kind_t kind)
{
  ast_node_t* node = malloc(sizeof(ast_node_t));
  assert(node != NULL);

  node->kind = kind;
  node->tok = (token_t*)list_front(par->toks);

  return node;
}

ast_node_t* parser_parse_delimited_list(parser_t* par, token_kind_t delim, ast_node_t*(*parse_func)(parser_t*))
{
  ast_node_t *node = NULL, *last_node = NULL;

  for (;;)
  {
    ast_node_t* next_node = parser_node_init(par, AST_NODE_LIST);
    next_node->node_list.node = parse_func(par);


    if (node == NULL)
      node = next_node;
    else
      last_node->node_list.next = next_node;

    last_node = next_node;

    if (!parser_consume(par, delim))
      break;
  }

  last_node->node_list.next = NULL;

  return node;
}

ast_node_t* parser_parse_terminated_list(parser_t* par, token_kind_t termin, ast_node_t*(*parse_func)(parser_t*))
{
  ast_node_t *node = NULL, *last_node = NULL;

  for (;;)
  {
    ast_node_t* next_node = parser_node_init(par, AST_NODE_LIST);
    next_node->node_list.node = parse_func(par);

    if (node == NULL)
      node = next_node;
    else
      last_node->node_list.next = next_node;

    last_node = next_node;

    if (parser_consume(par, termin))
      break;
  }

  last_node->node_list.next = NULL;

  return node;
}

ast_node_t* parser_parse_id(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_ID);
  parser_expect(par, TOK_ID);
  return node;
}

ast_node_t* parser_parse_arg(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_ARG);
  node->arg.id = parser_parse_id(par);
  parser_expect(par, TOK_PUNCT_COLON);
  node->arg.type = parser_parse_type(par);
  return node;
}

ast_node_t* parser_parse_type_mut(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_MUT);
  parser_expect(par, TOK_KW_MUT);
  node->type_mut.base_type = parser_parse_type(par);
  return node;
}

ast_node_t* parser_parse_type_const(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_CONST);
  parser_expect(par, TOK_KW_CONST);
  node->type_const.base_type = parser_parse_type(par);
  return node;
}

ast_node_t* parser_parse_type_static(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_STATIC);
  parser_expect(par, TOK_KW_STATIC);
  node->type_static.base_type = parser_parse_type(par);
  return node;
}

ast_node_t* parser_parse_type_ptr(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_PTR);
  parser_expect(par, TOK_PUNCT_ASTERISK);
  node->type_ptr.base_type = parser_parse_type(par);
  return node;
}

ast_node_t* parser_parse_type_array(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_ARRAY);
  parser_expect(par, TOK_PUNCT_BRACKET_LEFT);
  node->type_array.size = parser_parse_expr(par);
  parser_expect(par, TOK_PUNCT_BRACKET_RIGHT);
  node->type_array.base_type = parser_parse_type(par);
  return node;
}

ast_node_t* parser_parse_type_ref(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_REF);
  parser_expect(par, TOK_PUNCT_AMPERSAND);
  node->type_ref.base_type = parser_parse_type(par);
  return node;
}

ast_node_t* parser_parse_type_nullable(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_NULLABLE);
  parser_expect(par, TOK_PUNCT_QUESTION);
  node->type_nullable.base_type = parser_parse_type(par);
  return node;
}

ast_node_t* parser_parse_type_fun(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_FUN);

  parser_expect(par, TOK_KW_FUN);
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  node->type_fun.args = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_type);

  parser_expect(par, TOK_PUNCT_COLON);

  node->type_fun.ret_type = parser_parse_type(par);

  return node;
}

ast_node_t* parser_parse_type_gen(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_TYPE_GEN);

  parser_expect(par, TOK_KW_GEN);
  parser_expect(par, TOK_PUNCT_PAREN_LEFT);
  
  node->type_gen.args = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_type);
  
  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  parser_expect(par, TOK_PUNCT_COLON);

  node->type_gen.ret_type = parser_parse_type(par);

  return node;
}

ast_node_t* parser_parse_type_id(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_UNKNOWN);

  parser_expect(par, TOK_ID);

       if (strcmp(node->tok->id.value, "i8"   ) == 0) node->kind = AST_TYPE_BUILTIN_I8;
  else if (strcmp(node->tok->id.value, "i16"  ) == 0) node->kind = AST_TYPE_BUILTIN_I16;
  else if (strcmp(node->tok->id.value, "i32"  ) == 0) node->kind = AST_TYPE_BUILTIN_I32;
  else if (strcmp(node->tok->id.value, "i64"  ) == 0) node->kind = AST_TYPE_BUILTIN_I64;
  else if (strcmp(node->tok->id.value, "isize") == 0) node->kind = AST_TYPE_BUILTIN_ISIZE;
  else if (strcmp(node->tok->id.value, "u8"   ) == 0) node->kind = AST_TYPE_BUILTIN_U8;
  else if (strcmp(node->tok->id.value, "u16"  ) == 0) node->kind = AST_TYPE_BUILTIN_U16;
  else if (strcmp(node->tok->id.value, "u32"  ) == 0) node->kind = AST_TYPE_BUILTIN_U32;
  else if (strcmp(node->tok->id.value, "u64"  ) == 0) node->kind = AST_TYPE_BUILTIN_U64;
  else if (strcmp(node->tok->id.value, "usize") == 0) node->kind = AST_TYPE_BUILTIN_USIZE;
  else if (strcmp(node->tok->id.value, "f32"  ) == 0) node->kind = AST_TYPE_BUILTIN_F32;
  else if (strcmp(node->tok->id.value, "f64"  ) == 0) node->kind = AST_TYPE_BUILTIN_F64;
  else if (strcmp(node->tok->id.value, "bool" ) == 0) node->kind = AST_TYPE_BUILTIN_BOOL;
  else if (strcmp(node->tok->id.value, "unit" ) == 0) node->kind = AST_TYPE_BUILTIN_UNIT;
  else                                                node->kind = AST_ID;

  return node;
}

ast_node_t* parser_parse_type(parser_t* par)
{
  ast_node_t* node;

  switch (parser_current(par)->kind)
  {
  case TOK_KW_MUT:             return parser_parse_type_mut(par);
  case TOK_KW_CONST:           return parser_parse_type_const(par);
  case TOK_KW_STATIC:          return parser_parse_type_static(par);
  case TOK_PUNCT_ASTERISK:     return parser_parse_type_ptr(par);
  case TOK_PUNCT_BRACKET_LEFT: return parser_parse_type_array(par);
  case TOK_PUNCT_AMPERSAND:    return parser_parse_type_ref(par);
  case TOK_PUNCT_QUESTION:     return parser_parse_type_nullable(par);
  case TOK_KW_FUN:             return parser_parse_type_fun(par);
  case TOK_KW_GEN:             return parser_parse_type_gen(par);
  case TOK_KW_I8:              node = parser_node_init(par, AST_TYPE_BUILTIN_I8   ); parser_expect(par, TOK_KW_I8   ); return node;
  case TOK_KW_I16:             node = parser_node_init(par, AST_TYPE_BUILTIN_I16  ); parser_expect(par, TOK_KW_I16  ); return node;
  case TOK_KW_I32:             node = parser_node_init(par, AST_TYPE_BUILTIN_I32  ); parser_expect(par, TOK_KW_I32  ); return node;
  case TOK_KW_I64:             node = parser_node_init(par, AST_TYPE_BUILTIN_I64  ); parser_expect(par, TOK_KW_I64  ); return node;
  case TOK_KW_ISIZE:           node = parser_node_init(par, AST_TYPE_BUILTIN_ISIZE); parser_expect(par, TOK_KW_ISIZE); return node;
  case TOK_KW_U8:              node = parser_node_init(par, AST_TYPE_BUILTIN_U8   ); parser_expect(par, TOK_KW_U8   ); return node;
  case TOK_KW_U16:             node = parser_node_init(par, AST_TYPE_BUILTIN_U16  ); parser_expect(par, TOK_KW_U16  ); return node;
  case TOK_KW_U32:             node = parser_node_init(par, AST_TYPE_BUILTIN_U32  ); parser_expect(par, TOK_KW_U32  ); return node;
  case TOK_KW_U64:             node = parser_node_init(par, AST_TYPE_BUILTIN_U64  ); parser_expect(par, TOK_KW_U64  ); return node;
  case TOK_KW_USIZE:           node = parser_node_init(par, AST_TYPE_BUILTIN_USIZE); parser_expect(par, TOK_KW_USIZE); return node;  
  case TOK_KW_F32:             node = parser_node_init(par, AST_TYPE_BUILTIN_F32  ); parser_expect(par, TOK_KW_F32  ); return node;
  case TOK_KW_F64:             node = parser_node_init(par, AST_TYPE_BUILTIN_F64  ); parser_expect(par, TOK_KW_F64  ); return node;
  case TOK_KW_BOOL:            node = parser_node_init(par, AST_TYPE_BUILTIN_BOOL ); parser_expect(par, TOK_KW_BOOL ); return node;
  case TOK_KW_UNIT:            node = parser_node_init(par, AST_TYPE_BUILTIN_UNIT ); parser_expect(par, TOK_KW_UNIT ); return node;
  case TOK_ID:                 return parser_parse_type_id(par);
  default: assert(false);
  }

  return NULL;
}

ast_node_t* parser_parse_expr(parser_t* par)
{
  // TODO
  return NULL;
}

ast_node_t* parser_parse_decl_var(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_DECL_VAR);

  parser_expect(par, TOK_KW_VAR);

  node->decl_var.id = parser_parse_id(par);

  if (parser_consume(par, TOK_PUNCT_COLON))
    node->decl_var.type = parser_parse_type(par);

  if (parser_consume(par, TOK_PUNCT_EQUAL))
    node->decl_var.init = parser_parse_expr(par);

  return node;
}

ast_node_t* parser_parse_decl_fun(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_DECL_FUN);

  parser_expect(par, TOK_KW_FUN);

  node->decl_fun.id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  node->decl_fun.args = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_arg);

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  
  if (parser_consume(par, TOK_PUNCT_COLON))
    node->decl_fun.ret_type = parser_parse_type(par);
  else
    node->decl_fun.ret_type = NULL;

  node->decl_fun.expr = parser_parse_expr(par);

  return node;
}

ast_node_t* parser_parse_decl_gen(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_DECL_GEN);

  parser_expect(par, TOK_KW_GEN);

  node->decl_gen.id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_PAREN_LEFT);

  node->decl_gen.args = parser_parse_delimited_list(par, TOK_PUNCT_COMMA, parser_parse_arg);

  parser_expect(par, TOK_PUNCT_PAREN_RIGHT);
  
  if (parser_consume(par, TOK_PUNCT_COLON))
    node->decl_gen.ret_type = parser_parse_type(par);
  else
    node->decl_gen.ret_type = NULL;

  return node;
}

ast_node_t* parser_parse_decl_struct(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_DECL_STRUCT);

  parser_expect(par, TOK_KW_STRUCT);

  node->decl_struct.id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  node->decl_struct.members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_member);

  return node;
}

ast_node_t* parser_parse_decl_union(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_DECL_UNION);

  parser_expect(par, TOK_KW_UNION);

  node->decl_union.id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  node->decl_union.members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_member);

  return node;
}

ast_node_t* parser_parse_decl_enum(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_DECL_ENUM);

  parser_expect(par, TOK_KW_ENUM);

  node->decl_enum.id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  node->decl_enum.members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_id);

  return node;
}

ast_node_t* parser_parse_decl_mod(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_DECL_MOD);

  parser_expect(par, TOK_KW_MOD);

  node->decl_mod.id = parser_parse_id(par);

  parser_expect(par, TOK_PUNCT_BRACE_LEFT);

  node->decl_mod.members = parser_parse_terminated_list(par, TOK_PUNCT_BRACE_RIGHT, parser_parse_decl_member);

  return node;
}

ast_node_t* parser_parse_decl(parser_t* par)
{
  switch (parser_current(par)->kind)
  {
  case TOK_KW_VAR:    return parser_parse_decl_var(par);
  case TOK_KW_FUN:    return parser_parse_decl_fun(par);
  case TOK_KW_GEN:    return parser_parse_decl_gen(par);
  case TOK_KW_STRUCT: return parser_parse_decl_struct(par);
  case TOK_KW_UNION:  return parser_parse_decl_union(par);
  case TOK_KW_ENUM:   return parser_parse_decl_enum(par);
  case TOK_KW_MOD:    return parser_parse_decl_mod(par);
  default: assert(false);
  }

  return NULL;
}

ast_node_t* parser_parse_decl_member(parser_t* par)
{
  ast_node_t* node = parser_node_init(par, AST_DECL_MEMBER);

  node->decl_member.is_pub = parser_consume(par, TOK_KW_PUB);
  node->decl_member.decl = parser_parse_decl(par);

  return node;
}

void parser_parse(parser_t* par)
{
  par->root = parser_parse_terminated_list(par, TOK_EOF, parser_parse_decl);
}
