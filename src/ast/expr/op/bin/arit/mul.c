/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/arit/mul.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/diagnostics.h"

static void tau_ast_expr_op_bin_arit_mul_typecheck_scalar(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node, tau_typedesc_t* lhs_desc, tau_typedesc_t* rhs_desc)
{
  if (!tau_typedesc_is_arithmetic(lhs_desc))
  {
    tau_error_bag_put_typecheck_expected_arithmetic(ctx->errors, tau_token_location(node->lhs->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  if (!tau_typedesc_is_arithmetic(rhs_desc))
  {
    tau_error_bag_put_typecheck_expected_arithmetic(ctx->errors, tau_token_location(node->rhs->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  if (tau_typedesc_is_signed(lhs_desc) != tau_typedesc_is_signed(rhs_desc))
    tau_report_warning_mixed_signedness(tau_token_location(node->tok));

  tau_typedesc_t* desc = tau_typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_desc, rhs_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);

  if (tau_typedesc_is_integer(desc))
    node->op_subkind = OP_ARIT_MUL_INTEGER;
  else if (tau_typedesc_is_float(desc))
    node->op_subkind = OP_ARIT_MUL_FLOAT;
  else if (tau_typedesc_is_complex(desc))
    node->op_subkind = OP_ARIT_MUL_COMPLEX;
  else
    TAU_UNREACHABLE();
}

static void tau_ast_expr_op_bin_arit_mul_typecheck_vector_scalar(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node, tau_typedesc_t* lhs_desc, tau_typedesc_t* rhs_desc)
{
  if (!tau_typedesc_is_vector(lhs_desc))
  {
    tau_error_bag_put_typecheck_expected_vector(ctx->errors, tau_token_location(node->lhs->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  if (!tau_typedesc_is_arithmetic(rhs_desc))
  {
    tau_error_bag_put_typecheck_expected_arithmetic(ctx->errors, tau_token_location(node->rhs->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  tau_typedesc_vec_t* lhs_vec_desc = (tau_typedesc_vec_t*)lhs_desc;

  if (tau_typedesc_is_signed(lhs_vec_desc->base_type) != tau_typedesc_is_signed(rhs_desc))
    tau_report_warning_mixed_signedness(tau_token_location(node->tok));

  tau_typedesc_t* base_desc = tau_typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_vec_desc->base_type, rhs_desc);
  tau_typedesc_t* vec_desc = tau_typebuilder_build_vec(ctx->typebuilder, lhs_vec_desc->size, base_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, vec_desc);

  node->op_subkind = OP_ARIT_MUL_VECTOR_SCALAR;
}

static void tau_ast_expr_op_bin_arit_mul_typecheck_matrix_scalar(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node, tau_typedesc_t* lhs_desc, tau_typedesc_t* rhs_desc)
{
  if (!tau_typedesc_is_matrix(lhs_desc))
  {
    tau_error_bag_put_typecheck_expected_matrix(ctx->errors, tau_token_location(node->lhs->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  if (!tau_typedesc_is_arithmetic(rhs_desc))
  {
    tau_error_bag_put_typecheck_expected_arithmetic(ctx->errors, tau_token_location(node->rhs->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  tau_typedesc_mat_t* lhs_mat_desc = (tau_typedesc_mat_t*)lhs_desc;

  if (tau_typedesc_is_signed(lhs_mat_desc->base_type) != tau_typedesc_is_signed(rhs_desc))
    tau_report_warning_mixed_signedness(tau_token_location(node->tok));

  tau_typedesc_t* base_desc = tau_typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_mat_desc->base_type, rhs_desc);
  tau_typedesc_t* mat_desc = tau_typebuilder_build_mat(ctx->typebuilder, lhs_mat_desc->rows, lhs_mat_desc->cols, base_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, mat_desc);

  node->op_subkind = OP_ARIT_MUL_MATRIX_SCALAR;
}

static void tau_ast_expr_op_bin_arit_mul_codegen_scalar(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node, tau_typedesc_t* desc)
{
  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  llvm_lhs_value = tau_codegen_build_arithmetic_cast(ctx, llvm_lhs_value, tau_typedesc_remove_ref_mut(lhs_desc), desc);
  llvm_rhs_value = tau_codegen_build_arithmetic_cast(ctx, llvm_rhs_value, tau_typedesc_remove_ref_mut(rhs_desc), desc);

  switch (node->op_subkind)
  {
  case OP_ARIT_MUL_INTEGER: node->llvm_value = LLVMBuildMul(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_ARIT_MUL_FLOAT: node->llvm_value = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_ARIT_MUL_COMPLEX: node->llvm_value = tau_codegen_build_complex_mul(ctx, llvm_lhs_value, llvm_rhs_value); break;
  default: TAU_UNREACHABLE();
  }
}

static void tau_ast_expr_op_bin_arit_mul_codegen_vector_scalar(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node, tau_typedesc_t* desc)
{
  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  lhs_desc = tau_typedesc_remove_ref_mut(lhs_desc);
  rhs_desc = tau_typedesc_remove_ref_mut(rhs_desc);

  if (tau_typedesc_is_arithmetic(lhs_desc) && tau_typedesc_is_vector(rhs_desc))
  {
    TAU_SWAP(lhs_desc, rhs_desc);
    TAU_SWAP(llvm_lhs_value, llvm_rhs_value);
  }

  llvm_lhs_value = tau_codegen_build_vector_cast(ctx, llvm_lhs_value, lhs_desc, desc);
  llvm_rhs_value = tau_codegen_build_arithmetic_cast(ctx, llvm_rhs_value, rhs_desc, ((tau_typedesc_vec_t*)desc)->base_type);

  node->llvm_value = tau_codegen_build_vector_mul_scalar(ctx, desc, llvm_lhs_value, llvm_rhs_value);
}

static void tau_ast_expr_op_bin_arit_mul_codegen_matrix_scalar(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node, tau_typedesc_t* desc)
{
  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  lhs_desc = tau_typedesc_remove_ref_mut(lhs_desc);
  rhs_desc = tau_typedesc_remove_ref_mut(rhs_desc);

  if (tau_typedesc_is_arithmetic(lhs_desc) && tau_typedesc_is_matrix(rhs_desc))
  {
    TAU_SWAP(lhs_desc, rhs_desc);
    TAU_SWAP(llvm_lhs_value, llvm_rhs_value);
  }

  llvm_lhs_value = tau_codegen_build_matrix_cast(ctx, llvm_lhs_value, lhs_desc, desc);
  llvm_rhs_value = tau_codegen_build_arithmetic_cast(ctx, llvm_rhs_value, rhs_desc, ((tau_typedesc_mat_t*)desc)->base_type);

  node->llvm_value = tau_codegen_build_matrix_mul_scalar(ctx, desc, llvm_lhs_value, llvm_rhs_value);
}

tau_ast_expr_op_bin_arit_mul_t* tau_ast_expr_op_bin_arit_mul_init(void)
{
  tau_ast_expr_op_bin_arit_mul_t* node = (tau_ast_expr_op_bin_arit_mul_t*)malloc(sizeof(tau_ast_expr_op_bin_arit_mul_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_BINARY;
  node->op_kind = OP_ARIT_MUL;

  return node;
}

void tau_ast_expr_op_bin_arit_mul_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node)
{
  tau_ast_node_nameres(ctx, node->lhs);
  tau_ast_node_nameres(ctx, node->rhs);
}

void tau_ast_expr_op_bin_arit_mul_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node)
{
  tau_ast_node_typecheck(ctx, node->lhs);
  tau_ast_node_typecheck(ctx, node->rhs);

  tau_typedesc_t* lhs_desc = tau_typedesc_remove_ref_mut(tau_typetable_lookup(ctx->typetable, node->lhs));
  tau_typedesc_t* rhs_desc = tau_typedesc_remove_ref_mut(tau_typetable_lookup(ctx->typetable, node->rhs));

  if (tau_typedesc_is_arithmetic(lhs_desc) && tau_typedesc_is_arithmetic(rhs_desc))
    tau_ast_expr_op_bin_arit_mul_typecheck_scalar(ctx, node, lhs_desc, rhs_desc);
  else if (tau_typedesc_is_vector(lhs_desc) && tau_typedesc_is_arithmetic(rhs_desc))
    tau_ast_expr_op_bin_arit_mul_typecheck_vector_scalar(ctx, node, lhs_desc, rhs_desc);
  else if (tau_typedesc_is_arithmetic(lhs_desc) && tau_typedesc_is_vector(rhs_desc))
    tau_ast_expr_op_bin_arit_mul_typecheck_vector_scalar(ctx, node, rhs_desc, lhs_desc);
  else if (tau_typedesc_is_matrix(lhs_desc) && tau_typedesc_is_arithmetic(rhs_desc))
    tau_ast_expr_op_bin_arit_mul_typecheck_matrix_scalar(ctx, node, lhs_desc, rhs_desc);
  else if (tau_typedesc_is_arithmetic(lhs_desc) && tau_typedesc_is_matrix(rhs_desc))
    tau_ast_expr_op_bin_arit_mul_typecheck_matrix_scalar(ctx, node, rhs_desc, lhs_desc);
  else
    TAU_UNREACHABLE();
}

void tau_ast_expr_op_bin_arit_mul_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_arit_mul_t* node)
{
  tau_ast_node_codegen(ctx, node->lhs);
  tau_ast_node_codegen(ctx, node->rhs);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  switch (node->op_subkind)
  {
  case OP_ARIT_MUL_INTEGER:
  case OP_ARIT_MUL_FLOAT:
  case OP_ARIT_MUL_COMPLEX: tau_ast_expr_op_bin_arit_mul_codegen_scalar(ctx, node, desc); break;
  case OP_ARIT_MUL_VECTOR_SCALAR: tau_ast_expr_op_bin_arit_mul_codegen_vector_scalar(ctx, node, desc); break;
  case OP_ARIT_MUL_MATRIX_SCALAR: tau_ast_expr_op_bin_arit_mul_codegen_matrix_scalar(ctx, node, desc); break;
  default: TAU_UNREACHABLE();
  }
}
