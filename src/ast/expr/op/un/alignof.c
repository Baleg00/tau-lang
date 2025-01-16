/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/alignof.h"

#include "ast/ast.h"
#include "ast/registry.h"

ast_expr_op_un_alignof_t* ast_expr_op_un_alignof_init(void)
{
  ast_expr_op_un_alignof_t* node = (ast_expr_op_un_alignof_t*)malloc(sizeof(ast_expr_op_un_alignof_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_ALIGNOF;

  return node;
}

void ast_expr_op_un_alignof_nameres(nameres_ctx_t* ctx, ast_expr_op_un_alignof_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_alignof_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_alignof_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* desc = typebuilder_build_usize(ctx->typebuilder);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_alignof_codegen(codegen_ctx_t* ctx, ast_expr_op_un_alignof_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  uint64_t value = LLVMABIAlignmentOfType(ctx->llvm_layout, expr->llvm_type);
  node->llvm_value = LLVMConstInt(node->llvm_type, value, false);
}
