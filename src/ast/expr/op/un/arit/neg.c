/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/arit/neg.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/diagnostics.h"

ast_expr_op_un_arit_neg_t* ast_expr_op_un_arit_neg_init(void)
{
  ast_expr_op_un_arit_neg_t* node = (ast_expr_op_un_arit_neg_t*)malloc(sizeof(ast_expr_op_un_arit_neg_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_ARIT_NEG;

  return node;
}

void ast_expr_op_un_arit_neg_nameres(nameres_ctx_t* ctx, ast_expr_op_un_arit_neg_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_arit_neg_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_arit_neg_t* node)
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

void ast_expr_op_un_arit_neg_codegen(codegen_ctx_t* ctx, ast_expr_op_un_arit_neg_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  typedesc_t* expr_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->expr));

  LLVMValueRef llvm_value = codegen_build_load_if_ref(ctx, expr);

  if (typedesc_is_integer(expr_desc))
  {
    node->llvm_value = LLVMBuildNeg(ctx->llvm_builder, llvm_value, "");
  }
  else if (typedesc_is_float(expr_desc))
  {
    node->llvm_value = LLVMBuildFNeg(ctx->llvm_builder, llvm_value, "");
  }
  else if (typedesc_is_complex(expr_desc))
  {
    node->llvm_value = codegen_build_complex_neg(ctx, llvm_value);
  }
  else
  {
    UNREACHABLE();
  }
}
