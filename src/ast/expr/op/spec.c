/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/spec.h"

#include "ast/registry.h"

tau_ast_expr_op_spec_t* tau_ast_expr_op_spec_init(void)
{
  tau_ast_expr_op_spec_t* node = (tau_ast_expr_op_spec_t*)malloc(sizeof(tau_ast_expr_op_spec_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_SPEC;
  node->op_kind = OP_SPEC;
  node->params = tau_vector_init();

  return node;
}

void tau_ast_expr_op_spec_free(tau_ast_expr_op_spec_t* node)
{
  tau_vector_free(node->params);
  free(node);
}

void tau_ast_expr_op_spec_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_op_spec_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
}

void tau_ast_expr_op_spec_typecheck(tau_typecheck_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_op_spec_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
}

void tau_ast_expr_op_spec_codegen(tau_codegen_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_op_spec_t* TAU_UNUSED(node))
{
  TAU_UNREACHABLE();
}

void tau_ast_expr_op_spec_dump_json(FILE* stream, tau_ast_expr_op_spec_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"generic\":");
  tau_ast_node_dump_json(stream, node->generic);
  fprintf(stream, ",\"params\":");
  tau_ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, "}");
}
