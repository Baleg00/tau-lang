/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/unwrap_unsafe.h"

#include "ast/ast.h"
#include "ast/registry.h"

ast_expr_op_un_unwrap_unsafe_t* ast_expr_op_un_unwrap_unsafe_init(void)
{
  ast_expr_op_un_unwrap_unsafe_t* node = (ast_expr_op_un_unwrap_unsafe_t*)malloc(sizeof(ast_expr_op_un_unwrap_unsafe_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_UNWRAP_UNSAFE;

  return node;
}

void ast_expr_op_un_unwrap_unsafe_nameres(nameres_ctx_t* ctx, ast_expr_op_un_unwrap_unsafe_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_unwrap_unsafe_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_unwrap_unsafe_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
  ASSERT(expr_desc != NULL);

  if (!typedesc_is_opt(typedesc_remove_ref_mut(expr_desc)))
  {
    error_bag_put_typecheck_expected_optional(ctx->errors, token_location(node->expr->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  typedesc_opt_t* opt_desc = (typedesc_opt_t*)typedesc_remove_ref_mut(expr_desc);
  typedesc_t* desc = opt_desc->base_type;

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_unwrap_unsafe_codegen(codegen_ctx_t* ctx, ast_expr_op_un_unwrap_unsafe_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  typedesc_t* expr_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->expr));

  LLVMValueRef llvm_value_ptr = LLVMBuildStructGEP2(ctx->llvm_builder, expr_desc->llvm_type, expr->llvm_value, 1, "");
  node->llvm_value = LLVMBuildLoad2(ctx->llvm_builder, ((typedesc_opt_t*)expr_desc)->base_type->llvm_type, llvm_value_ptr, "");
}
