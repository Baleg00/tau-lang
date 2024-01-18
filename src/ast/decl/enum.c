/**
 * \file enum.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/enum.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_decl_enum_t* ast_decl_enum_init(void)
{
  ast_decl_enum_t* node = (ast_decl_enum_t*)malloc(sizeof(ast_decl_enum_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_ENUM;

  return node;
}

void ast_decl_enum_free(ast_decl_enum_t* node)
{
  vector_free(node->members);
  free(node);
}

void ast_decl_enum_dump_json(FILE* stream, ast_decl_enum_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  ast_node_dump_json_vector(stream, node->members);
  fputc('}', stream);
}
