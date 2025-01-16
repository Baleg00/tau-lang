/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/bit/not.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/diagnostics.h"

ast_expr_op_un_bit_not_t* ast_expr_op_un_bit_not_init(void)
{
  ast_expr_op_un_bit_not_t* node = (ast_expr_op_un_bit_not_t*)malloc(sizeof(ast_expr_op_un_bit_not_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_BIT_NOT;

  return node;
}

void ast_expr_op_un_bit_not_nameres(nameres_ctx_t* ctx, ast_expr_op_un_bit_not_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_bit_not_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_bit_not_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
  ASSERT(expr_desc != NULL);

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(expr_desc)))
  {
    location_t loc = token_location(node->expr->tok);

    report_error_expected_arithmetic_type(loc);
  }

  typedesc_t* desc = typedesc_remove_ref_mut(expr_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_bit_not_codegen(codegen_ctx_t* ctx, ast_expr_op_un_bit_not_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  LLVMValueRef llvm_value = codegen_build_load_if_ref(ctx, expr);
  node->llvm_value = LLVMBuildNot(ctx->llvm_builder, llvm_value, "");
}
