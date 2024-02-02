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
#include "utils/memory/memtrace.h"

ast_stmt_continue_t* ast_stmt_continue_init(void)
{
  ast_stmt_continue_t* node = (ast_stmt_continue_t*)malloc(sizeof(ast_stmt_continue_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_CONTINUE;

  return node;
}

void ast_stmt_continue_free(ast_stmt_continue_t* node)
{
  free(node);
}

void ast_stmt_continue_nameres(nameres_ctx_t* ctx, ast_stmt_continue_t* node)
{
}

void ast_stmt_continue_typecheck(typecheck_ctx_t* ctx, ast_stmt_continue_t* node)
{
}

void ast_stmt_continue_codegen(codegen_ctx_t* ctx, ast_stmt_continue_t* node)
{
  switch (node->loop->kind)
  {
  case AST_STMT_WHILE:
  {
    LLVMBuildBr(ctx->llvm_builder, ((ast_stmt_while_t*)node->loop)->llvm_cond);
    break;
  }
  default:
    unreachable();
  }
}

void ast_stmt_continue_dump_json(FILE* stream, ast_stmt_continue_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
