/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/call.h"

#include "ast/registry.h"

tau_ast_expr_op_call_t* tau_ast_expr_op_call_init(void)
{
  tau_ast_expr_op_call_t* node = (tau_ast_expr_op_call_t*)malloc(sizeof(tau_ast_expr_op_call_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_CALL;
  node->params = tau_vector_init();

  return node;
}

void tau_ast_expr_op_call_free(tau_ast_expr_op_call_t* node)
{
  tau_vector_free(node->params);
  free(node);
}

void tau_ast_expr_op_call_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_call_t* node)
{
  tau_ast_node_nameres(ctx, node->callee);

  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_nameres(ctx, (tau_ast_node_t*)tau_vector_get(node->params, i));
  }
}

void tau_ast_expr_op_call_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_call_t* node)
{
  tau_ast_node_typecheck(ctx, node->callee);

  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_typecheck(ctx, (tau_ast_node_t*)tau_vector_get(node->params, i));
  }

  tau_typedesc_t* callee_desc = tau_typetable_lookup(ctx->typetable, node->callee);
  TAU_ASSERT(callee_desc != NULL);

  tau_typedesc_fun_t* fun_desc = (tau_typedesc_fun_t*)tau_typedesc_underlying_callable(callee_desc);
  TAU_ASSERT(fun_desc->kind == TAU_TYPEDESC_FUN);

  size_t i = 0;
  for (; i < tau_vector_size(node->params) && i < tau_vector_size(fun_desc->param_types); i++)
  {
    tau_ast_node_t* caller_param = (tau_ast_node_t*)tau_vector_get(node->params, i);

    tau_typedesc_t* caller_param_desc = tau_typetable_lookup(ctx->typetable, caller_param);
    TAU_ASSERT(caller_param_desc != NULL);

    tau_typedesc_t* callee_param_desc = (tau_typedesc_t*)tau_vector_get(fun_desc->param_types, i);

    if (!tau_typedesc_is_implicitly_direct_convertible(caller_param_desc, callee_param_desc))
      tau_error_bag_put_typecheck_illegal_conversion(ctx->errors, tau_token_location(caller_param->tok));
  }

  if (i == tau_vector_size(node->params) && i != tau_vector_size(fun_desc->param_types))
    tau_error_bag_put_typecheck_too_few_function_parameters(ctx->errors, tau_token_location(node->tok));

  if (i != tau_vector_size(node->params) && i == tau_vector_size(fun_desc->param_types) && fun_desc->callconv != TAU_CALLCONV_CDECL)
    tau_error_bag_put_typecheck_too_many_function_parameters(ctx->errors, tau_token_location(node->tok));

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, fun_desc->return_type);
}

void tau_ast_expr_op_call_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_call_t* node)
{
  tau_ast_node_codegen(ctx, node->callee);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_typedesc_fun_t* fun_desc = (tau_typedesc_fun_t*)tau_typedesc_remove_ref(tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node->callee));

  LLVMValueRef* llvm_param_values = NULL;

  if (tau_vector_size(node->params) > 0)
  {
    llvm_param_values = (LLVMValueRef*)malloc(sizeof(LLVMValueRef) * tau_vector_size(node->params));

    TAU_VECTOR_FOR_LOOP(i, node->params)
    {
      tau_ast_expr_t* param = (tau_ast_expr_t*)tau_vector_get(node->params, i);
      tau_ast_node_codegen(ctx, (tau_ast_node_t*)param);

      tau_typedesc_t* param_desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)param);

      LLVMValueRef llvm_param_value = param->llvm_value;

      if (i < tau_vector_size(fun_desc->param_types))
      {
        tau_typedesc_t* expected_param_desc = (tau_typedesc_t*)tau_vector_get(fun_desc->param_types, i);

        llvm_param_value = tau_codegen_build_implicit_cast(ctx, llvm_param_value, param_desc, expected_param_desc);
      }
      else
      {
        llvm_param_value = tau_codegen_build_load_if_ref(ctx, param->llvm_value, param_desc);

        // C default argument promotions for variadic functions
        if (fun_desc->callconv == TAU_CALLCONV_CDECL && fun_desc->is_vararg)
        {
          tau_typedesc_t* dst_desc = NULL;

          tau_typedesc_t* param_base_desc = tau_typedesc_remove_ref_mut(param_desc);

          switch (param_base_desc->kind)
          {
          case TAU_TYPEDESC_I8:
          case TAU_TYPEDESC_I16:
          case TAU_TYPEDESC_U8:
          case TAU_TYPEDESC_U16: dst_desc = tau_typebuilder_build_i32(ctx->typebuilder); break;
          case TAU_TYPEDESC_F32: dst_desc = tau_typebuilder_build_f64(ctx->typebuilder); break;
          default: TAU_NOOP();
          }

          if (dst_desc != NULL)
            llvm_param_value = tau_codegen_build_arithmetic_cast(ctx, llvm_param_value, param_base_desc, dst_desc);
        }
      }

      llvm_param_values[i] = llvm_param_value;
    }
  }

  node->llvm_value = LLVMBuildCall2(
    ctx->llvm_builder,
    ((tau_ast_expr_t*)node->callee)->llvm_type,
    ((tau_ast_expr_t*)node->callee)->llvm_value,
    llvm_param_values,
    (uint32_t)tau_vector_size(node->params),
    ""
  );

  if (llvm_param_values != NULL)
    free(llvm_param_values);
}

void tau_ast_expr_op_call_dump_json(FILE* stream, tau_ast_expr_op_call_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"callee\":");
  tau_ast_node_dump_json(stream, node->callee);
  fprintf(stream, ",\"params\":");
  tau_ast_node_dump_json_vector(stream, node->params);
  fputc('}', stream);
}
