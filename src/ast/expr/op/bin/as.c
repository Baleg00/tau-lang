/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/as.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_bin_as_t* tau_ast_expr_op_bin_as_init(void)
{
  tau_ast_expr_op_bin_as_t* node = (tau_ast_expr_op_bin_as_t*)malloc(sizeof(tau_ast_expr_op_bin_as_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_BINARY;
  node->op_kind = OP_AS;

  return node;
}

void tau_ast_expr_op_bin_as_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_as_t* node)
{
  tau_ast_node_nameres(ctx, node->lhs);
  tau_ast_node_nameres(ctx, node->rhs);
}

void tau_ast_expr_op_bin_as_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_as_t* node)
{
  tau_ast_node_typecheck(ctx, node->lhs);
  tau_ast_node_typecheck(ctx, node->rhs);

  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  TAU_ASSERT(lhs_desc != NULL);

  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);
  TAU_ASSERT(rhs_desc != NULL);

  TAU_ASSERT(tau_typedesc_is_explicitly_convertible(lhs_desc, rhs_desc));

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, rhs_desc);
}

void tau_ast_expr_op_bin_as_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_as_t* node)
{
  tau_ast_node_codegen(ctx, node->lhs);
  tau_ast_node_codegen(ctx, node->rhs);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_typedesc_t* lhs_desc = tau_typetable_lookup(ctx->typetable, node->lhs);
  tau_typedesc_t* rhs_desc = tau_typetable_lookup(ctx->typetable, node->rhs);

  LLVMValueRef llvm_lhs_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->lhs)->llvm_value, lhs_desc);

  lhs_desc = tau_typedesc_remove_ref_mut(lhs_desc);
  rhs_desc = tau_typedesc_remove_ref_mut(rhs_desc);

  if (tau_typedesc_is_arithmetic(lhs_desc) && tau_typedesc_is_arithmetic(rhs_desc))
    node->llvm_value = tau_codegen_build_arithmetic_cast(ctx, llvm_lhs_value, lhs_desc, rhs_desc);
  else if (tau_typedesc_is_ptr(lhs_desc) && tau_typedesc_is_ptr(rhs_desc))
    node->llvm_value = llvm_lhs_value;
  else if (tau_typedesc_is_ptr(lhs_desc) && tau_typedesc_is_integer(rhs_desc))
    node->llvm_value = LLVMBuildPtrToInt(ctx->llvm_builder, llvm_lhs_value, rhs_desc->llvm_type, "");
  else if (tau_typedesc_is_integer(lhs_desc) && tau_typedesc_is_ptr(rhs_desc))
    node->llvm_value = LLVMBuildIntToPtr(ctx->llvm_builder, llvm_lhs_value, rhs_desc->llvm_type, "");
  else
    TAU_UNREACHABLE();
}
