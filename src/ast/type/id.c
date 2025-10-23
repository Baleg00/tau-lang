/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/id.h"

#include "ast/registry.h"

tau_ast_type_id_t* tau_ast_type_id_init(void)
{
  tau_ast_type_id_t* node = (tau_ast_type_id_t*)malloc(sizeof(tau_ast_type_id_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_TYPE_ID;

  return node;
}

void tau_ast_type_id_free(tau_ast_type_id_t* node)
{
  free(node);
}

void tau_ast_type_id_nameres(tau_nameres_ctx_t* ctx, tau_ast_type_id_t* node)
{
  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->tok);
  tau_symbol_t* sym = tau_symtable_lookup_with_str_view(scope, id_view);

  if (sym == NULL)
  {
    tau_error_bag_put_nameres_undefined_symbol(ctx->errors, tau_token_location(node->tok));
    return;
  }

  switch (sym->node->kind)
  {
  case TAU_AST_DECL_STRUCT:
  case TAU_AST_DECL_UNION:
  case TAU_AST_DECL_ENUM:
  case TAU_AST_DECL_MOD:
  case TAU_AST_DECL_TYPE_ALIAS: break;
  default:
  {
    tau_error_bag_put_nameres_expected_typename(ctx->errors, tau_token_location(node->tok));
    return;
  }
  }

  node->decl = sym->node;
}

void tau_ast_type_id_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_id_t* node)
{
  if (node->decl->kind == TAU_AST_DECL_MOD)
    return;

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, node->decl);
  TAU_ASSERT(desc != NULL);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_type_id_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_id_t* node)
{
  if (node->decl->kind == TAU_AST_DECL_MOD)
    return;

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, node->decl);
  TAU_ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void tau_ast_type_id_dump_json(FILE* stream, tau_ast_type_id_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", tau_ast_kind_to_cstr(node->kind));
}
