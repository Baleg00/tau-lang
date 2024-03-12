/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/defer.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_stmt_defer_t* ast_stmt_defer_init(void)
{
  ast_stmt_defer_t* node = (ast_stmt_defer_t*)malloc(sizeof(ast_stmt_defer_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_DEFER;

  return node;
}

void ast_stmt_defer_free(ast_stmt_defer_t* node)
{
  free(node);
}

void ast_stmt_defer_nameres(nameres_ctx_t* ctx, ast_stmt_defer_t* node)
{
  ast_node_nameres(ctx, node->stmt);
}

void ast_stmt_defer_typecheck(typecheck_ctx_t* ctx, ast_stmt_defer_t* node)
{
  ast_node_typecheck(ctx, node->stmt);
}

void ast_stmt_defer_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_defer_t* node)
{
  ctrlflow_ctx_defer_begin(ctx, node);

  ast_node_ctrlflow(ctx, node->stmt);

  ctrlflow_ctx_defer_end(ctx);
}

void ast_stmt_defer_codegen(codegen_ctx_t* ctx, ast_stmt_defer_t* node)
{
  unused(ctx);
  unused(node);
  
  unreachable();
  // TODO
}

void ast_stmt_defer_dump_json(FILE* stream, ast_stmt_defer_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
