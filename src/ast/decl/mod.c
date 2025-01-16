/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/mod.h"

#include "ast/registry.h"
#include "utils/diagnostics.h"

ast_decl_mod_t* ast_decl_mod_init(void)
{
  ast_decl_mod_t* node = (ast_decl_mod_t*)malloc(sizeof(ast_decl_mod_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_MOD;
  node->members = vector_init();

  return node;
}

void ast_decl_mod_free(ast_decl_mod_t* node)
{
  vector_free(node->members);
  free(node);
}

void ast_decl_mod_nameres(nameres_ctx_t* ctx, ast_decl_mod_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);

  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL)
  {
    location_t loc = token_location(node->tok);

    report_error_symbol_redeclaration(loc);
  }

  node->scope = nameres_ctx_scope_begin(ctx);

  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_node_nameres(ctx, (ast_node_t*)vector_get(node->members, i));
  }

  nameres_ctx_scope_end(ctx);
}

void ast_decl_mod_typecheck(typecheck_ctx_t* ctx, ast_decl_mod_t* node)
{
  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_node_typecheck(ctx, (ast_node_t*)vector_get(node->members, i));
  }
}

void ast_decl_mod_ctrlflow(ctrlflow_ctx_t* ctx, ast_decl_mod_t* node)
{
  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_node_ctrlflow(ctx, (ast_node_t*)vector_get(node->members, i));
  }
}

void ast_decl_mod_codegen(codegen_ctx_t* ctx, ast_decl_mod_t* node)
{
  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_node_codegen(ctx, (ast_node_t*)vector_get(node->members, i));
  }
}

void ast_decl_mod_dump_json(FILE* stream, ast_decl_mod_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  ast_node_dump_json_vector(stream, node->members);
  fputc('}', stream);
}
