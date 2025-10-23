/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/arit/dec_post.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_un_arit_dec_post_t* tau_ast_expr_op_un_arit_dec_post_init(void)
{
  tau_ast_expr_op_un_arit_dec_post_t* node = (tau_ast_expr_op_un_arit_dec_post_t*)malloc(sizeof(tau_ast_expr_op_un_arit_dec_post_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_UNARY;
  node->op_kind = OP_ARIT_DEC_POST;

  return node;
}

void tau_ast_expr_op_un_arit_dec_post_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_arit_dec_post_t* node)
{
  tau_ast_node_nameres(ctx, node->expr);
}

void tau_ast_expr_op_un_arit_dec_post_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_arit_dec_post_t* node)
{
  tau_ast_node_typecheck(ctx, node->expr);

  tau_typedesc_t* expr_desc = tau_typetable_lookup(ctx->typetable, node->expr);
  TAU_ASSERT(expr_desc != NULL);

  if (!tau_typedesc_is_ref(expr_desc))
  {
    tau_error_bag_put_typecheck_expected_reference(ctx->errors, tau_token_location(node->expr->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  if (!tau_typedesc_is_mut(tau_typedesc_remove_ref(expr_desc)))
    tau_error_bag_put_typecheck_expected_mutable(ctx->errors, tau_token_location(node->expr->tok));

  if (!tau_typedesc_is_arithmetic(tau_typedesc_remove_ref_mut(expr_desc)))
  {
    tau_error_bag_put_typecheck_expected_arithmetic(ctx->errors, tau_token_location(node->expr->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  tau_typedesc_t* desc = tau_typedesc_remove_ref_mut(expr_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_op_un_arit_dec_post_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_arit_dec_post_t* node)
{
  tau_ast_node_codegen(ctx, node->expr);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_ast_expr_t* expr = (tau_ast_expr_t*)node->expr;

  tau_typedesc_t* expr_desc = tau_typedesc_remove_ref_mut(tau_typetable_lookup(ctx->typetable, node->expr));

  LLVMValueRef llvm_value = LLVMBuildLoad2(ctx->llvm_builder, expr->llvm_type, expr->llvm_value, "");
  LLVMValueRef llvm_dec_value = NULL;

  if (tau_typedesc_is_integer(expr_desc))
    llvm_dec_value = LLVMBuildSub(ctx->llvm_builder, llvm_value, LLVMConstInt(expr->llvm_type, 1, false), "");
  else if (tau_typedesc_is_float(expr_desc))
    llvm_dec_value = LLVMBuildFSub(ctx->llvm_builder, llvm_value, LLVMConstReal(expr->llvm_type, 1.0), "");
  else
    TAU_UNREACHABLE();

  LLVMBuildStore(ctx->llvm_builder, llvm_dec_value, expr->llvm_value);
  node->llvm_value = llvm_value;
}
