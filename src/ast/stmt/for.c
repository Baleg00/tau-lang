/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/for.h"

#include "ast/registry.h"

tau_ast_stmt_for_t* tau_ast_stmt_for_init(void)
{
  tau_ast_stmt_for_t* node = (tau_ast_stmt_for_t*)malloc(sizeof(tau_ast_stmt_for_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_STMT_FOR;

  return node;
}

void tau_ast_stmt_for_free(tau_ast_stmt_for_t* node)
{
  free(node);
}

void tau_ast_stmt_for_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_stmt_for_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
  // TODO
}

void tau_ast_stmt_for_typecheck(tau_typecheck_ctx_t* TAU_UNUSED(ctx), tau_ast_stmt_for_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
  // TODO
}

void tau_ast_stmt_for_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_for_t* node)
{
  tau_ctrlflow_ctx_for_begin(ctx, node);

  TAU_UNREACHABLE();
  // TODO

  tau_ctrlflow_ctx_for_end(ctx);
}

void tau_ast_stmt_for_codegen(tau_codegen_ctx_t* TAU_UNUSED(ctx), tau_ast_stmt_for_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
  // TODO
}

void tau_ast_stmt_for_dump_json(FILE* stream, tau_ast_stmt_for_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"var\":");
  tau_ast_node_dump_json(stream, node->var);
  fprintf(stream, ",\"range\":");
  tau_ast_node_dump_json(stream, node->range);
  fprintf(stream, ",\"stmt\":");
  tau_ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
