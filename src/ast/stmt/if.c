/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/if.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_stmt_if_t* ast_stmt_if_init(void)
{
  ast_stmt_if_t* node = (ast_stmt_if_t*)malloc(sizeof(ast_stmt_if_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_IF;

  return node;
}

void ast_stmt_if_free(ast_stmt_if_t* node)
{
  free(node);
}

void ast_stmt_if_nameres(nameres_ctx_t* ctx, ast_stmt_if_t* node)
{
  node->scope = nameres_ctx_scope_begin(ctx);

  ast_node_nameres(ctx, node->cond);
  ast_node_nameres(ctx, node->stmt);
  ast_node_nameres(ctx, node->stmt_else);

  nameres_ctx_scope_end(ctx);
}

void ast_stmt_if_typecheck(typecheck_ctx_t* ctx, ast_stmt_if_t* node)
{
  ast_node_typecheck(ctx, node->cond);
  ast_node_typecheck(ctx, node->stmt);
  ast_node_typecheck(ctx, node->stmt_else);

  typedesc_t* cond_desc = typetable_lookup(ctx->typetable, node->cond);
  ASSERT(cond_desc != NULL);

  if (typedesc_remove_ref_mut(cond_desc)->kind != TYPEDESC_BOOL)
  {
    location_t loc = token_location(node->cond->tok);

    report_error_expected_bool_type(loc);
  }
}

void ast_stmt_if_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_if_t* node)
{
  ast_node_ctrlflow(ctx, node->stmt);
  ast_node_ctrlflow(ctx, node->stmt_else);
}

void ast_stmt_if_codegen(codegen_ctx_t* ctx, ast_stmt_if_t* node)
{
  node->llvm_then = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "if_then");

  if (node->stmt_else != NULL)
    node->llvm_else = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "if_else");
  
  node->llvm_end = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "if_end");

  ast_node_codegen(ctx, node->cond);

  LLVMBuildCondBr(ctx->llvm_builder, ((ast_expr_t*)node->cond)->llvm_value, node->llvm_then, node->stmt_else != NULL ? node->llvm_else : node->llvm_end);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_then);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_then);

  ast_node_codegen(ctx, node->stmt);

  if (LLVMGetBasicBlockTerminator(node->llvm_then) == NULL)
    LLVMBuildBr(ctx->llvm_builder, node->llvm_end);

  if (node->stmt_else != NULL)
  {
    LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_else);
    LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_else);

    ast_node_codegen(ctx, node->stmt_else);

    if (LLVMGetBasicBlockTerminator(node->llvm_else) == NULL)
      LLVMBuildBr(ctx->llvm_builder, node->llvm_end);
  }

  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_end);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_end);
}

void ast_stmt_if_dump_json(FILE* stream, ast_stmt_if_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"cond\":");
  ast_node_dump_json(stream, node->cond);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fprintf(stream, ",\"stmt_else\":");
  ast_node_dump_json(stream, node->stmt_else);
  fputc('}', stream);
}
