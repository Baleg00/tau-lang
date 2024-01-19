/**
 * \file call.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/call.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_expr_op_call_t* ast_expr_op_call_init(void)
{
  ast_expr_op_call_t* node = (ast_expr_op_call_t*)malloc(sizeof(ast_expr_op_call_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_CALL;

  return node;
}

void ast_expr_op_call_free(ast_expr_op_call_t* node)
{
  vector_free(node->params);
  free(node);
}

void ast_expr_op_call_nameres(nameres_ctx_t* ctx, ast_expr_op_call_t* node)
{
  ast_node_nameres(ctx, &node->callee);

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_t* param = (ast_node_t*)vector_get(node->params, i);
    ast_node_nameres(ctx, &param);
    vector_set(node->params, i, param);
  }
}

void ast_expr_op_call_dump_json(FILE* stream, ast_expr_op_call_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"callee\":");
  ast_node_dump_json(stream, node->callee);
  fprintf(stream, ",\"params\":");
  ast_node_dump_json_vector(stream, node->params);
  fputc('}', stream);
}
