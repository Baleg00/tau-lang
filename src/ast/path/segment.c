/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/path/segment.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_path_segment_t* ast_path_segment_init(void)
{
  ast_path_segment_t* node = (ast_path_segment_t*)malloc(sizeof(ast_path_segment_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_PATH_SEGMENT;

  return node;
}

void ast_path_segment_free(ast_path_segment_t* node)
{
  free(node);
}

void ast_path_segment_dump_json(FILE* stream, ast_path_segment_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fputc('}', stream);
}
