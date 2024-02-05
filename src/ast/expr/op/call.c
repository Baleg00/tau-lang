/**
 * \file
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

void ast_expr_op_call_codegen(codegen_ctx_t* ctx, ast_expr_op_call_t* node)
{
  ast_node_codegen(ctx, node->callee);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
  
  typedesc_fun_t* fun_desc = (typedesc_fun_t*)typedesc_remove_const_ref(typetable_lookup(ctx->typetable, (ast_node_t*)node->callee));

  LLVMValueRef* llvm_param_values = NULL;

  if (vector_size(node->params) > 0)
    llvm_param_values = (LLVMValueRef*)malloc(sizeof(LLVMValueRef) * vector_size(node->params));

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_expr_t* param = (ast_expr_t*)vector_get(node->params, i);
    ast_node_codegen(ctx, (ast_node_t*)param);

    LLVMValueRef llvm_param_value = param->llvm_value;

    if (i < vector_size(fun_desc->param_types))
    {
      typedesc_t* expected_param_desc = (typedesc_t*)vector_get(fun_desc->param_types, i);
      typedesc_t* actual_param_desc = typetable_lookup(ctx->typetable, (ast_node_t*)param);

      if (typedesc_remove_const(expected_param_desc)->kind != TYPEDESC_REF &&
          typedesc_remove_const(actual_param_desc)->kind == TYPEDESC_REF)
        llvm_param_value = LLVMBuildLoad2(ctx->llvm_builder, param->llvm_type, param->llvm_value, "load_tmp");
    }

    llvm_param_values[i] = llvm_param_value;
  }

  node->llvm_value = LLVMBuildCall2(
    ctx->llvm_builder,
    ((ast_expr_t*)node->callee)->llvm_type,
    ((ast_expr_t*)node->callee)->llvm_value,
    llvm_param_values,
    (uint32_t)vector_size(node->params),
    "call2_tmp"
  );

  if (llvm_param_values != NULL)
    free(llvm_param_values);
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
