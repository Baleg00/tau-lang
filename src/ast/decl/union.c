/**
 * \file union.h
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/union.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_decl_union_t* ast_decl_union_init(void)
{
  ast_decl_union_t* node = (ast_decl_union_t*)malloc(sizeof(ast_decl_union_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_UNION;

  return node;
}

void ast_decl_union_free(ast_decl_union_t* node)
{
  vector_free(node->members);
  free(node);
}

void ast_decl_union_nameres(nameres_ctx_t* ctx, ast_decl_union_t* node)
{
  VECTOR_FOR_LOOP(i, node->members)
    ast_node_nameres(ctx, (ast_node_t*)vector_get(node->members, i));
}

void ast_decl_union_dump_json(FILE* stream, ast_decl_union_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  ast_node_dump_json_vector(stream, node->members);
  fputc('}', stream);
}
