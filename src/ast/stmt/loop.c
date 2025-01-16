/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/loop.h"

#include "ast/ast.h"
#include "ast/registry.h"

ast_stmt_loop_t* ast_stmt_loop_init(void)
{
  ast_stmt_loop_t* node = (ast_stmt_loop_t*)malloc(sizeof(ast_stmt_loop_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_LOOP;

  return node;
}

void ast_stmt_loop_free(ast_stmt_loop_t* node)
{
  free(node);
}

void ast_stmt_loop_nameres(nameres_ctx_t* ctx, ast_stmt_loop_t* node)
{
  node->scope = nameres_ctx_scope_begin(ctx);

  ast_node_nameres(ctx, node->stmt);

  nameres_ctx_scope_end(ctx);
}

void ast_stmt_loop_typecheck(typecheck_ctx_t* ctx, ast_stmt_loop_t* node)
{
  ast_node_typecheck(ctx, node->stmt);
}

void ast_stmt_loop_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_loop_t* node)
{
  ctrlflow_ctx_loop_begin(ctx, node);

  ast_node_ctrlflow(ctx, node->stmt);

  ctrlflow_ctx_loop_end(ctx);
}

void ast_stmt_loop_codegen(codegen_ctx_t* ctx, ast_stmt_loop_t* node)
{
  node->llvm_begin = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "loop_begin");
  node->llvm_end = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "loop_end");

  LLVMBuildBr(ctx->llvm_builder, node->llvm_begin);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_begin);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_begin);

  ast_node_codegen(ctx, node->stmt);

  LLVMBuildBr(ctx->llvm_builder, node->llvm_begin);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_end);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_end);
}

void ast_stmt_loop_dump_json(FILE* stream, ast_stmt_loop_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
