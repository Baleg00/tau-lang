/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/generic/fun.h"

#include "ast/registry.h"

ast_decl_generic_fun_t* ast_decl_generic_fun_init(void)
{
  ast_decl_generic_fun_t* node = (ast_decl_generic_fun_t*)malloc(sizeof(ast_decl_generic_fun_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_GENERIC_FUN;
  node->generic_params = vector_init();
  node->params = vector_init();

  return node;
}

void ast_decl_generic_fun_free(ast_decl_generic_fun_t* node)
{
  vector_free(node->generic_params);
  vector_free(node->params);
  free(node);
}

void ast_decl_generic_fun_nameres(nameres_ctx_t* ctx, ast_decl_generic_fun_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);

  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL)
  {
    error_bag_put_nameres_symbol_collision(ctx->errors, token_location(node->tok), token_location(collision->node->tok));
    return;
  }

  node->scope = nameres_ctx_scope_begin(ctx);

  VECTOR_FOR_LOOP(i, node->generic_params)
  {
    ast_node_t* generic_param = (ast_node_t*)vector_get(node->generic_params, i);

    ast_node_nameres(ctx, generic_param);
  }

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_t* param = (ast_node_t*)vector_get(node->params, i);

    ast_node_nameres(ctx, param);
  }

  ast_node_nameres(ctx, node->return_type);
  ast_node_nameres(ctx, node->stmt);

  nameres_ctx_scope_end(ctx);
}

void ast_decl_generic_fun_dump_json(FILE* stream, ast_decl_generic_fun_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"generic_params\":");
  ast_node_dump_json_vector(stream, node->generic_params);
  fprintf(stream, ",\"params\":");
  ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, ",\"return_type\":");
  ast_node_dump_json(stream, node->return_type);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
