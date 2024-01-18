/**
 * \file while.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/while.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_stmt_while_t* ast_stmt_while_init(void)
{
  ast_stmt_while_t* node = (ast_stmt_while_t*)malloc(sizeof(ast_stmt_while_t));
  assert(node != NULL);

  node->kind = AST_STMT_WHILE;

  return node;
}

void ast_stmt_while_dump_json(FILE* stream, ast_stmt_while_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"cond\":");
  ast_node_dump_json(stream, node->cond);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
