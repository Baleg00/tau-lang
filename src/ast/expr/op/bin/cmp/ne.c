/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/cmp/ne.h"

#include "ast/ast.h"
#include "ast/registry.h"

static void tau_ast_expr_op_bin_cmp_ne_typecheck_scalar(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_cmp_ne_t* node, tau_typedesc_t* lhs_desc, tau_typedesc_t* rhs_desc)
{
  if (!tau_typedesc_is_arithmetic(tau_typedesc_remove_ref_mut(lhs_desc)))
    tau_error_bag_put_typecheck_expected_arithmetic(ctx->errors, tau_token_location(node->lhs->tok));

  if (!tau_typedesc_is_arithmetic(tau_typedesc_remove_ref_mut(rhs_desc)))
    tau_error_bag_put_typecheck_expected_arithmetic(ctx->errors, tau_token_location(node->rhs->tok));

  tau_typedesc_t* desc = tau_typebuilder_build_bool(ctx->typebuilder);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);

  tau_typedesc_t* promoted_desc = tau_typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_desc, rhs_desc);

  if (tau_typedesc_is_integer(promoted_desc))
    node->op_subkind = OP_CMP_NE_INTEGER;
  else if (tau_typedesc_is_float(promoted_desc))
    node->op_subkind = OP_CMP_NE_FLOAT;
  else if (tau_typedesc_is_complex(promoted_desc))
    node->op_subkind = OP_CMP_NE_COMPLEX;
  else
    TAU_UNREACHABLE();
}

static void tau_ast_expr_op_bin_cmp_ne_typecheck_vector(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_cmp_ne_t* node, tau_typedesc_t* lhs_desc, tau_typedesc_t* rhs_desc)
{
  if (!tau_typedesc_is_vector(tau_typedesc_remove_ref_mut(lhs_desc)))
    tau_error_bag_put_typecheck_expected_vector(ctx->errors, tau_token_location(node->lhs->tok));

  if (!tau_typedesc_is_vector(tau_typedesc_remove_ref_mut(rhs_desc)))
    tau_error_bag_put_typecheck_expected_vector(ctx->errors, tau_token_location(node->rhs->tok));

  tau_typedesc_t* desc = tau_typebuilder_build_bool(ctx->typebuilder);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);

  node->op_subkind = OP_CMP_NE_VECTOR;
}

static void tau_ast_expr_op_bin_cmp_ne_codegen_scalar(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_cmp_ne_t* node)
{
  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  lhs_desc = tau_typedesc_remove_ref_mut(lhs_desc);
  rhs_desc = tau_typedesc_remove_ref_mut(rhs_desc);

  tau_typedesc_t* promoted_desc = tau_typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_desc, rhs_desc);

  llvm_lhs_value = tau_codegen_build_arithmetic_cast(ctx, llvm_lhs_value, lhs_desc, promoted_desc);
  llvm_rhs_value = tau_codegen_build_arithmetic_cast(ctx, llvm_rhs_value, rhs_desc, promoted_desc);

  switch (node->op_subkind)
  {
  case OP_CMP_NE_INTEGER: node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntNE, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_CMP_NE_FLOAT: node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealONE, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_CMP_NE_COMPLEX: node->llvm_value = tau_codegen_build_complex_ne(ctx, llvm_lhs_value, llvm_rhs_value); break;
  default: TAU_UNREACHABLE();
  }
}

static void tau_ast_expr_op_bin_cmp_ne_codegen_vector(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_cmp_ne_t* node)
{
  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  tau_typedesc_vec_t* lhs_vec_desc = (tau_typedesc_vec_t*)tau_typedesc_remove_ref_mut(lhs_desc);
  tau_typedesc_vec_t* rhs_vec_desc = (tau_typedesc_vec_t*)tau_typedesc_remove_ref_mut(rhs_desc);

  tau_typedesc_t* promoted_base_desc = tau_typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_vec_desc->base_type, rhs_vec_desc->base_type);
  tau_typedesc_t* promoted_vec_desc = tau_typebuilder_build_vec(ctx->typebuilder, lhs_vec_desc->size, promoted_base_desc);

  llvm_lhs_value = tau_codegen_build_vector_cast(ctx, llvm_lhs_value, (tau_typedesc_t*)lhs_vec_desc, promoted_vec_desc);
  llvm_rhs_value = tau_codegen_build_vector_cast(ctx, llvm_rhs_value, (tau_typedesc_t*)rhs_vec_desc, promoted_vec_desc);

  node->llvm_value = tau_codegen_build_vector_ne(ctx, promoted_vec_desc, llvm_lhs_value, llvm_rhs_value);
}

tau_ast_expr_op_bin_cmp_ne_t* tau_ast_expr_op_bin_cmp_ne_init(void)
{
  tau_ast_expr_op_bin_cmp_ne_t* node = (tau_ast_expr_op_bin_cmp_ne_t*)malloc(sizeof(tau_ast_expr_op_bin_cmp_ne_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_BINARY;
  node->op_kind = OP_CMP_NE;

  return node;
}

void tau_ast_expr_op_bin_cmp_ne_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_cmp_ne_t* node)
{
  tau_ast_node_nameres(ctx, node->lhs);
  tau_ast_node_nameres(ctx, node->rhs);
}

void tau_ast_expr_op_bin_cmp_ne_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_cmp_ne_t* node)
{
  tau_ast_node_typecheck(ctx, node->lhs);
  tau_ast_node_typecheck(ctx, node->rhs);

  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  TAU_ASSERT(lhs_desc != NULL);

  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);
  TAU_ASSERT(rhs_desc != NULL);

  if (tau_typedesc_is_arithmetic(tau_typedesc_remove_ref_mut(lhs_desc)))
    tau_ast_expr_op_bin_cmp_ne_typecheck_scalar(ctx, node, lhs_desc, rhs_desc);
  else if (tau_typedesc_is_vector(tau_typedesc_remove_ref_mut(lhs_desc)))
    tau_ast_expr_op_bin_cmp_ne_typecheck_vector(ctx, node, lhs_desc, rhs_desc);
  else
    TAU_UNREACHABLE();
}

void tau_ast_expr_op_bin_cmp_ne_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_cmp_ne_t* node)
{
  tau_ast_node_codegen(ctx, node->lhs);
  tau_ast_node_codegen(ctx, node->rhs);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  switch (node->op_subkind)
  {
  case OP_CMP_NE_INTEGER:
  case OP_CMP_NE_FLOAT:
  case OP_CMP_NE_COMPLEX: tau_ast_expr_op_bin_cmp_ne_codegen_scalar(ctx, node); break;
  case OP_CMP_NE_VECTOR: tau_ast_expr_op_bin_cmp_ne_codegen_vector(ctx, node); break;
  default: TAU_UNREACHABLE();
  }
}
