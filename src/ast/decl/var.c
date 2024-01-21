/**
 * \file var.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/var.h"

#include "ast/registry.h"
#include "stages/analysis/symtable.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_decl_var_t* ast_decl_var_init(void)
{
  ast_decl_var_t* node = (ast_decl_var_t*)malloc(sizeof(ast_decl_var_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_VAR;

  return node;
}

void ast_decl_var_free(ast_decl_var_t* node)
{
  free(node);
}

void ast_decl_var_nameres(nameres_ctx_t* ctx, ast_decl_var_t* node)
{
  ast_node_nameres(ctx, node->type);
  ast_node_nameres(ctx, node->expr);

  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);

  symbol_t* lookup = symtable_lookup_with_str_view(scope, id_view);

  if (lookup != NULL && (lookup->node->kind == AST_DECL_VAR || lookup->node->kind == AST_DECL_PARAM))
    report_warning_shadowed_variable(node->id->tok->loc);

  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redeclaration(node->id->tok->loc, collision->node->tok->loc);
}

void ast_decl_var_dump_json(FILE* stream, ast_decl_var_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"type\":");
  ast_node_dump_json(stream, node->type);
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
