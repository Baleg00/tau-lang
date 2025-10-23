/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/null.h"

#include "ast/registry.h"

tau_ast_expr_lit_null_t* tau_ast_expr_lit_null_init(void)
{
  tau_ast_expr_lit_null_t* node = (tau_ast_expr_lit_null_t*)malloc(sizeof(tau_ast_expr_lit_null_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_LIT_NULL;

  return node;
}

void tau_ast_expr_lit_null_free(tau_ast_expr_lit_null_t* node)
{
  free(node);
}

void tau_ast_expr_lit_null_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_lit_null_t* TAU_UNUSED(node))
{
}

void tau_ast_expr_lit_null_typecheck(tau_typecheck_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_lit_null_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
  // TODO
}

void tau_ast_expr_lit_null_codegen(tau_codegen_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_lit_null_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
  // TODO
}

void tau_ast_expr_lit_null_dump_json(FILE* stream, tau_ast_expr_lit_null_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", tau_ast_kind_to_cstr(node->kind));
}
