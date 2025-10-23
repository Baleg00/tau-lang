/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/alignof.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_un_alignof_t* tau_ast_expr_op_un_alignof_init(void)
{
  tau_ast_expr_op_un_alignof_t* node = (tau_ast_expr_op_un_alignof_t*)malloc(sizeof(tau_ast_expr_op_un_alignof_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_UNARY;
  node->op_kind = OP_ALIGNOF;

  return node;
}

void tau_ast_expr_op_un_alignof_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_alignof_t* node)
{
  tau_ast_node_nameres(ctx, node->expr);
}

void tau_ast_expr_op_un_alignof_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_alignof_t* node)
{
  tau_ast_node_typecheck(ctx, node->expr);

  tau_typedesc_t* desc = tau_typebuilder_build_usize(ctx->typebuilder);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_op_un_alignof_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_alignof_t* node)
{
  tau_ast_node_codegen(ctx, node->expr);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_ast_expr_t* expr = (tau_ast_expr_t*)node->expr;

  uint64_t value = LLVMABIAlignmentOfType(ctx->llvm_layout, expr->llvm_type);
  node->llvm_value = LLVMConstInt(node->llvm_type, value, false);
}
