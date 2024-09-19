/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/continue.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_stmt_continue_t* ast_stmt_continue_init(void)
{
  ast_stmt_continue_t* node = (ast_stmt_continue_t*)malloc(sizeof(ast_stmt_continue_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_CONTINUE;

  return node;
}

void ast_stmt_continue_free(ast_stmt_continue_t* node)
{
  free(node);
}

void ast_stmt_continue_nameres(nameres_ctx_t* UNUSED(ctx), ast_stmt_continue_t* UNUSED(node))
{
}

void ast_stmt_continue_typecheck(typecheck_ctx_t* UNUSED(ctx), ast_stmt_continue_t* UNUSED(node))
{
}

void ast_stmt_continue_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_continue_t* node)
{
  if (vector_empty(ctx->stmts))
  {
    location_t loc = token_location(node->tok);

    report_error_continue_outside_loop(loc);
  }

  for (int i = (int)vector_size(ctx->stmts) - 1; i >= 0; i--)
  {
    ast_node_t* stmt_node = (ast_node_t*)vector_get(ctx->stmts, (size_t)i);

    switch (stmt_node->kind)
    {
    case AST_STMT_WHILE:
    case AST_STMT_DO_WHILE:
    case AST_STMT_LOOP: node->loop = stmt_node; return;
    default: NOOP();
    }
  }

  location_t loc = token_location(node->tok);

  report_error_continue_outside_loop(loc);
}

void ast_stmt_continue_codegen(codegen_ctx_t* ctx, ast_stmt_continue_t* node)
{
  switch (node->loop->kind)
  {
  case AST_STMT_WHILE:    LLVMBuildBr(ctx->llvm_builder, ((ast_stmt_while_t*   )node->loop)->llvm_cond ); break;
  case AST_STMT_DO_WHILE: LLVMBuildBr(ctx->llvm_builder, ((ast_stmt_do_while_t*)node->loop)->llvm_cond ); break;
  case AST_STMT_LOOP:     LLVMBuildBr(ctx->llvm_builder, ((ast_stmt_loop_t*    )node->loop)->llvm_begin); break;
  default: UNREACHABLE();
  }
}

void ast_stmt_continue_dump_json(FILE* stream, ast_stmt_continue_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
