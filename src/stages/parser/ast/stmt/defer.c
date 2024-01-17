/**
 * \file defer.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/stmt/defer.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_stmt_defer_t* ast_stmt_defer_init(void)
{
  ast_stmt_defer_t* node = (ast_stmt_defer_t*)malloc(sizeof(ast_stmt_defer_t));
  assert(node != NULL);

  node->kind = AST_STMT_DEFER;

  return node;
}

void ast_stmt_defer_dump_json(FILE* stream, ast_stmt_defer_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
