/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/cmp/gt.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/diagnostics.h"

ast_expr_op_bin_cmp_gt_t* ast_expr_op_bin_cmp_gt_init(void)
{
  ast_expr_op_bin_cmp_gt_t* node = (ast_expr_op_bin_cmp_gt_t*)malloc(sizeof(ast_expr_op_bin_cmp_gt_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_CMP_GT;

  return node;
}

void ast_expr_op_bin_cmp_gt_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_cmp_gt_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_cmp_gt_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_cmp_gt_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  ASSERT(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  ASSERT(rhs_desc != NULL);

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)))
  {
    location_t loc = token_location(node->lhs->tok);

    report_error_expected_arithmetic_type(loc);
  }

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
  {
    location_t loc = token_location(node->rhs->tok);

    report_error_expected_arithmetic_type(loc);
  }

  typedesc_t* desc = typebuilder_build_bool(ctx->typebuilder);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_bin_cmp_gt_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_cmp_gt_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  typedesc_t* lhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->lhs));
  typedesc_t* rhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->rhs));

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->lhs);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->rhs);

  typedesc_t* promoted_desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, lhs_desc, rhs_desc);

  llvm_lhs_value = codegen_build_arithmetic_cast(ctx, llvm_lhs_value, lhs_desc, promoted_desc);
  llvm_rhs_value = codegen_build_arithmetic_cast(ctx, llvm_rhs_value, rhs_desc, promoted_desc);

  if (typedesc_is_integer(promoted_desc))
  {
    if (typedesc_is_signed(promoted_desc))
    {
      node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSGT, llvm_lhs_value, llvm_rhs_value, "");
    }
    else
    {
      node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntUGT, llvm_lhs_value, llvm_rhs_value, "");
    }
  }
  else if (typedesc_is_float(promoted_desc))
  {
    node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOGT, llvm_lhs_value, llvm_rhs_value, "");
  }
  else
  {
    UNREACHABLE();
  }
}
