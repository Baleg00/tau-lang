/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/mbr.h"

#include "ast/ast.h"
#include "ast/registry.h"

ast_type_mbr_t* ast_type_mbr_init(void)
{
  ast_type_mbr_t* node = (ast_type_mbr_t*)malloc(sizeof(ast_type_mbr_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_MEMBER;

  return node;
}

void ast_type_mbr_free(ast_type_mbr_t* node)
{
  free(node);
}

void ast_type_mbr_nameres(nameres_ctx_t* ctx, ast_type_mbr_t* node)
{
  ast_node_nameres(ctx, node->parent);

  ast_node_t* parent_node = NULL;

  switch (node->parent->kind)
  {
  case AST_TYPE_ID:     parent_node = ((ast_type_id_t* )node->parent)->decl; break;
  case AST_TYPE_MEMBER: parent_node = ((ast_type_mbr_t*)node->parent)->decl; break;
  default: UNREACHABLE();
  }

  ASSERT(parent_node->kind == AST_DECL_MOD); // Only modules have member types.
  ast_decl_mod_t* mod_node = (ast_decl_mod_t*)parent_node;

  ast_type_id_t* member_node = (ast_type_id_t*)node->member;

  string_view_t id_view = token_to_string_view(member_node->tok);
  symbol_t* mbr_sym = symtable_get_with_str_view(mod_node->scope, id_view);

  if (mbr_sym == NULL)
  {
    error_bag_put_nameres_no_member(ctx->errors, token_location(node->tok));
    return;
  }

  if (!((ast_decl_t*)mbr_sym->node)->is_pub)
  {
    error_bag_put_nameres_private_member(ctx->errors, token_location(node->member->tok));
    return;
  }

  member_node->decl = mbr_sym->node;
  node->decl = mbr_sym->node;
}

void ast_type_mbr_typecheck(typecheck_ctx_t* ctx, ast_type_mbr_t* node)
{
  ast_node_typecheck(ctx, node->parent);
  ast_node_typecheck(ctx, node->member);

  if (node->decl->kind == AST_DECL_MOD)
    return;

  typedesc_t* desc = typetable_lookup(ctx->typetable, node->decl);
  ASSERT(desc != NULL);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_mbr_codegen(codegen_ctx_t* ctx, ast_type_mbr_t* node)
{
  ast_node_codegen(ctx, node->parent);
  ast_node_codegen(ctx, node->member);

  if (node->decl->kind != AST_DECL_MOD)
  {
    typedesc_t* desc = typetable_lookup(ctx->typetable, node->decl);
    ASSERT(desc != NULL);

    node->llvm_type = desc->llvm_type;
  }
}

size_t ast_type_mbr_mangle(ast_type_mbr_t* node, char* buf, size_t len)
{
  size_t written = 0;

  switch (node->decl->kind)
  {
  case AST_DECL_STRUCT: written += snprintf(buf, len, "S"); break;
  case AST_DECL_UNION:  written += snprintf(buf, len, "U"); break;
  case AST_DECL_ENUM:   written += snprintf(buf, len, "E"); break;
  default: UNREACHABLE();
  }

  written += ast_node_mangle_nested_name(node->decl, buf + written, len <= written ? 0 : len - written);

  return written;
}

void ast_type_mbr_dump_json(FILE* stream, ast_type_mbr_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"parent\":");
  ast_node_dump_json(stream, node->parent);
  fprintf(stream, ",\"member\":");
  ast_node_dump_json(stream, node->member);
  fputc('}', stream);
}
