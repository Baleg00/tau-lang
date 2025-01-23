/**
* \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/generic/param.h"

#include "ast/registry.h"

ast_decl_generic_param_t* ast_decl_generic_param_init(void)
{
  ast_decl_generic_param_t* node = (ast_decl_generic_param_t*)malloc(sizeof(ast_decl_generic_param_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_GENERIC_PARAM;

  return node;
}

void ast_decl_generic_param_free(ast_decl_generic_param_t* node)
{
  free(node);
}

void ast_decl_generic_param_nameres(nameres_ctx_t* ctx, ast_decl_generic_param_t* node)
{
  ast_node_nameres(ctx, node->type);

  if (node->expr != NULL)
  {
    ast_node_nameres(ctx, node->expr);
  }

  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);

  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL && collision->node->kind == AST_DECL_GENERIC_PARAM)
  {
    error_bag_put_nameres_symbol_collision(ctx->errors, token_location(node->tok), token_location(collision->node->tok));
  }
}

void ast_decl_generic_param_dump_json(FILE* stream, ast_decl_generic_param_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"type\":", ast_kind_to_cstr(node->kind));
  ast_node_dump_json(stream, node->type);
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fprintf(stream, "}");
}
