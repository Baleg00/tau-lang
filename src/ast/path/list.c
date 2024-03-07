/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/path/list.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_path_list_t* ast_path_list_init(void)
{
  ast_path_list_t* node = (ast_path_list_t*)malloc(sizeof(ast_path_list_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_PATH_LIST;

  return node;
}

void ast_path_list_free(ast_path_list_t* node)
{
  vector_free(node->paths);
  free(node);
}

void ast_path_list_dump_json(FILE* stream, ast_path_list_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"paths\":");
  ast_node_dump_json_vector(stream, node->paths);
  fputc('}', stream);
}
