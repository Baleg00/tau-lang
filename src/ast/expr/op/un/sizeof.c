/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/sizeof.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_un_sizeof_t* tau_ast_expr_op_un_sizeof_init(void)
{
  tau_ast_expr_op_un_sizeof_t* node = (tau_ast_expr_op_un_sizeof_t*)malloc(sizeof(tau_ast_expr_op_un_sizeof_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_UNARY;
  node->op_kind = OP_SIZEOF;

  return node;
}

void tau_ast_expr_op_un_sizeof_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_sizeof_t* node)
{
  tau_ast_node_nameres(ctx, node->expr);
}

void tau_ast_expr_op_un_sizeof_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_sizeof_t* node)
{
  tau_ast_node_typecheck(ctx, node->expr);

  tau_typedesc_t* desc = tau_typebuilder_build_usize(ctx->typebuilder);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_op_un_sizeof_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_sizeof_t* node)
{
  tau_ast_node_codegen(ctx, node->expr);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_ast_expr_t* expr = (tau_ast_expr_t*)node->expr;

  uint64_t value = LLVMABISizeOfType(ctx->llvm_layout, expr->llvm_type);
  node->llvm_value = LLVMConstInt(node->llvm_type, value, false);
}
