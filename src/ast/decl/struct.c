/**
 * \file struct.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/struct.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_decl_struct_t* ast_decl_struct_init(void)
{
  ast_decl_struct_t* node = (ast_decl_struct_t*)malloc(sizeof(ast_decl_struct_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_STRUCT;

  return node;
}

void ast_decl_struct_free(ast_decl_struct_t* node)
{
  list_free(node->members);
  free(node);
}

void ast_decl_struct_dump_json(FILE* stream, ast_decl_struct_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  ast_list_dump_json(stream, node->members);
  fputc('}', stream); 
}
