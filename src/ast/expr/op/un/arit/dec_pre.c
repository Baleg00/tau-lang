/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/arit/dec_pre.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_un_arit_dec_pre_t* ast_expr_op_un_arit_dec_pre_init(void)
{
  ast_expr_op_un_arit_dec_pre_t* node = (ast_expr_op_un_arit_dec_pre_t*)malloc(sizeof(ast_expr_op_un_arit_dec_pre_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_ARIT_DEC_PRE;

  return node;
}

void ast_expr_op_un_arit_dec_pre_nameres(nameres_ctx_t* ctx, ast_expr_op_un_arit_dec_pre_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_arit_dec_pre_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_arit_dec_pre_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
  ASSERT(expr_desc != NULL);

  if (expr_desc->kind != TYPEDESC_REF)
    report_error_expected_reference_type(node->expr->tok->loc);

  if (typedesc_remove_ref(expr_desc)->kind != TYPEDESC_MUT)
    report_error_expected_mutable_type(node->expr->tok->loc);

  if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(expr_desc)))
    report_error_expected_arithmetic_type(node->expr->tok->loc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, expr_desc);
}

void ast_expr_op_un_arit_dec_pre_codegen(codegen_ctx_t* ctx, ast_expr_op_un_arit_dec_pre_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  typedesc_t* expr_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->expr));

  LLVMValueRef llvm_value = LLVMBuildLoad2(ctx->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
  LLVMValueRef llvm_dec_value = NULL;

  if (typedesc_is_integer(expr_desc))
    llvm_dec_value = LLVMBuildSub(ctx->llvm_builder, llvm_value, LLVMConstInt(expr->llvm_type, 1, false), "pre_dec_tmp");
  else if (typedesc_is_float(expr_desc))
    llvm_dec_value = LLVMBuildFSub(ctx->llvm_builder, llvm_value, LLVMConstReal(expr->llvm_type, 1.0), "pre_dec_tmp");
  else
    UNREACHABLE();

  LLVMBuildStore(ctx->llvm_builder, llvm_dec_value, expr->llvm_value);
  node->llvm_value = expr->llvm_value;
}
