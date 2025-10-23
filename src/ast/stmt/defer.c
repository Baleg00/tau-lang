/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/defer.h"

#include "ast/registry.h"

tau_ast_stmt_defer_t* tau_ast_stmt_defer_init(void)
{
  tau_ast_stmt_defer_t* node = (tau_ast_stmt_defer_t*)malloc(sizeof(tau_ast_stmt_defer_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_STMT_DEFER;

  return node;
}

void tau_ast_stmt_defer_free(tau_ast_stmt_defer_t* node)
{
  free(node);
}

void tau_ast_stmt_defer_nameres(tau_nameres_ctx_t* ctx, tau_ast_stmt_defer_t* node)
{
  tau_ast_node_nameres(ctx, node->stmt);
}

void tau_ast_stmt_defer_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_stmt_defer_t* node)
{
  tau_ast_node_typecheck(ctx, node->stmt);
}

void tau_ast_stmt_defer_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_defer_t* node)
{
  tau_ctrlflow_ctx_defer_begin(ctx, node);

  tau_ast_node_ctrlflow(ctx, node->stmt);

  tau_ctrlflow_ctx_defer_end(ctx);
}

void tau_ast_stmt_defer_codegen(tau_codegen_ctx_t* TAU_UNUSED(ctx), tau_ast_stmt_defer_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
  // TODO
}

void tau_ast_stmt_defer_dump_json(FILE* stream, tau_ast_stmt_defer_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"stmt\":");
  tau_ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
