/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/mod.h"

#include "ast/registry.h"

tau_ast_decl_mod_t* tau_ast_decl_mod_init(void)
{
  tau_ast_decl_mod_t* node = (tau_ast_decl_mod_t*)malloc(sizeof(tau_ast_decl_mod_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_MOD;
  node->members = tau_vector_init();

  return node;
}

void tau_ast_decl_mod_free(tau_ast_decl_mod_t* node)
{
  tau_vector_free(node->members);
  free(node);
}

void tau_ast_decl_mod_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_mod_t* node)
{
  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->id->tok);
  tau_symbol_t* sym = tau_symbol_init_with_str_view(id_view, (tau_ast_node_t*)node);

  tau_symbol_t* collision = tau_symtable_insert(scope, sym);

  if (collision != NULL)
  {
    tau_error_bag_put_nameres_symbol_collision(ctx->errors, tau_token_location(node->tok), tau_token_location(collision->node->tok));
    return;
  }

  node->scope = tau_nameres_ctx_scope_begin(ctx);

  TAU_VECTOR_FOR_LOOP(i, node->members)
  {
    tau_ast_node_nameres(ctx, (tau_ast_node_t*)tau_vector_get(node->members, i));
  }

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_decl_mod_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_mod_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->members)
  {
    tau_ast_node_typecheck(ctx, (tau_ast_node_t*)tau_vector_get(node->members, i));
  }
}

void tau_ast_decl_mod_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_decl_mod_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->members)
  {
    tau_ast_node_ctrlflow(ctx, (tau_ast_node_t*)tau_vector_get(node->members, i));
  }
}

void tau_ast_decl_mod_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_mod_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->members)
  {
    tau_ast_node_codegen(ctx, (tau_ast_node_t*)tau_vector_get(node->members, i));
  }
}

void tau_ast_decl_mod_dump_json(FILE* stream, tau_ast_decl_mod_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  tau_ast_node_dump_json_vector(stream, node->members);
  fputc('}', stream);
}
