/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/expr.h"

#include "ast/registry.h"

tau_ast_stmt_expr_t* tau_ast_stmt_expr_init(void)
{
  tau_ast_stmt_expr_t* node = (tau_ast_stmt_expr_t*)malloc(sizeof(tau_ast_stmt_expr_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_STMT_EXPR;

  return node;
}

void tau_ast_stmt_expr_free(tau_ast_stmt_expr_t* node)
{
  free(node);
}

void tau_ast_stmt_expr_nameres(tau_nameres_ctx_t* ctx, tau_ast_stmt_expr_t* node)
{
  tau_ast_node_nameres(ctx, node->expr);
}

void tau_ast_stmt_expr_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_stmt_expr_t* node)
{
  tau_ast_node_typecheck(ctx, node->expr);
}

void tau_ast_stmt_expr_codegen(tau_codegen_ctx_t* ctx, tau_ast_stmt_expr_t* node)
{
  tau_ast_node_codegen(ctx, node->expr);
}

void tau_ast_stmt_expr_dump_json(FILE* stream, tau_ast_stmt_expr_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"expr\":");
  tau_ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
