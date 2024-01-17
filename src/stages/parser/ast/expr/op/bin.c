/**
 * \file bin.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/expr/op/bin.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_expr_op_bin_t* ast_expr_op_bin_init(void)
{
  ast_expr_op_bin_t* node = (ast_expr_op_bin_t*)malloc(sizeof(ast_expr_op_bin_t));
  assert(node != NULL);

  node->kind = AST_EXPR_OP_BINARY;

  return node;
}

void ast_expr_op_bin_dump_json(FILE* stream, ast_expr_op_bin_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"lhs\":");
  ast_node_dump_json(stream, node->lhs);
  fprintf(stream, ",\"rhs\":");
  ast_node_dump_json(stream, node->rhs);
  fputc('}', stream);
}
