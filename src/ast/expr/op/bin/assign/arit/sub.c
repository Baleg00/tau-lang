/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/assign/arit/sub.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "stages/codegen/codegen.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_bin_assign_arit_sub_t* ast_expr_op_bin_assign_arit_sub_init(void)
{
  ast_expr_op_bin_assign_arit_sub_t* node = (ast_expr_op_bin_assign_arit_sub_t*)malloc(sizeof(ast_expr_op_bin_assign_arit_sub_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;
  node->op_kind = OP_ASSIGN_ARIT_SUB;

  return node;
}

void ast_expr_op_bin_assign_arit_sub_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_assign_arit_sub_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_assign_arit_sub_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_assign_arit_sub_t* node)
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

    report_error_expected_reference_type(loc);
  }

  if (typedesc_remove_ref(lhs_desc)->kind != TYPEDESC_MUT)
  {
    location_t loc = token_location(node->lhs->tok);

    report_error_expected_mutable_type(loc);
  }

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)))
  {
    location_t loc = token_location(node->lhs->tok);

    report_error_expected_arithmetic_type(loc);
  }

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
  {
    location_t loc = token_location(node->rhs->tok);

    report_error_expected_arithmetic_type(loc);
  }

  if (typedesc_is_implicitly_convertible(typedesc_remove_ref_mut(rhs_desc), typedesc_remove_ref_mut(lhs_desc), false))
  {
    location_t loc = token_location(node->lhs->tok);

    report_error_type_mismatch(loc, lhs_desc, rhs_desc);
  }

  typetable_insert(ctx->typetable, (ast_node_t*)node, lhs_desc);
}

void ast_expr_op_bin_assign_arit_sub_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_assign_arit_sub_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->lhs);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->rhs);

  if (typedesc_is_integer(desc))
  {
    node->llvm_value = LLVMBuildSub(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "");
  }
  else if (typedesc_is_float(desc))
  {
    node->llvm_value = LLVMBuildFSub(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "");
  }
  else if (typedesc_is_complex(desc))
  {
    node->llvm_value = codegen_build_complex_sub(ctx, llvm_lhs_value, llvm_rhs_value);
  }
  else
  {
    UNREACHABLE();
  }

  LLVMBuildStore(ctx->llvm_builder, node->llvm_value, ((ast_expr_t*)node->lhs)->llvm_value);
  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
}
