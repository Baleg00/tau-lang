/**
 * \file flt.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/flt.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_expr_lit_flt_t* ast_expr_lit_flt_init(void)
{
  ast_expr_lit_flt_t* node = (ast_expr_lit_flt_t*)malloc(sizeof(ast_expr_lit_flt_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_LIT_FLT;

  return node;
}

void ast_expr_lit_flt_free(ast_expr_lit_flt_t* node)
{
  free(node);
}

void ast_expr_lit_flt_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_flt_t* node)
{
  
}

void ast_expr_lit_flt_dump_json(FILE* stream, ast_expr_lit_flt_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"value\":%Lf", node->value);
  fputc('}', stream);
}
