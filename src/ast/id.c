/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/id.h"

#include "ast/registry.h"

ast_id_t* ast_id_init(void)
{
  ast_id_t* node = (ast_id_t*)malloc(sizeof(ast_id_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_ID;

  return node;
}

void ast_id_free(ast_id_t* node)
{
  free(node);
}

void ast_id_dump_json(FILE* stream, ast_id_t* node)
{
  location_t loc = token_location(node->tok);

  fprintf(stream, "{\"kind\":\"%s\",\"id\":\"%.*s\"}",
    ast_kind_to_cstr(node->kind),
    (int)loc.len, loc.ptr);
}
