/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/break.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_stmt_break_t* ast_stmt_break_init(void)
{
  ast_stmt_break_t* node = (ast_stmt_break_t*)malloc(sizeof(ast_stmt_break_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_BREAK;

  return node;
}

void ast_stmt_break_free(ast_stmt_break_t* node)
{
  free(node);
}

void ast_stmt_break_nameres(nameres_ctx_t* ctx, ast_stmt_break_t* node)
{
  unused(ctx);
  unused(node);
}

void ast_stmt_break_typecheck(typecheck_ctx_t* ctx, ast_stmt_break_t* node)
{
  unused(ctx);
  unused(node);
}

void ast_stmt_break_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_break_t* node)
{
  if (vector_empty(ctx->stmts))
    report_error_break_outside_loop(node->tok->loc);
  
  for (int i = (int)vector_size(ctx->stmts) - 1; i >= 0; i--)
  {
    ast_node_t* stmt_node = (ast_node_t*)vector_get(ctx->stmts, (size_t)i);

    switch (stmt_node->kind)
    {
    case AST_STMT_FOR:
    case AST_STMT_WHILE: node->loop = stmt_node; return;
    default: report_error_break_outside_loop(node->tok->loc);
    }
  }
}

void ast_stmt_break_codegen(codegen_ctx_t* ctx, ast_stmt_break_t* node)
{
  switch (node->loop->kind)
  {
  case AST_STMT_WHILE:
  {
    LLVMBuildBr(ctx->llvm_builder, ((ast_stmt_while_t*)node->loop)->llvm_end);
    break;
  }
  default:
    unreachable();
  }
}

void ast_stmt_break_dump_json(FILE* stream, ast_stmt_break_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
