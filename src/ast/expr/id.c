/**
 * \file decl.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/id.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_id_t* ast_expr_id_init(void)
{
  ast_expr_id_t* node = (ast_expr_id_t*)malloc(sizeof(ast_expr_id_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_ID;

  return node;
}

void ast_expr_id_free(ast_expr_id_t* node)
{
  free(node);
}

void ast_expr_id_nameres(nameres_ctx_t* ctx, ast_expr_id_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->tok);
  symbol_t* sym = symtable_lookup_with_str_view(scope, id_view);
  
  if (sym == NULL)
    report_error_undefined_symbol(node->tok->loc);

  if (sym->node->kind != AST_DECL_VAR &&
      sym->node->kind != AST_DECL_PARAM &&
      sym->node->kind != AST_DECL_FUN)
    report_error_symbol_is_not_an_expression(node->tok->loc);
  
  node->decl = sym->node;
}

void ast_expr_id_typecheck(typecheck_ctx_t* ctx, ast_expr_id_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, node->decl);
  assert(desc != NULL);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_id_dump_json(FILE* stream, ast_expr_id_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
