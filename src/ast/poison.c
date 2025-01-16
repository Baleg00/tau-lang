/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/poison.h"

#include "ast/registry.h"

ast_poison_t* ast_poison_init(void)
{
  ast_poison_t* node = (ast_poison_t*)malloc(sizeof(ast_poison_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_POISON;

  return node;
}

void ast_poison_free(ast_poison_t* node)
{
  free(node);
}

void ast_poison_dump_json(FILE* stream, ast_poison_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
