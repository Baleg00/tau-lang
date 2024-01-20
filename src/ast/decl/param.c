/**
 * \file param.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/param.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_decl_param_t* ast_decl_param_init(void)
{
  ast_decl_param_t* node = (ast_decl_param_t*)malloc(sizeof(ast_decl_param_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_PARAM;

  return node;
}

void ast_decl_param_free(ast_decl_param_t* node)
{
  free(node);
}

void ast_decl_param_nameres(nameres_ctx_t* ctx, ast_decl_param_t* node)
{
  ast_node_nameres(ctx, node->type);
  ast_node_nameres(ctx, node->expr);
}

void ast_decl_param_dump_json(FILE* stream, ast_decl_param_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"type\":");
  ast_node_dump_json(stream, node->type);
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fputc('}', stream); 
}
