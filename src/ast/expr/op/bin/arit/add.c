/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/arit/add.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/diagnostics.h"

static void ast_expr_op_bin_arit_add_typecheck_scalar(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_add_t* node, typedesc_t* lhs_desc, typedesc_t* rhs_desc)
{
  if (!typedesc_is_arithmetic(lhs_desc))
  {
    error_bag_put_typecheck_expected_arithmetic(ctx->errors, token_location(node->lhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  if (!typedesc_is_arithmetic(rhs_desc))
  {
    error_bag_put_typecheck_expected_arithmetic(ctx->errors, token_location(node->rhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  if (typedesc_is_signed(lhs_desc) != typedesc_is_signed(rhs_desc))
    report_warning_mixed_signedness(token_location(node->tok));

  typedesc_t* desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_desc, rhs_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);

  if (typedesc_is_integer(desc))
    node->op_subkind = OP_ARIT_ADD_INTEGER;
  else if (typedesc_is_float(desc))
    node->op_subkind = OP_ARIT_ADD_FLOAT;
  else if (typedesc_is_complex(desc))
    node->op_subkind = OP_ARIT_ADD_COMPLEX;
  else
    UNREACHABLE();
}

static void ast_expr_op_bin_arit_add_typecheck_vector(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_add_t* node, typedesc_t* lhs_desc, typedesc_t* rhs_desc)
{
  if (!typedesc_is_vector(lhs_desc))
  {
    error_bag_put_typecheck_expected_vector(ctx->errors, token_location(node->lhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  if (!typedesc_is_vector(rhs_desc))
  {
    error_bag_put_typecheck_expected_vector(ctx->errors, token_location(node->rhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  typedesc_vec_t* lhs_vec_desc = (typedesc_vec_t*)lhs_desc;
  typedesc_vec_t* rhs_vec_desc = (typedesc_vec_t*)rhs_desc;

  if (lhs_vec_desc->size != rhs_vec_desc->size || lhs_vec_desc->is_row != rhs_vec_desc->is_row)
    error_bag_put_typecheck_incompatible_vector_dimensions(ctx->errors, token_location(node->tok));

  if (typedesc_is_signed(lhs_vec_desc->base_type) != typedesc_is_signed(rhs_vec_desc->base_type))
    report_warning_mixed_signedness(token_location(node->tok));

  typedesc_t* base_desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_vec_desc->base_type, rhs_vec_desc->base_type);
  typedesc_t* vec_desc = typebuilder_build_vec(ctx->typebuilder, lhs_vec_desc->is_row, lhs_vec_desc->size, base_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, vec_desc);

  node->op_subkind = OP_ARIT_ADD_VECTOR;
}

static void ast_expr_op_bin_arit_add_typecheck_matrix(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_add_t* node, typedesc_t* lhs_desc, typedesc_t* rhs_desc)
{
  if (!typedesc_is_matrix(lhs_desc))
  {
    error_bag_put_typecheck_expected_matrix(ctx->errors, token_location(node->lhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  if (!typedesc_is_matrix(rhs_desc))
  {
    error_bag_put_typecheck_expected_matrix(ctx->errors, token_location(node->rhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  typedesc_mat_t* lhs_mat_desc = (typedesc_mat_t*)lhs_desc;
  typedesc_mat_t* rhs_mat_desc = (typedesc_mat_t*)rhs_desc;

  if (lhs_mat_desc->rows != rhs_mat_desc->rows || lhs_mat_desc->cols != rhs_mat_desc->cols)
    error_bag_put_typecheck_incompatible_matrix_dimensions(ctx->errors, token_location(node->tok));

  if (typedesc_is_signed(lhs_mat_desc->base_type) != typedesc_is_signed(rhs_mat_desc->base_type))
    report_warning_mixed_signedness(token_location(node->tok));

  typedesc_t* base_desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_mat_desc->base_type, rhs_mat_desc->base_type);
  typedesc_t* mat_desc = typebuilder_build_mat(ctx->typebuilder, lhs_mat_desc->rows, lhs_mat_desc->cols, base_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, mat_desc);

  node->op_subkind = OP_ARIT_ADD_MATRIX;
}

ast_expr_op_bin_arit_add_t* ast_expr_op_bin_arit_add_init(void)
{
  ast_expr_op_bin_arit_add_t* node = (ast_expr_op_bin_arit_add_t*)malloc(sizeof(ast_expr_op_bin_arit_add_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_ARIT_ADD;

  return node;
}

void ast_expr_op_bin_arit_add_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_arit_add_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_arit_add_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_add_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->lhs));
  typedesc_t* rhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->rhs));

  if (typedesc_is_arithmetic(lhs_desc))
    ast_expr_op_bin_arit_add_typecheck_scalar(ctx, node, lhs_desc, rhs_desc);
  else if (typedesc_is_vector(lhs_desc))
    ast_expr_op_bin_arit_add_typecheck_vector(ctx, node, lhs_desc, rhs_desc);
  else if (typedesc_is_matrix(lhs_desc))
    ast_expr_op_bin_arit_add_typecheck_matrix(ctx, node, lhs_desc, rhs_desc);
  else
    UNREACHABLE();
}

void ast_expr_op_bin_arit_add_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_arit_add_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, ((ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, ((ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  switch (node->op_subkind)
  {
  case OP_ARIT_ADD_INTEGER:
  case OP_ARIT_ADD_FLOAT:
  case OP_ARIT_ADD_COMPLEX:
    llvm_lhs_value = codegen_build_arithmetic_cast(ctx, llvm_lhs_value, typedesc_remove_ref_mut(lhs_desc), desc);
    llvm_rhs_value = codegen_build_arithmetic_cast(ctx, llvm_rhs_value, typedesc_remove_ref_mut(rhs_desc), desc);
    break;
  case OP_ARIT_ADD_VECTOR:
    llvm_lhs_value = codegen_build_vector_cast(ctx, llvm_lhs_value, typedesc_remove_ref_mut(lhs_desc), desc);
    llvm_rhs_value = codegen_build_vector_cast(ctx, llvm_rhs_value, typedesc_remove_ref_mut(rhs_desc), desc);
    break;
  case OP_ARIT_ADD_MATRIX:
    llvm_lhs_value = codegen_build_matrix_cast(ctx, llvm_lhs_value, typedesc_remove_ref_mut(lhs_desc), desc);
    llvm_rhs_value = codegen_build_matrix_cast(ctx, llvm_rhs_value, typedesc_remove_ref_mut(rhs_desc), desc);
    break;
  default:
    UNREACHABLE();
  }

  switch (node->op_subkind)
  {
  case OP_ARIT_ADD_INTEGER: node->llvm_value = LLVMBuildAdd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_ARIT_ADD_FLOAT:   node->llvm_value = LLVMBuildFAdd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_ARIT_ADD_COMPLEX: node->llvm_value = codegen_build_complex_add(ctx, llvm_lhs_value, llvm_rhs_value); break;
  case OP_ARIT_ADD_VECTOR:  node->llvm_value = codegen_build_vector_add(ctx, desc, llvm_lhs_value, llvm_rhs_value); break;
  case OP_ARIT_ADD_MATRIX:  node->llvm_value = codegen_build_matrix_add(ctx, desc, llvm_lhs_value, llvm_rhs_value); break;
  default: UNREACHABLE();
  }
}
