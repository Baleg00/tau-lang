/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/arit/inc_post.h"

#include "ast/ast.h"
#include "ast/registry.h"

ast_expr_op_un_arit_inc_post_t* ast_expr_op_un_arit_inc_post_init(void)
{
  ast_expr_op_un_arit_inc_post_t* node = (ast_expr_op_un_arit_inc_post_t*)malloc(sizeof(ast_expr_op_un_arit_inc_post_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_ARIT_INC_POST;

  return node;
}

void ast_expr_op_un_arit_inc_post_nameres(nameres_ctx_t* ctx, ast_expr_op_un_arit_inc_post_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_arit_inc_post_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_arit_inc_post_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
  ASSERT(expr_desc != NULL);

  if (!typedesc_is_ref(expr_desc))
  {
    error_bag_put_typecheck_expected_reference(ctx->errors, token_location(node->expr->tok));
    return;
  }

  if (!typedesc_is_mut(typedesc_remove_ref(expr_desc)))
  {
    error_bag_put_typecheck_expected_mutable(ctx->errors, token_location(node->expr->tok));
    return;
  }

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(expr_desc)))
  {
    error_bag_put_typecheck_expected_arithmetic(ctx->errors, token_location(node->expr->tok));
    return;
  }

  typedesc_t* desc = typedesc_remove_ref_mut(expr_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_arit_inc_post_codegen(codegen_ctx_t* ctx, ast_expr_op_un_arit_inc_post_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  typedesc_t* expr_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->expr));

  LLVMValueRef llvm_value = LLVMBuildLoad2(ctx->llvm_builder, expr->llvm_type, expr->llvm_value, "");
  LLVMValueRef llvm_inc_value = NULL;

  if (typedesc_is_integer(expr_desc))
    llvm_inc_value = LLVMBuildAdd(ctx->llvm_builder, llvm_value, LLVMConstInt(expr->llvm_type, 1, false), "");
  else if (typedesc_is_float(expr_desc))
    llvm_inc_value = LLVMBuildFAdd(ctx->llvm_builder, llvm_value, LLVMConstReal(expr->llvm_type, 1.0), "");
  else
    UNREACHABLE();

  LLVMBuildStore(ctx->llvm_builder, llvm_inc_value, expr->llvm_value);
  node->llvm_value = llvm_value;
}
