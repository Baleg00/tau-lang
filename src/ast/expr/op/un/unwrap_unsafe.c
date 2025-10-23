/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/unwrap_unsafe.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_un_unwrap_unsafe_t* tau_ast_expr_op_un_unwrap_unsafe_init(void)
{
  tau_ast_expr_op_un_unwrap_unsafe_t* node = (tau_ast_expr_op_un_unwrap_unsafe_t*)malloc(sizeof(tau_ast_expr_op_un_unwrap_unsafe_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_UNARY;
  node->op_kind = OP_UNWRAP_UNSAFE;

  return node;
}

void tau_ast_expr_op_un_unwrap_unsafe_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_unwrap_unsafe_t* node)
{
  tau_ast_node_nameres(ctx, node->expr);
}

void tau_ast_expr_op_un_unwrap_unsafe_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_unwrap_unsafe_t* node)
{
  tau_ast_node_typecheck(ctx, node->expr);

  tau_typedesc_t* expr_desc = tau_typetable_lookup(ctx->typetable, node->expr);
  TAU_ASSERT(expr_desc != NULL);

  if (!tau_typedesc_is_opt(tau_typedesc_remove_ref_mut(expr_desc)))
  {
    tau_error_bag_put_typecheck_expected_optional(ctx->errors, tau_token_location(node->expr->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  tau_typedesc_opt_t* opt_desc = (tau_typedesc_opt_t*)tau_typedesc_remove_ref_mut(expr_desc);
  tau_typedesc_t* desc = opt_desc->base_type;

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_op_un_unwrap_unsafe_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_unwrap_unsafe_t* node)
{
  tau_ast_node_codegen(ctx, node->expr);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_ast_expr_t* expr = (tau_ast_expr_t*)node->expr;

  tau_typedesc_t* expr_desc = tau_typedesc_remove_ref_mut(tau_typetable_lookup(ctx->typetable, node->expr));

  LLVMValueRef llvm_value_ptr = LLVMBuildStructGEP2(ctx->llvm_builder, expr_desc->llvm_type, expr->llvm_value, 1, "");
  node->llvm_value = LLVMBuildLoad2(ctx->llvm_builder, ((tau_typedesc_opt_t*)expr_desc)->base_type->llvm_type, llvm_value_ptr, "");
}
