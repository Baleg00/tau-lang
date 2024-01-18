/**
 * \file expr.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/expr.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_stmt_expr_t* ast_stmt_expr_init(void)
{
  ast_stmt_expr_t* node = (ast_stmt_expr_t*)malloc(sizeof(ast_stmt_expr_t));
  assert(node != NULL);

  node->kind = AST_STMT_EXPR;

  return node;
}

void ast_stmt_expr_dump_json(FILE* stream, ast_stmt_expr_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
