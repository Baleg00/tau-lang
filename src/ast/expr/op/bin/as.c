/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/as.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "stages/codegen/utils.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_expr_op_bin_as_t* ast_expr_op_bin_as_init(void)
{
  ast_expr_op_bin_as_t* node = (ast_expr_op_bin_as_t*)malloc(sizeof(ast_expr_op_bin_as_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_AS;

  return node;
}

void ast_expr_op_bin_as_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_as_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_as_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_as_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  ASSERT(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  ASSERT(rhs_desc != NULL);

  ASSERT(typedesc_is_explicitly_convertible(lhs_desc, rhs_desc));

  typetable_insert(ctx->typetable, (ast_node_t*)node, rhs_desc);
}

void ast_expr_op_bin_as_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_as_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  typedesc_t* lhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->lhs));
  typedesc_t* rhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->rhs));

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->lhs);

  ASSERT(typedesc_is_arithmetic(lhs_desc) && typedesc_is_arithmetic(rhs_desc));
  // TODO: Add explicit cast for non-arithmetic types.

  node->llvm_value = codegen_build_arithmetic_cast(ctx, llvm_lhs_value, lhs_desc, rhs_desc);
}
