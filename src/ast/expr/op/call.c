/**
 * \file call.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/call.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
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
  ast_node_nameres(ctx, node->callee);

  VECTOR_FOR_LOOP(i, node->params)
    ast_node_nameres(ctx, (ast_node_t*)vector_get(node->params, i));
}

void ast_expr_op_call_typecheck(typecheck_ctx_t* ctx, ast_expr_op_call_t* node)
{
  ast_node_typecheck(ctx, node->callee);

  VECTOR_FOR_LOOP(i, node->params)
    ast_node_typecheck(ctx, (ast_node_t*)vector_get(node->params, i));

  typedesc_t* callee_desc = typetable_lookup(ctx->typetable, node->callee);
  assert(callee_desc != NULL);

  typedesc_fun_t* fun_desc = (typedesc_fun_t*)typedesc_underlying_callable(callee_desc);
  assert(fun_desc->kind == TYPEDESC_FUN);

  size_t i = 0;
  for (; i < vector_size(node->params) && i < vector_size(fun_desc->param_types); i++)
  {
    ast_node_t* caller_param = (ast_node_t*)vector_get(node->params, i);

    typedesc_t* caller_param_desc = typetable_lookup(ctx->typetable, caller_param);
    assert(caller_param_desc != NULL);

    typedesc_t* callee_param_desc = (typedesc_t*)vector_get(fun_desc->param_types, i);

    if (!typedesc_is_implicitly_convertible(caller_param_desc, callee_param_desc))
      report_error_type_mismatch(caller_param->tok->loc, callee_param_desc, caller_param_desc);
  }

  if (i == vector_size(node->params) && i != vector_size(fun_desc->param_types))
    report_error_too_few_arguments(node->tok->loc);

  if (i != vector_size(node->params) && i == vector_size(fun_desc->param_types))
    if (fun_desc->callconv != CALLCONV_CDECL)
      report_error_too_many_arguments(node->tok->loc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, fun_desc->return_type);
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
