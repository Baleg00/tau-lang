/**
 * \file call.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/call.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_expr_op_call_t* ast_expr_op_call_init(void)
{
  ast_expr_op_call_t* node = (ast_expr_op_call_t*)malloc(sizeof(ast_expr_op_call_t));
  assert(node != NULL);

  node->kind = AST_EXPR_OP_CALL;

  return node;
}

void ast_expr_op_call_dump_json(FILE* stream, ast_expr_op_call_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"callee\":");
  ast_node_dump_json(stream, node->callee);
  fprintf(stream, ",\"params\":");
  ast_list_dump_json(stream, node->params);
  fputc('}', stream);
}
