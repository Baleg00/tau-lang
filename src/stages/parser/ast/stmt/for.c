/**
 * \file for.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/stmt/for.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_stmt_for_t* ast_stmt_for_init(void)
{
  ast_stmt_for_t* node = (ast_stmt_for_t*)malloc(sizeof(ast_stmt_for_t));
  assert(node != NULL);

  node->kind = AST_STMT_FOR;

  return node;
}

void ast_stmt_for_dump_json(FILE* stream, ast_stmt_for_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"var\":");
  ast_node_dump_json(stream, node->var);
  fprintf(stream, ",\"range\":");
  ast_node_dump_json(stream, node->range);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
