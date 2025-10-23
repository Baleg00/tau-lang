/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/vec.h"

#include "ast/registry.h"

tau_ast_expr_lit_vec_t* tau_ast_expr_lit_vec_init(void)
{
  tau_ast_expr_lit_vec_t* node = (tau_ast_expr_lit_vec_t*)malloc(sizeof(tau_ast_expr_lit_vec_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_LIT_VEC;
  node->values = tau_vector_init();

  return node;
}

void tau_ast_expr_lit_vec_free(tau_ast_expr_lit_vec_t* node)
{
  tau_vector_free(node->values);
  free(node);
}

void tau_ast_expr_lit_vec_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_lit_vec_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->values)
    tau_ast_node_nameres(ctx, (tau_ast_node_t*)tau_vector_get(node->values, i));
}

void tau_ast_expr_lit_vec_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_lit_vec_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->values)
    tau_ast_node_typecheck(ctx, (tau_ast_node_t*)tau_vector_get(node->values, i));

  tau_typedesc_t* base_desc = NULL;

  TAU_VECTOR_FOR_LOOP(i, node->values)
  {
    tau_ast_node_t* value_node = (tau_ast_node_t*)tau_vector_get(node->values, i);

    tau_typedesc_t* value_desc = tau_typedesc_remove_ref_mut(tau_typetable_lookup(ctx->typetable, value_node));

    if (!tau_typedesc_is_integer(value_desc) && !tau_typedesc_is_float(value_desc))
    {
      tau_error_bag_put_typecheck_expected_integer_or_float(ctx->errors, tau_token_location(value_node->tok));
      tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
      return;
    }

    if (base_desc == NULL)
      base_desc = value_desc;
    else
      base_desc = tau_typebuilder_build_promoted_arithmetic(ctx->typebuilder, base_desc, value_desc);
  }

  tau_typedesc_t* desc = tau_typebuilder_build_vec(ctx->typebuilder, tau_vector_size(node->values), base_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_lit_vec_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_lit_vec_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->values)
    tau_ast_node_codegen(ctx, (tau_ast_node_t*)tau_vector_get(node->values, i));

  tau_typedesc_vec_t* desc = (tau_typedesc_vec_t*)tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  LLVMValueRef* llvm_values = (LLVMValueRef*)malloc(tau_vector_size(node->values) * sizeof(LLVMValueRef));

  if (tau_typedesc_is_integer(desc->base_type))
    TAU_VECTOR_FOR_LOOP(i, node->values)
      llvm_values[i] = LLVMConstInt(desc->base_type->llvm_type, 0, false);
  else if (tau_typedesc_is_float(desc->base_type))
    TAU_VECTOR_FOR_LOOP(i, node->values)
      llvm_values[i] = LLVMConstReal(desc->base_type->llvm_type, 0.0);
  else
    TAU_UNREACHABLE();

  node->llvm_value = LLVMConstVector(llvm_values, (uint32_t)tau_vector_size(node->values));

  free(llvm_values);

  TAU_VECTOR_FOR_LOOP(i, node->values)
  {
    tau_ast_expr_t* value_node = (tau_ast_expr_t*)tau_vector_get(node->values, i);
    tau_typedesc_t* value_desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)value_node);

    LLVMValueRef llvm_value = tau_codegen_build_load_if_ref(ctx, value_node->llvm_value, value_desc);
    llvm_value = tau_codegen_build_arithmetic_cast(ctx, llvm_value, tau_typedesc_remove_ref_mut(value_desc), desc->base_type);

    LLVMValueRef llvm_index = LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), i, false);

    node->llvm_value = LLVMBuildInsertElement(ctx->llvm_builder, node->llvm_value, llvm_value, llvm_index, "");
  }
}

void tau_ast_expr_lit_vec_dump_json(FILE* stream, tau_ast_expr_lit_vec_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"values\":", tau_ast_kind_to_cstr(node->kind));
  tau_ast_node_dump_json_vector(stream, node->values);
  fputc('}', stream);
}
