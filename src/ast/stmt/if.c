/**
 * \file if.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/if.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_stmt_if_t* ast_stmt_if_init(void)
{
  ast_stmt_if_t* node = (ast_stmt_if_t*)malloc(sizeof(ast_stmt_if_t));
  assert(node != NULL);

  node->kind = AST_STMT_IF;

  return node;
}

void ast_stmt_if_dump_json(FILE* stream, ast_stmt_if_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"cond\":");
  ast_node_dump_json(stream, node->cond);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fprintf(stream, ",\"stmt_else\":");
  ast_node_dump_json(stream, node->stmt_else);
  fputc('}', stream);
}
