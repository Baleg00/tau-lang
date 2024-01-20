/**
 * \file array.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/array.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_type_array_t* ast_type_array_init(void)
{
  ast_type_array_t* node = (ast_type_array_t*)malloc(sizeof(ast_type_array_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_ARRAY;

  return node;
}

void ast_type_array_free(ast_type_array_t* node)
{
  free(node);
}

void ast_type_array_nameres(nameres_ctx_t* ctx, ast_type_array_t* node)
{
  ast_node_nameres(ctx, node->base_type);
  ast_node_nameres(ctx, node->size);
}

void ast_type_array_dump_json(FILE* stream, ast_type_array_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"base_type\":");
  ast_node_dump_json(stream, node->base_type);
  fprintf(stream, ",\"size\":");
  ast_node_dump_json(stream, node->size);
  fputc('}', stream);
}
