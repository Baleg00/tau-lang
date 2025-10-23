/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/loop.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_stmt_loop_t* tau_ast_stmt_loop_init(void)
{
  tau_ast_stmt_loop_t* node = (tau_ast_stmt_loop_t*)malloc(sizeof(tau_ast_stmt_loop_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_STMT_LOOP;

  return node;
}

void tau_ast_stmt_loop_free(tau_ast_stmt_loop_t* node)
{
  free(node);
}

void tau_ast_stmt_loop_nameres(tau_nameres_ctx_t* ctx, tau_ast_stmt_loop_t* node)
{
  node->scope = tau_nameres_ctx_scope_begin(ctx);

  tau_ast_node_nameres(ctx, node->stmt);

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_stmt_loop_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_stmt_loop_t* node)
{
  tau_ast_node_typecheck(ctx, node->stmt);
}

void tau_ast_stmt_loop_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_loop_t* node)
{
  tau_ctrlflow_ctx_loop_begin(ctx, node);

  tau_ast_node_ctrlflow(ctx, node->stmt);

  tau_ctrlflow_ctx_loop_end(ctx);
}

void tau_ast_stmt_loop_codegen(tau_codegen_ctx_t* ctx, tau_ast_stmt_loop_t* node)
{
  node->llvm_begin = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "loop_begin");
  node->llvm_end = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "loop_end");

  LLVMBuildBr(ctx->llvm_builder, node->llvm_begin);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_begin);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_begin);

  tau_ast_node_codegen(ctx, node->stmt);

  LLVMBuildBr(ctx->llvm_builder, node->llvm_begin);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_end);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_end);
}

void tau_ast_stmt_loop_dump_json(FILE* stream, tau_ast_stmt_loop_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"stmt\":");
  tau_ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
