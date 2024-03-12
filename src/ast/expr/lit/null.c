/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/null.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_expr_lit_null_t* ast_expr_lit_null_init(void)
{
  ast_expr_lit_null_t* node = (ast_expr_lit_null_t*)malloc(sizeof(ast_expr_lit_null_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_LIT_NULL;

  return node;
}

void ast_expr_lit_null_free(ast_expr_lit_null_t* node)
{
  free(node);
}

void ast_expr_lit_null_nameres(nameres_ctx_t* ctx, ast_expr_lit_null_t* node)
{
  unused(ctx);
  unused(node);
}

void ast_expr_lit_null_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_null_t* node)
{
  unused(ctx);
  unused(node);

  unreachable();
  // TODO
}

void ast_expr_lit_null_codegen(codegen_ctx_t* ctx, ast_expr_lit_null_t* node)
{
  unused(ctx);
  unused(node);
  
  unreachable();
  // TODO
}

void ast_expr_lit_null_dump_json(FILE* stream, ast_expr_lit_null_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
