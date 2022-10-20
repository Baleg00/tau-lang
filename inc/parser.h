#ifndef TAU_PARSER_H
#define TAU_PARSER_H

#include <stdbool.h>

#include "ast.h"
#include "token.h"
#include "list.h"

typedef struct parser_s
{
  ast_node_t* root;
  list_t* toks;
} parser_t;

parser_t* parser_init(list_t* toks);
void parser_free(parser_t* par);

ast_node_t* parser_node_init(parser_t* par, ast_kind_t kind);

token_t* parser_current(parser_t* par);
token_t* parser_next(parser_t* par);
token_t* parser_peek(parser_t* par);
bool parser_consume(parser_t* par, token_kind_t kind);
token_t* parser_expect(parser_t* par, token_kind_t kind);

ast_node_t* parser_parse_delimited_list(parser_t* par, token_kind_t delim, ast_node_t*(*parse_func)(parser_t*));
ast_node_t* parser_parse_terminated_list(parser_t* par, token_kind_t termin, ast_node_t*(*parse_func)(parser_t*));

ast_node_t* parser_parse_id(parser_t* par);

ast_node_t* parser_parse_arg(parser_t* par);

ast_node_t* parser_parse_type_mut(parser_t* par);
ast_node_t* parser_parse_type_const(parser_t* par);
ast_node_t* parser_parse_type_static(parser_t* par);
ast_node_t* parser_parse_type_ptr(parser_t* par);
ast_node_t* parser_parse_type_array(parser_t* par);
ast_node_t* parser_parse_type_ref(parser_t* par);
ast_node_t* parser_parse_type_nullable(parser_t* par);
ast_node_t* parser_parse_type_fun(parser_t* par);
ast_node_t* parser_parse_type_gen(parser_t* par);
ast_node_t* parser_parse_type_id(parser_t* par);
ast_node_t* parser_parse_type(parser_t* par);

ast_node_t* parser_parse_expr(parser_t* par);

ast_node_t* parser_parse_decl_var(parser_t* par);
ast_node_t* parser_parse_decl_fun(parser_t* par);
ast_node_t* parser_parse_decl_gen(parser_t* par);
ast_node_t* parser_parse_decl_struct(parser_t* par);
ast_node_t* parser_parse_decl_union(parser_t* par);
ast_node_t* parser_parse_decl_enum(parser_t* par);
ast_node_t* parser_parse_decl_mod(parser_t* par);
ast_node_t* parser_parse_decl(parser_t* par);
ast_node_t* parser_parse_decl_member(parser_t* par);

void parser_parse(parser_t* par);

#endif