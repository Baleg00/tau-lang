/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/arit/div.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/diagnostics.h"

ast_expr_op_bin_arit_div_t* ast_expr_op_bin_arit_div_init(void)
{
  ast_expr_op_bin_arit_div_t* node = (ast_expr_op_bin_arit_div_t*)malloc(sizeof(ast_expr_op_bin_arit_div_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_ARIT_DIV;

  return node;
}

void ast_expr_op_bin_arit_div_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_arit_div_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_arit_div_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_div_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  ASSERT(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  ASSERT(rhs_desc != NULL);

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
}

void ast_expr_op_bin_arit_div_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_arit_div_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, ((ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, ((ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  llvm_lhs_value = codegen_build_arithmetic_cast(ctx, llvm_lhs_value, typedesc_remove_ref_mut(lhs_desc), desc);
  llvm_rhs_value = codegen_build_arithmetic_cast(ctx, llvm_rhs_value, typedesc_remove_ref_mut(rhs_desc), desc);

  if (typedesc_is_integer(desc))
  {
    if (typedesc_is_signed(desc))
    {
      node->llvm_value = LLVMBuildSDiv(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "");
    }
    else
    {
      node->llvm_value = LLVMBuildUDiv(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "");
    }
  }
  else if (typedesc_is_float(desc))
  {
    node->llvm_value = LLVMBuildFDiv(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "");
  }
  else if (typedesc_is_complex(desc))
  {
    node->llvm_value = codegen_build_complex_div(ctx, llvm_lhs_value, llvm_rhs_value);
  }
  else
  {
    UNREACHABLE();
  }
}
