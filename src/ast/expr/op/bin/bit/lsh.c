/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/bit/lsh.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_bin_bit_lsh_t* tau_ast_expr_op_bin_bit_lsh_init(void)
{
  tau_ast_expr_op_bin_bit_lsh_t* node = (tau_ast_expr_op_bin_bit_lsh_t*)malloc(sizeof(tau_ast_expr_op_bin_bit_lsh_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_BINARY;
  node->op_kind = OP_BIT_LSH;

  return node;
}

void tau_ast_expr_op_bin_bit_lsh_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_bit_lsh_t* node)
{
  tau_ast_node_nameres(ctx, node->lhs);
  tau_ast_node_nameres(ctx, node->rhs);
}

void tau_ast_expr_op_bin_bit_lsh_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_bit_lsh_t* node)
{
  tau_ast_node_typecheck(ctx, node->lhs);
  tau_ast_node_typecheck(ctx, node->rhs);

  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  TAU_ASSERT(lhs_desc != NULL);

  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);
  TAU_ASSERT(rhs_desc != NULL);

  if (!tau_typedesc_is_integer(tau_typedesc_remove_ref_mut(lhs_desc)))
  {
    tau_error_bag_put_typecheck_expected_integer(ctx->errors, tau_token_location(node->lhs->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  if (!tau_typedesc_is_integer(tau_typedesc_remove_ref_mut(rhs_desc)))
    tau_error_bag_put_typecheck_expected_integer(ctx->errors, tau_token_location(node->rhs->tok));

  tau_typedesc_t* desc = tau_typedesc_remove_ref_mut(lhs_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_op_bin_bit_lsh_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_bit_lsh_t* node)
{
  tau_ast_node_codegen(ctx, node->lhs);
  tau_ast_node_codegen(ctx, node->rhs);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->lhs)->llvm_value, lhs_desc);
  LLVMValueRef llvm_rhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->rhs)->llvm_value, rhs_desc);

  node->llvm_value = LLVMBuildShl(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "");
}
