/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/do_while.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/diagnostics.h"

ast_stmt_do_while_t* ast_stmt_do_while_init(void)
{
  ast_stmt_do_while_t* node = (ast_stmt_do_while_t*)malloc(sizeof(ast_stmt_do_while_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_DO_WHILE;

  return node;
}

void ast_stmt_do_while_free(ast_stmt_do_while_t* node)
{
  free(node);
}

void ast_stmt_do_while_nameres(nameres_ctx_t* ctx, ast_stmt_do_while_t* node)
{
  node->scope = nameres_ctx_scope_begin(ctx);

  ast_node_nameres(ctx, node->cond);
  ast_node_nameres(ctx, node->stmt);

  nameres_ctx_scope_end(ctx);
}

void ast_stmt_do_while_typecheck(typecheck_ctx_t* ctx, ast_stmt_do_while_t* node)
{
  ast_node_typecheck(ctx, node->cond);
  ast_node_typecheck(ctx, node->stmt);

  typedesc_t* cond_desc = typetable_lookup(ctx->typetable, node->cond);
  ASSERT(cond_desc != NULL);

  if (typedesc_remove_ref_mut(cond_desc)->kind != TYPEDESC_BOOL)
  {
    location_t loc = token_location(node->cond->tok);

    report_error_expected_bool_type(loc);
  }
}

void ast_stmt_do_while_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_do_while_t* node)
{
  ctrlflow_ctx_do_while_begin(ctx, node);

  ast_node_ctrlflow(ctx, node->stmt);

  ctrlflow_ctx_do_while_end(ctx);
}

void ast_stmt_do_while_codegen(codegen_ctx_t* ctx, ast_stmt_do_while_t* node)
{
  node->llvm_loop = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "do_while_loop");
  node->llvm_cond = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "do_while_cond");
  node->llvm_end = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "do_while_end");

  LLVMBuildBr(ctx->llvm_builder, node->llvm_loop);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_loop);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_loop);

  ast_node_codegen(ctx, node->stmt);

  LLVMBuildBr(ctx->llvm_builder, node->llvm_cond);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_cond);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_cond);

  ast_node_codegen(ctx, node->cond);

  LLVMValueRef llvm_cond_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->cond);

  LLVMBuildCondBr(ctx->llvm_builder, llvm_cond_value, node->llvm_loop, node->llvm_end);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_end);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_end);
}

void ast_stmt_do_while_dump_json(FILE* stream, ast_stmt_do_while_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"cond\":");
  ast_node_dump_json(stream, node->cond);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
