/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/unwrap_safe.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_un_unwrap_safe_t* ast_expr_op_un_unwrap_safe_init(void)
{
  ast_expr_op_un_unwrap_safe_t* node = (ast_expr_op_un_unwrap_safe_t*)malloc(sizeof(ast_expr_op_un_unwrap_safe_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_UNWRAP_SAFE;

  return node;
}

void ast_expr_op_un_unwrap_safe_nameres(nameres_ctx_t* ctx, ast_expr_op_un_unwrap_safe_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_unwrap_safe_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_unwrap_safe_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
  ASSERT(expr_desc != NULL);

  if (typedesc_remove_ref_mut(expr_desc)->kind != TYPEDESC_OPT)
  {
    location_t loc = token_location(node->expr->tok);

    report_error_expected_optional_type(loc);
  }

  typedesc_opt_t* opt_desc = (typedesc_opt_t*)typedesc_remove_ref_mut(expr_desc);
  typedesc_t* desc = opt_desc->base_type;

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_unwrap_safe_codegen(codegen_ctx_t* ctx, ast_expr_op_un_unwrap_safe_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  typedesc_t* expr_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->expr));

  LLVMBasicBlockRef llvm_cond_block = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "unwrap_safe_cond");
  LLVMBasicBlockRef llvm_exit_block = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "unwrap_safe_exit");
  LLVMBasicBlockRef llvm_end_block = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "unwrap_safe_end");

  LLVMBuildBr(ctx->llvm_builder, llvm_cond_block);

  LLVMInsertExistingBasicBlockAfterInsertBlock(ctx->llvm_builder, llvm_cond_block);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, llvm_cond_block);

  LLVMValueRef llvm_flag_ptr = LLVMBuildStructGEP2(ctx->llvm_builder, expr_desc->llvm_type, expr->llvm_value, 0, "gep2_tmp");
  LLVMValueRef llvm_flag_value = LLVMBuildLoad2(ctx->llvm_builder, LLVMInt1TypeInContext(ctx->llvm_ctx), llvm_flag_ptr, "load2_tmp");
  LLVMValueRef llvm_cond_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntEQ, llvm_flag_value, LLVMConstInt(LLVMInt1TypeInContext(ctx->llvm_ctx), 0, false), "icmp_eq_tmp");

  LLVMBuildCondBr(ctx->llvm_builder, llvm_cond_value, llvm_exit_block, llvm_end_block);

  LLVMTypeRef llvm_param_types[] = {
    LLVMInt32TypeInContext(ctx->llvm_ctx)
  };

  LLVMTypeRef llvm_exit_type = LLVMFunctionType(LLVMVoidTypeInContext(ctx->llvm_ctx), llvm_param_types, COUNTOF(llvm_param_types), false);
  LLVMValueRef llvm_exit_func = LLVMAddFunction(ctx->llvm_mod, "exit", llvm_exit_type);
  LLVMSetFunctionCallConv(llvm_exit_func, LLVMCCallConv);

  LLVMInsertExistingBasicBlockAfterInsertBlock(ctx->llvm_builder, llvm_exit_block);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, llvm_exit_block);
  
  LLVMValueRef llvm_args[] = {
    LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), 1, false)
  };

  LLVMBuildCall2(ctx->llvm_builder, llvm_exit_type, llvm_exit_func, llvm_args, COUNTOF(llvm_args), "");
  LLVMBuildUnreachable(ctx->llvm_builder);

  LLVMInsertExistingBasicBlockAfterInsertBlock(ctx->llvm_builder, llvm_end_block);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, llvm_end_block);

  LLVMValueRef llvm_value_ptr = LLVMBuildStructGEP2(ctx->llvm_builder, expr_desc->llvm_type, expr->llvm_value, 1, "gep2_tmp");
  node->llvm_value = LLVMBuildLoad2(ctx->llvm_builder, ((typedesc_opt_t*)expr_desc)->base_type->llvm_type, llvm_value_ptr, "load2_tmp");
}
