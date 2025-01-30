/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/call.h"

#include "ast/registry.h"

ast_expr_op_call_t* ast_expr_op_call_init(void)
{
  ast_expr_op_call_t* node = (ast_expr_op_call_t*)malloc(sizeof(ast_expr_op_call_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_CALL;
  node->params = vector_init();

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
  {
    ast_node_nameres(ctx, (ast_node_t*)vector_get(node->params, i));
  }
}

void ast_expr_op_call_typecheck(typecheck_ctx_t* ctx, ast_expr_op_call_t* node)
{
  ast_node_typecheck(ctx, node->callee);

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_typecheck(ctx, (ast_node_t*)vector_get(node->params, i));
  }

  typedesc_t* callee_desc = typetable_lookup(ctx->typetable, node->callee);
  ASSERT(callee_desc != NULL);

  typedesc_fun_t* fun_desc = (typedesc_fun_t*)typedesc_underlying_callable(callee_desc);
  ASSERT(fun_desc->kind == TYPEDESC_FUN);

  size_t i = 0;
  for (; i < vector_size(node->params) && i < vector_size(fun_desc->param_types); i++)
  {
    ast_node_t* caller_param = (ast_node_t*)vector_get(node->params, i);

    typedesc_t* caller_param_desc = typetable_lookup(ctx->typetable, caller_param);
    ASSERT(caller_param_desc != NULL);

    typedesc_t* callee_param_desc = (typedesc_t*)vector_get(fun_desc->param_types, i);

    if (!typedesc_is_implicitly_direct_convertible(caller_param_desc, callee_param_desc))
      error_bag_put_typecheck_illegal_conversion(ctx->errors, token_location(caller_param->tok));
  }

  if (i == vector_size(node->params) && i != vector_size(fun_desc->param_types))
    error_bag_put_typecheck_too_few_function_parameters(ctx->errors, token_location(node->tok));

  if (i != vector_size(node->params) && i == vector_size(fun_desc->param_types) && fun_desc->callconv != CALLCONV_CDECL)
    error_bag_put_typecheck_too_many_function_parameters(ctx->errors, token_location(node->tok));

  typetable_insert(ctx->typetable, (ast_node_t*)node, fun_desc->return_type);
}

void ast_expr_op_call_codegen(codegen_ctx_t* ctx, ast_expr_op_call_t* node)
{
  ast_node_codegen(ctx, node->callee);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  typedesc_fun_t* fun_desc = (typedesc_fun_t*)typedesc_remove_ref(typetable_lookup(ctx->typetable, (ast_node_t*)node->callee));

  LLVMValueRef* llvm_param_values = NULL;

  if (vector_size(node->params) > 0)
  {
    llvm_param_values = (LLVMValueRef*)malloc(sizeof(LLVMValueRef) * vector_size(node->params));

    VECTOR_FOR_LOOP(i, node->params)
    {
      ast_expr_t* param = (ast_expr_t*)vector_get(node->params, i);
      ast_node_codegen(ctx, (ast_node_t*)param);

      typedesc_t* param_desc = typetable_lookup(ctx->typetable, (ast_node_t*)param);

      LLVMValueRef llvm_param_value = param->llvm_value;

      if (i < vector_size(fun_desc->param_types))
      {
        typedesc_t* expected_param_desc = (typedesc_t*)vector_get(fun_desc->param_types, i);

        llvm_param_value = codegen_build_implicit_cast(ctx, llvm_param_value, param_desc, expected_param_desc);
      }
      else
        llvm_param_value = codegen_build_load_if_ref(ctx, param->llvm_value, param_desc);

      llvm_param_values[i] = llvm_param_value;
    }
  }

  node->llvm_value = LLVMBuildCall2(
    ctx->llvm_builder,
    ((ast_expr_t*)node->callee)->llvm_type,
    ((ast_expr_t*)node->callee)->llvm_value,
    llvm_param_values,
    (uint32_t)vector_size(node->params),
    ""
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
