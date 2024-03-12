/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/path/wildcard.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_path_wildcard_t* ast_path_wildcard_init(void)
{
  ast_path_wildcard_t* node = (ast_path_wildcard_t*)malloc(sizeof(ast_path_wildcard_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_PATH_WILDCARD;

  return node;
}

void ast_path_wildcard_free(ast_path_wildcard_t* node)
{
  free(node);
}

void ast_path_wildcard_dump_json(FILE* stream, ast_path_wildcard_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
