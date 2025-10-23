/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/mbr.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_type_mbr_t* tau_ast_type_mbr_init(void)
{
  tau_ast_type_mbr_t* node = (tau_ast_type_mbr_t*)malloc(sizeof(tau_ast_type_mbr_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_TYPE_MEMBER;

  return node;
}

void tau_ast_type_mbr_free(tau_ast_type_mbr_t* node)
{
  free(node);
}

void tau_ast_type_mbr_nameres(tau_nameres_ctx_t* ctx, tau_ast_type_mbr_t* node)
{
  tau_ast_node_nameres(ctx, node->parent);

  tau_ast_node_t* parent_node = NULL;

  switch (node->parent->kind)
  {
  case TAU_AST_TYPE_ID:     parent_node = ((tau_ast_type_id_t* )node->parent)->decl; break;
  case TAU_AST_TYPE_MEMBER: parent_node = ((tau_ast_type_mbr_t*)node->parent)->decl; break;
  default: TAU_UNREACHABLE();
  }

  TAU_ASSERT(parent_node->kind == TAU_AST_DECL_MOD); // Only modules have member types.
  tau_ast_decl_mod_t* mod_node = (tau_ast_decl_mod_t*)parent_node;

  tau_ast_type_id_t* member_node = (tau_ast_type_id_t*)node->member;

  tau_string_view_t id_view = tau_token_to_string_view(member_node->tok);
  tau_symbol_t* mbr_sym = tau_symtable_get_with_str_view(mod_node->scope, id_view);

  if (mbr_sym == NULL)
  {
    tau_error_bag_put_nameres_no_member(ctx->errors, tau_token_location(node->tok));
    return;
  }

  if (!((tau_ast_decl_t*)mbr_sym->node)->is_pub)
  {
    tau_error_bag_put_nameres_private_member(ctx->errors, tau_token_location(node->member->tok));
    return;
  }

  member_node->decl = mbr_sym->node;
  node->decl = mbr_sym->node;
}

void tau_ast_type_mbr_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_mbr_t* node)
{
  tau_ast_node_typecheck(ctx, node->parent);
  tau_ast_node_typecheck(ctx, node->member);

  if (node->decl->kind == TAU_AST_DECL_MOD)
    return;

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, node->decl);
  TAU_ASSERT(desc != NULL);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_type_mbr_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_mbr_t* node)
{
  tau_ast_node_codegen(ctx, node->parent);
  tau_ast_node_codegen(ctx, node->member);

  if (node->decl->kind != TAU_AST_DECL_MOD)
  {
    tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, node->decl);
    TAU_ASSERT(desc != NULL);

    node->llvm_type = desc->llvm_type;
  }
}

void tau_ast_type_mbr_dump_json(FILE* stream, tau_ast_type_mbr_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"parent\":");
  tau_ast_node_dump_json(stream, node->parent);
  fprintf(stream, ",\"member\":");
  tau_ast_node_dump_json(stream, node->member);
  fputc('}', stream);
}
