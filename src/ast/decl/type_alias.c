/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/type_alias.h"

#include "ast/registry.h"

tau_ast_decl_type_alias_t* tau_ast_decl_type_alias_init(void)
{
  tau_ast_decl_type_alias_t* node = (tau_ast_decl_type_alias_t*)malloc(sizeof(tau_ast_decl_type_alias_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_TYPE_ALIAS;

  return node;
}

void tau_ast_decl_type_alias_free(tau_ast_decl_type_alias_t* node)
{
  free(node);
}

void tau_ast_decl_type_alias_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_type_alias_t* node)
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

  tau_ast_node_nameres(ctx, node->type);

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_decl_type_alias_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_type_alias_t* node)
{
  tau_ast_node_typecheck(ctx, node->type);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, node->type);
  TAU_ASSERT(desc != NULL);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_decl_type_alias_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_type_alias_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  TAU_ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void tau_ast_decl_type_alias_dump_json(FILE* stream, tau_ast_decl_type_alias_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"type\":");
  tau_ast_node_dump_json(stream, node->type);
  fputc('}', stream);
}
