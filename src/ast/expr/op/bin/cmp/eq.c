/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/cmp/eq.h"

#include "ast/ast.h"
#include "ast/registry.h"

static void ast_expr_op_bin_cmp_eq_typecheck_scalar(typecheck_ctx_t* ctx, ast_expr_op_bin_cmp_eq_t* node, typedesc_t* lhs_desc, typedesc_t* rhs_desc)
{
  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)))
    error_bag_put_typecheck_expected_arithmetic(ctx->errors, token_location(node->lhs->tok));

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
    error_bag_put_typecheck_expected_arithmetic(ctx->errors, token_location(node->rhs->tok));

  typedesc_t* desc = typebuilder_build_bool(ctx->typebuilder);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);

  typedesc_t* promoted_desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_desc, rhs_desc);

  if (typedesc_is_integer(promoted_desc))
    node->op_subkind = OP_CMP_EQ_INTEGER;
  else if (typedesc_is_float(promoted_desc))
    node->op_subkind = OP_CMP_EQ_FLOAT;
  else if (typedesc_is_complex(promoted_desc))
    node->op_subkind = OP_CMP_EQ_COMPLEX;
  else
    UNREACHABLE();
}

static void ast_expr_op_bin_cmp_eq_typecheck_vector(typecheck_ctx_t* ctx, ast_expr_op_bin_cmp_eq_t* node, typedesc_t* lhs_desc, typedesc_t* rhs_desc)
{
  if (!typedesc_is_vector(typedesc_remove_ref_mut(lhs_desc)))
    error_bag_put_typecheck_expected_vector(ctx->errors, token_location(node->lhs->tok));

  if (!typedesc_is_vector(typedesc_remove_ref_mut(rhs_desc)))
    error_bag_put_typecheck_expected_vector(ctx->errors, token_location(node->rhs->tok));

  typedesc_t* desc = typebuilder_build_bool(ctx->typebuilder);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);

  node->op_subkind = OP_CMP_EQ_VECTOR;
}

static void ast_expr_op_bin_cmp_eq_codegen_scalar(codegen_ctx_t* ctx, ast_expr_op_bin_cmp_eq_t* node)
{
  typedesc_t* lhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->lhs));
  typedesc_t* rhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->rhs));

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->lhs);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->rhs);

  typedesc_t* promoted_desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_desc, rhs_desc);

  llvm_lhs_value = codegen_build_arithmetic_cast(ctx, llvm_lhs_value, lhs_desc, promoted_desc);
  llvm_rhs_value = codegen_build_arithmetic_cast(ctx, llvm_rhs_value, rhs_desc, promoted_desc);

  switch (node->op_subkind)
  {
  case OP_CMP_EQ_INTEGER: node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntEQ, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_CMP_EQ_FLOAT: node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOEQ, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_CMP_EQ_COMPLEX: node->llvm_value = codegen_build_complex_eq(ctx, llvm_lhs_value, llvm_rhs_value); break;
  default: UNREACHABLE();
  }
}

static void ast_expr_op_bin_cmp_eq_codegen_vector(codegen_ctx_t* ctx, ast_expr_op_bin_cmp_eq_t* node)
{
  typedesc_vec_t* lhs_desc = (typedesc_vec_t*)typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->lhs));
  typedesc_vec_t* rhs_desc = (typedesc_vec_t*)typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->rhs));

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->lhs);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->rhs);

  typedesc_t* promoted_base_desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_desc->base_type, rhs_desc->base_type);
  typedesc_vec_t* promoted_vec_desc = (typedesc_vec_t*)typebuilder_build_vec(ctx->typebuilder, lhs_desc->size, promoted_base_desc);

  llvm_lhs_value = codegen_build_vector_cast(ctx, llvm_lhs_value, (typedesc_t*)lhs_desc, (typedesc_t*)promoted_vec_desc);
  llvm_rhs_value = codegen_build_vector_cast(ctx, llvm_rhs_value, (typedesc_t*)rhs_desc, (typedesc_t*)promoted_vec_desc);

  node->llvm_value = codegen_build_vector_eq(ctx, promoted_vec_desc, llvm_lhs_value, llvm_rhs_value);
}

ast_expr_op_bin_cmp_eq_t* ast_expr_op_bin_cmp_eq_init(void)
{
  ast_expr_op_bin_cmp_eq_t* node = (ast_expr_op_bin_cmp_eq_t*)malloc(sizeof(ast_expr_op_bin_cmp_eq_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_CMP_EQ;

  return node;
}

void ast_expr_op_bin_cmp_eq_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_cmp_eq_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_cmp_eq_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_cmp_eq_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  ASSERT(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  ASSERT(rhs_desc != NULL);

  if (typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)))
    ast_expr_op_bin_cmp_eq_typecheck_scalar(ctx, node, lhs_desc, rhs_desc);
  else if (typedesc_is_vector(typedesc_remove_ref_mut(lhs_desc)))
    ast_expr_op_bin_cmp_eq_typecheck_vector(ctx, node, lhs_desc, rhs_desc);
  else
    UNREACHABLE();
}

void ast_expr_op_bin_cmp_eq_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_cmp_eq_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  switch (node->op_subkind)
  {
  case OP_CMP_EQ_INTEGER:
  case OP_CMP_EQ_FLOAT:
  case OP_CMP_EQ_COMPLEX: ast_expr_op_bin_cmp_eq_codegen_scalar(ctx, node); break;
  case OP_CMP_EQ_VECTOR: ast_expr_op_bin_cmp_eq_codegen_vector(ctx, node); break;
  default: UNREACHABLE();
  }
}
