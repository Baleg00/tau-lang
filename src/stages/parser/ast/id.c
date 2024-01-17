/**
 * \file id.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/id.h"

#include <string.h>

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_id_t* ast_id_init(void)
{
  ast_id_t* node = (ast_id_t*)malloc(sizeof(ast_id_t));
  assert(node != NULL);

  node->kind = AST_ID;

  return node;
}

void ast_id_dump_json(FILE* stream, ast_id_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"id\":\"%.*s\"}",
    ast_kind_to_cstr(node->kind),
    (int)node->tok->loc->len, node->tok->loc->ptr);
}
