/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/use.h"

#include "ast/registry.h"

ast_use_t* ast_use_init(void)
{
  ast_use_t* node = (ast_use_t*)malloc(sizeof(ast_use_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_USE;

  return node;
}

void ast_use_free(ast_use_t* node)
{
  free(node);
}

void ast_use_dump_json(FILE* stream, ast_use_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"path\":");
  ast_node_dump_json(stream, node->path);
  fputc('}', stream);
}
