/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/prog.h"

#include "ast/registry.h"

tau_ast_prog_t* tau_ast_prog_init(void)
{
  tau_ast_prog_t* node = (tau_ast_prog_t*)malloc(sizeof(tau_ast_prog_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_PROG;
  node->decls = tau_vector_init();

  return node;
}

void tau_ast_prog_free(tau_ast_prog_t* node)
{
  tau_vector_free(node->decls);
  free(node);
}

void tau_ast_prog_nameres(tau_nameres_ctx_t* ctx, tau_ast_prog_t* node)
{
  node->scope = tau_nameres_ctx_scope_begin(ctx);

  TAU_VECTOR_FOR_LOOP(i, node->decls)
    tau_ast_node_nameres(ctx, (tau_ast_node_t*)tau_vector_get(node->decls, i));

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_prog_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_prog_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->decls)
    tau_ast_node_typecheck(ctx, (tau_ast_node_t*)tau_vector_get(node->decls, i));
}

void tau_ast_prog_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_prog_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->decls)
    tau_ast_node_ctrlflow(ctx, (tau_ast_node_t*)tau_vector_get(node->decls, i));
}

void tau_ast_prog_codegen(tau_codegen_ctx_t* ctx, tau_ast_prog_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->decls)
    tau_ast_node_codegen(ctx, (tau_ast_node_t*)tau_vector_get(node->decls, i));
}

void tau_ast_prog_dump_json(FILE* stream, tau_ast_prog_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"decls\":");
  tau_ast_node_dump_json_vector(stream, node->decls);
  fputc('}', stream);
}
