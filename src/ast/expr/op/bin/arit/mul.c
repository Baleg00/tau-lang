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

static void ast_expr_op_bin_arit_mul_typecheck_scalar(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node, typedesc_t* lhs_desc, typedesc_t* rhs_desc)
{
  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)))
  {
    error_bag_put_typecheck_expected_arithmetic(ctx->errors, token_location(node->lhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
  {
    error_bag_put_typecheck_expected_arithmetic(ctx->errors, token_location(node->rhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  if (typedesc_is_signed(typedesc_remove_ref_mut(lhs_desc)) != typedesc_is_signed(typedesc_remove_ref_mut(rhs_desc)))
    report_warning_mixed_signedness(token_location(node->tok));

  typedesc_t* desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, typedesc_remove_ref_mut(lhs_desc), typedesc_remove_ref_mut(rhs_desc));

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);

  if (typedesc_is_integer(desc))
    node->op_subkind = OP_ARIT_MUL_INTEGER;
  else if (typedesc_is_float(desc))
    node->op_subkind = OP_ARIT_MUL_FLOAT;
  else if (typedesc_is_complex(desc))
    node->op_subkind = OP_ARIT_MUL_COMPLEX;
  else
    UNREACHABLE();
}

static void ast_expr_op_bin_arit_mul_typecheck_vector(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node, typedesc_t* lhs_desc, typedesc_t* rhs_desc)
{
  if (!typedesc_is_vector(typedesc_remove_ref_mut(lhs_desc)))
  {
    error_bag_put_typecheck_expected_vector(ctx->errors, token_location(node->lhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
  {
    error_bag_put_typecheck_expected_arithmetic(ctx->errors, token_location(node->rhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  typedesc_vec_t* lhs_vec_desc = (typedesc_vec_t*)typedesc_remove_ref_mut(lhs_desc);

  if (typedesc_is_signed(lhs_vec_desc->base_type) != typedesc_is_signed(rhs_desc))
    report_warning_mixed_signedness(token_location(node->tok));

  typedesc_t* base_desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_vec_desc->base_type, rhs_desc);
  typedesc_t* vec_desc = typebuilder_build_vec(ctx->typebuilder, lhs_vec_desc->size, base_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, vec_desc);

  node->op_subkind = OP_ARIT_MUL_VECTOR;
}

static void ast_expr_op_bin_arit_mul_codegen_scalar(codegen_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node, typedesc_t* desc)
{
  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, ((ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, ((ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  llvm_lhs_value = codegen_build_arithmetic_cast(ctx, llvm_lhs_value, typedesc_remove_ref_mut(lhs_desc), desc);
  llvm_rhs_value = codegen_build_arithmetic_cast(ctx, llvm_rhs_value, typedesc_remove_ref_mut(rhs_desc), desc);

  switch (node->op_subkind)
  {
  case OP_ARIT_MUL_INTEGER: node->llvm_value = LLVMBuildMul(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_ARIT_MUL_FLOAT: node->llvm_value = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, ""); break;
  case OP_ARIT_MUL_COMPLEX: node->llvm_value = codegen_build_complex_mul(ctx, llvm_lhs_value, llvm_rhs_value); break;
  default: UNREACHABLE();
  }
}

static void ast_expr_op_bin_arit_mul_codegen_vector(codegen_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node, typedesc_t* desc)
{
  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, ((ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, ((ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  lhs_desc = typedesc_remove_ref_mut(lhs_desc);
  rhs_desc = typedesc_remove_ref_mut(rhs_desc);

  LLVMValueRef llvm_vec_value = NULL;
  LLVMValueRef llvm_scalar_value = NULL;

  typedesc_t* vec_desc = NULL;
  typedesc_t* scalar_desc = NULL;

  if (typedesc_is_arithmetic(lhs_desc) && typedesc_is_vector(rhs_desc))
  {
    llvm_scalar_value = llvm_lhs_value;
    llvm_vec_value = llvm_rhs_value;

    scalar_desc = lhs_desc;
    vec_desc = rhs_desc;
  }
  else if (typedesc_is_vector(lhs_desc) && typedesc_is_arithmetic(rhs_desc))
  {
    llvm_vec_value = llvm_lhs_value;
    llvm_scalar_value = llvm_rhs_value;

    vec_desc = lhs_desc;
    scalar_desc = rhs_desc;
  }
  else
    UNREACHABLE();

  llvm_vec_value = codegen_build_vector_cast(ctx, llvm_vec_value, vec_desc, desc);
  llvm_scalar_value = codegen_build_arithmetic_cast(ctx, llvm_scalar_value, scalar_desc, ((typedesc_vec_t*)desc)->base_type);

  node->llvm_value = codegen_build_vector_mul(ctx, desc, llvm_vec_value, llvm_scalar_value);
}

ast_expr_op_bin_arit_mul_t* ast_expr_op_bin_arit_mul_init(void)
{
  ast_expr_op_bin_arit_mul_t* node = (ast_expr_op_bin_arit_mul_t*)malloc(sizeof(ast_expr_op_bin_arit_mul_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_ARIT_MUL;

  return node;
}

void ast_expr_op_bin_arit_mul_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_arit_mul_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  ASSERT(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  ASSERT(rhs_desc != NULL);

  if (typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)) && typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
    ast_expr_op_bin_arit_mul_typecheck_scalar(ctx, node, lhs_desc, rhs_desc);
  else if (typedesc_is_vector(typedesc_remove_ref_mut(lhs_desc)) && typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
    ast_expr_op_bin_arit_mul_typecheck_vector(ctx, node, lhs_desc, rhs_desc);
  else if (typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)) && typedesc_is_vector(typedesc_remove_ref_mut(rhs_desc)))
    ast_expr_op_bin_arit_mul_typecheck_vector(ctx, node, rhs_desc, lhs_desc);
  else
    UNREACHABLE();
}

void ast_expr_op_bin_arit_mul_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  switch (node->op_subkind)
  {
  case OP_ARIT_MUL_INTEGER:
  case OP_ARIT_MUL_FLOAT:
  case OP_ARIT_MUL_COMPLEX: ast_expr_op_bin_arit_mul_codegen_scalar(ctx, node, desc); break;
  case OP_ARIT_MUL_VECTOR: ast_expr_op_bin_arit_mul_codegen_vector(ctx, node, desc); break;
  default: UNREACHABLE();
  }
}
