/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/generic/fun.h"

#include "ast/registry.h"

tau_ast_decl_generic_fun_t* tau_ast_decl_generic_fun_init(void)
{
  tau_ast_decl_generic_fun_t* node = (tau_ast_decl_generic_fun_t*)malloc(sizeof(tau_ast_decl_generic_fun_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_GENERIC_FUN;
  node->generic_params = tau_vector_init();
  node->params = tau_vector_init();

  return node;
}

void tau_ast_decl_generic_fun_free(tau_ast_decl_generic_fun_t* node)
{
  tau_vector_free(node->generic_params);
  tau_vector_free(node->params);
  free(node);
}

void tau_ast_decl_generic_fun_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_generic_fun_t* node)
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

  TAU_VECTOR_FOR_LOOP(i, node->generic_params)
  {
    tau_ast_node_t* generic_param = (tau_ast_node_t*)tau_vector_get(node->generic_params, i);

    tau_ast_node_nameres(ctx, generic_param);
  }

  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_t* param = (tau_ast_node_t*)tau_vector_get(node->params, i);

    tau_ast_node_nameres(ctx, param);
  }

  tau_ast_node_nameres(ctx, node->return_type);
  tau_ast_node_nameres(ctx, node->stmt);

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_decl_generic_fun_dump_json(FILE* stream, tau_ast_decl_generic_fun_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"generic_params\":");
  tau_ast_node_dump_json_vector(stream, node->generic_params);
  fprintf(stream, ",\"params\":");
  tau_ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, ",\"return_type\":");
  tau_ast_node_dump_json(stream, node->return_type);
  fprintf(stream, ",\"stmt\":");
  tau_ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
