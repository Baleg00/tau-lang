/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/do_while.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_stmt_do_while_t* tau_ast_stmt_do_while_init(void)
{
  tau_ast_stmt_do_while_t* node = (tau_ast_stmt_do_while_t*)malloc(sizeof(tau_ast_stmt_do_while_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_STMT_DO_WHILE;

  return node;
}

void tau_ast_stmt_do_while_free(tau_ast_stmt_do_while_t* node)
{
  free(node);
}

void tau_ast_stmt_do_while_nameres(tau_nameres_ctx_t* ctx, tau_ast_stmt_do_while_t* node)
{
  node->scope = tau_nameres_ctx_scope_begin(ctx);

  tau_ast_node_nameres(ctx, node->cond);
  tau_ast_node_nameres(ctx, node->stmt);

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_stmt_do_while_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_stmt_do_while_t* node)
{
  tau_ast_node_typecheck(ctx, node->cond);
  tau_ast_node_typecheck(ctx, node->stmt);

  tau_typedesc_t* cond_desc = tau_typetable_lookup(ctx->typetable, node->cond);
  TAU_ASSERT(cond_desc != NULL);

  if (tau_typedesc_remove_ref_mut(cond_desc)->kind != TAU_TYPEDESC_BOOL)
    tau_error_bag_put_typecheck_expected_bool(ctx->errors, tau_token_location(node->cond->tok));
}

void tau_ast_stmt_do_while_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_do_while_t* node)
{
  tau_ctrlflow_ctx_do_while_begin(ctx, node);

  tau_ast_node_ctrlflow(ctx, node->stmt);

  tau_ctrlflow_ctx_do_while_end(ctx);
}

void tau_ast_stmt_do_while_codegen(tau_codegen_ctx_t* ctx, tau_ast_stmt_do_while_t* node)
{
  node->llvm_loop = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "do_while_loop");
  node->llvm_cond = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "do_while_cond");
  node->llvm_end = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "do_while_end");

  LLVMBuildBr(ctx->llvm_builder, node->llvm_loop);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_loop);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_loop);

  tau_ast_node_codegen(ctx, node->stmt);

  LLVMBuildBr(ctx->llvm_builder, node->llvm_cond);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_cond);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_cond);

  tau_ast_node_codegen(ctx, node->cond);

  tau_typedesc_t* cond_desc = tau_typetable_lookup(ctx->typetable, node->cond);

  LLVMValueRef llvm_cond_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->cond)->llvm_value, cond_desc);

  LLVMBuildCondBr(ctx->llvm_builder, llvm_cond_value, node->llvm_loop, node->llvm_end);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_end);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_end);
}

void tau_ast_stmt_do_while_dump_json(FILE* stream, tau_ast_stmt_do_while_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"cond\":");
  tau_ast_node_dump_json(stream, node->cond);
  fprintf(stream, ",\"stmt\":");
  tau_ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
