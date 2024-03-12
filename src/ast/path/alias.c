/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/path/alias.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_path_alias_t* ast_path_alias_init(void)
{
  ast_path_alias_t* node = (ast_path_alias_t*)malloc(sizeof(ast_path_alias_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_PATH_ALIAS;

  return node;
}

void ast_path_alias_free(ast_path_alias_t* node)
{
  free(node);
}

void ast_path_alias_dump_json(FILE* stream, ast_path_alias_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"path\":");
  ast_node_dump_json(stream, node->path);
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fputc('}', stream);
}
