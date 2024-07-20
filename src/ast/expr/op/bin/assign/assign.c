/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/assign/assign.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "stages/codegen/utils.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_bin_assign_t* ast_expr_op_bin_assign_init(void)
{
  ast_expr_op_bin_assign_t* node = (ast_expr_op_bin_assign_t*)malloc(sizeof(ast_expr_op_bin_assign_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_ASSIGN;

  return node;
}

void ast_expr_op_bin_assign_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_assign_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_assign_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_assign_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  ASSERT(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  ASSERT(rhs_desc != NULL);

  if (lhs_desc->kind != TYPEDESC_REF)
  {
    location_t loc = token_location(node->lhs->tok);

    report_error_expected_reference_type(&loc);
  }

  if (typedesc_remove_ref(lhs_desc)->kind != TYPEDESC_MUT)
  {
    location_t loc = token_location(node->lhs->tok);

    report_error_expected_mutable_type(&loc);
  }

  if (!typedesc_is_implicitly_convertible(rhs_desc, typedesc_remove_ref_mut(lhs_desc)))
  {
    location_t loc = token_location(node->lhs->tok);

    report_error_type_mismatch(&loc, lhs_desc, rhs_desc);
  }

  typetable_insert(ctx->typetable, (ast_node_t*)node, lhs_desc);
}

void ast_expr_op_bin_assign_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_assign_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->rhs);

  LLVMBuildStore(ctx->llvm_builder, llvm_rhs_value, ((ast_expr_t*)node->lhs)->llvm_value);
  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
}
