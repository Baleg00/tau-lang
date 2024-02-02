/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/id.h"

#include <string.h>

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_id_t* ast_id_init(void)
{
  ast_id_t* node = (ast_id_t*)malloc(sizeof(ast_id_t));
  clearobj(node);

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
  fprintf(stream, "{\"kind\":\"%s\",\"id\":\"%.*s\"}",
    ast_kind_to_cstr(node->kind),
    (int)node->tok->loc->len, node->tok->loc->ptr);
}
