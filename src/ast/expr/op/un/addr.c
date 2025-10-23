/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/addr.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_un_addr_t* tau_ast_expr_op_un_addr_init(void)
{
  tau_ast_expr_op_un_addr_t* node = (tau_ast_expr_op_un_addr_t*)malloc(sizeof(tau_ast_expr_op_un_addr_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_UNARY;
  node->op_kind = OP_IND;

  return node;
}

void tau_ast_expr_op_un_addr_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_addr_t* node)
{
  tau_ast_node_nameres(ctx, node->expr);
}

void tau_ast_expr_op_un_addr_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_addr_t* node)
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

  tau_typedesc_t* desc = tau_typebuilder_build_ptr(ctx->typebuilder, tau_typedesc_remove_ref(expr_desc));

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_op_un_addr_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_addr_t* node)
{
  tau_ast_node_codegen(ctx, node->expr);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_ast_expr_t* expr = (tau_ast_expr_t*)node->expr;

  node->llvm_value = expr->llvm_value;
}
