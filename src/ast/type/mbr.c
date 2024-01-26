/**
 * \file mbr.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/mbr.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_type_mbr_t* ast_type_mbr_init(void)
{
  ast_type_mbr_t* node = (ast_type_mbr_t*)malloc(sizeof(ast_type_mbr_t));
  clearobj(node);

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

  ast_node_t* parent_decl = NULL;
  
  switch (node->parent->kind)
  {
  case AST_TYPE_ID:     parent_decl = ((ast_type_id_t*)node->parent)->decl; break;
  case AST_TYPE_MEMBER: parent_decl = ((ast_type_mbr_t*)node->parent)->decl; break;
  default: unreachable();
  }

  assert(parent_decl->kind == AST_DECL_MOD);
  ast_decl_mod_t* mod_decl = (ast_decl_mod_t*)parent_decl;

  assert(node->member->kind == AST_TYPE_ID);
  string_view_t id_view = token_to_string_view(node->member->tok);
  symbol_t* mbr_sym = symtable_get_with_str_view(mod_decl->scope, id_view);

  if (mbr_sym == NULL)
    report_error_no_member_with_name(node->member->tok->loc);

  ((ast_type_id_t*)node->member)->decl = mbr_sym->node;
  node->decl = mbr_sym->node;
}

void ast_type_mbr_typecheck(typecheck_ctx_t* ctx, ast_type_mbr_t* node)
{
  ast_node_typecheck(ctx, node->parent);
  ast_node_typecheck(ctx, node->member);

  typedesc_t* desc = typetable_lookup(ctx->typetable, node->decl);
  assert(desc != NULL);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_mbr_codegen(codegen_ctx_t* ctx, ast_type_mbr_t* node)
{
  ast_node_codegen(ctx, node->parent);
  ast_node_codegen(ctx, node->member);

  if (node->decl->kind != AST_DECL_MOD)
  {
    typedesc_t* desc = typetable_lookup(ctx->typetable, node->decl);
    assert(desc != NULL);

    node->llvm_type = desc->llvm_type;
  }
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
