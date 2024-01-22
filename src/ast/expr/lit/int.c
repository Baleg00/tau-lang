/**
 * \file int.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/int.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_expr_lit_int_t* ast_expr_lit_int_init(void)
{
  ast_expr_lit_int_t* node = (ast_expr_lit_int_t*)malloc(sizeof(ast_expr_lit_int_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_LIT_INT;

  return node;
}

void ast_expr_lit_int_free(ast_expr_lit_int_t* node)
{
  free(node);
}

void ast_expr_lit_int_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_int_t* node)
{
  
}

void ast_expr_lit_int_dump_json(FILE* stream, ast_expr_lit_int_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"value\":%llu", node->value);
  fputc('}', stream);
}
