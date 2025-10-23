/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/type_type.h"

#include "ast/registry.h"

tau_ast_type_type_t* tau_ast_type_type_init(void)
{
  tau_ast_type_type_t* node = (tau_ast_type_type_t*)malloc(sizeof(tau_ast_type_type_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_TYPE_TYPE;

  return node;
}

void tau_ast_type_type_free(tau_ast_type_type_t* node)
{
  free(node);
}

void tau_ast_type_type_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_type_type_t* TAU_UNUSED(node))
{
}

void tau_ast_type_type_typecheck(tau_typecheck_ctx_t* TAU_UNUSED(ctx), tau_ast_type_type_t* TAU_UNUSED(node))
{
}

void tau_ast_type_type_codegen(tau_codegen_ctx_t* TAU_UNUSED(ctx), tau_ast_type_type_t* TAU_UNUSED(node))
{
}

void tau_ast_type_type_dump_json(FILE* stream, tau_ast_type_type_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", tau_ast_kind_to_cstr(node->kind));
}
