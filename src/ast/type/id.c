/**
 * \file id.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/id.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_type_id_t* ast_type_id_init(void)
{
  ast_type_id_t* node = (ast_type_id_t*)malloc(sizeof(ast_type_id_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_ID;

  return node;
}

void ast_type_id_free(ast_type_id_t* node)
{
  free(node);
}

void ast_type_id_nameres(nameres_ctx_t* ctx, ast_type_id_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->tok);
  symbol_t* sym = symtable_lookup_with_str_view(scope, id_view);

  if (sym == NULL)
    report_error_undefined_typename(node->tok->loc);

  if (sym->node->kind != AST_DECL_STRUCT &&
      sym->node->kind != AST_DECL_UNION &&
      sym->node->kind != AST_DECL_ENUM)
    report_error_symbol_is_not_a_typename(node->tok->loc);
  
  node->decl = sym->node;
}

void ast_type_id_typecheck(typecheck_ctx_t* ctx, ast_type_id_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, node->decl);
  assert(desc != NULL);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_id_codegen(codegen_ctx_t* ctx, ast_type_id_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, node->decl);
  assert(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

size_t ast_type_id_mangle(ast_type_id_t* node, char* buf, size_t len)
{
  if (buf == NULL || len == 0)
  {
    buf = NULL;
    len = 0;
  }

  size_t written = 0;

  switch (node->decl->kind)
  {
  case AST_DECL_STRUCT: written += snprintf(buf, len, "S"); break;
  case AST_DECL_UNION:  written += snprintf(buf, len, "U"); break;
  case AST_DECL_ENUM:   written += snprintf(buf, len, "E"); break;
  default: unreachable();
  }

  written += ast_node_mangle(node->decl, buf == NULL ? NULL : buf + written, len <= written ? 0 : len - written);

  return written;
}

void ast_type_id_dump_json(FILE* stream, ast_type_id_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
