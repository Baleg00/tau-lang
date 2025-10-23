/**
* \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/generic/param.h"

#include "ast/registry.h"

tau_ast_decl_generic_param_t* tau_ast_decl_generic_param_init(void)
{
  tau_ast_decl_generic_param_t* node = (tau_ast_decl_generic_param_t*)malloc(sizeof(tau_ast_decl_generic_param_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_GENERIC_PARAM;

  return node;
}

void tau_ast_decl_generic_param_free(tau_ast_decl_generic_param_t* node)
{
  free(node);
}

void tau_ast_decl_generic_param_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_generic_param_t* node)
{
  tau_ast_node_nameres(ctx, node->type);

  if (node->expr != NULL)
  {
    tau_ast_node_nameres(ctx, node->expr);
  }

  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->id->tok);
  tau_symbol_t* sym = tau_symbol_init_with_str_view(id_view, (tau_ast_node_t*)node);

  tau_symbol_t* collision = tau_symtable_insert(scope, sym);

  if (collision != NULL && collision->node->kind == TAU_AST_DECL_GENERIC_PARAM)
  {
    tau_error_bag_put_nameres_symbol_collision(ctx->errors, tau_token_location(node->tok), tau_token_location(collision->node->tok));
  }
}

void tau_ast_decl_generic_param_dump_json(FILE* stream, tau_ast_decl_generic_param_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"type\":", tau_ast_kind_to_cstr(node->kind));
  tau_ast_node_dump_json(stream, node->type);
  fprintf(stream, ",\"expr\":");
  tau_ast_node_dump_json(stream, node->expr);
  fprintf(stream, "}");
}
