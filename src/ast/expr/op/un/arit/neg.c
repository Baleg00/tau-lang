/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/arit/neg.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_un_arit_neg_t* tau_ast_expr_op_un_arit_neg_init(void)
{
  tau_ast_expr_op_un_arit_neg_t* node = (tau_ast_expr_op_un_arit_neg_t*)malloc(sizeof(tau_ast_expr_op_un_arit_neg_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_UNARY;
  node->op_kind = OP_ARIT_NEG;

  return node;
}

void tau_ast_expr_op_un_arit_neg_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_arit_neg_t* node)
{
  tau_ast_node_nameres(ctx, node->expr);
}

void tau_ast_expr_op_un_arit_neg_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_arit_neg_t* node)
{
  tau_ast_node_typecheck(ctx, node->expr);

  tau_typedesc_t* expr_desc = tau_typetable_lookup(ctx->typetable, node->expr);
  TAU_ASSERT(expr_desc != NULL);

  if (!tau_typedesc_is_arithmetic(tau_typedesc_remove_ref_mut(expr_desc)))
  {
    tau_error_bag_put_typecheck_expected_arithmetic(ctx->errors, tau_token_location(node->expr->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  tau_typedesc_t* desc = tau_typedesc_remove_ref_mut(expr_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_op_un_arit_neg_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_arit_neg_t* node)
{
  tau_ast_node_codegen(ctx, node->expr);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_ast_expr_t* expr = (tau_ast_expr_t*)node->expr;

  tau_typedesc_t* expr_desc = tau_typetable_lookup(ctx->typetable, node->expr);

  LLVMValueRef llvm_value = tau_codegen_build_load_if_ref(ctx, expr->llvm_value, expr_desc);

  expr_desc = tau_typedesc_remove_ref_mut(expr_desc);

  if (tau_typedesc_is_integer(expr_desc))
  {
    node->llvm_value = LLVMBuildNeg(ctx->llvm_builder, llvm_value, "");
  }
  else if (tau_typedesc_is_float(expr_desc))
  {
    node->llvm_value = LLVMBuildFNeg(ctx->llvm_builder, llvm_value, "");
  }
  else if (tau_typedesc_is_complex(expr_desc))
  {
    node->llvm_value = tau_codegen_build_complex_neg(ctx, llvm_value);
  }
  else
  {
    TAU_UNREACHABLE();
  }
}
