/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/bit/lsh.h"

#include "ast/ast.h"
#include "ast/registry.h"

ast_expr_op_bin_bit_lsh_t* ast_expr_op_bin_bit_lsh_init(void)
{
  ast_expr_op_bin_bit_lsh_t* node = (ast_expr_op_bin_bit_lsh_t*)malloc(sizeof(ast_expr_op_bin_bit_lsh_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_BIT_LSH;

  return node;
}

void ast_expr_op_bin_bit_lsh_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_bit_lsh_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_bit_lsh_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_bit_lsh_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  ASSERT(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  ASSERT(rhs_desc != NULL);

  if (!typedesc_is_integer(typedesc_remove_ref_mut(lhs_desc)))
  {
    error_bag_put_typecheck_expected_integer(ctx->errors, token_location(node->lhs->tok));
    typecheck_poison(ctx, (ast_node_t*)node);
    return;
  }

  if (!typedesc_is_integer(typedesc_remove_ref_mut(rhs_desc)))
    error_bag_put_typecheck_expected_integer(ctx->errors, token_location(node->rhs->tok));

  typedesc_t* desc = typedesc_remove_ref_mut(lhs_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_bin_bit_lsh_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_bit_lsh_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->lhs);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildShl(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "");
}
