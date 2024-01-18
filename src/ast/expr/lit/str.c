/**
 * \file str.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/str.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_expr_lit_str_t* ast_expr_lit_str_init(void)
{
  ast_expr_lit_str_t* node = (ast_expr_lit_str_t*)malloc(sizeof(ast_expr_lit_str_t));
  assert(node != NULL);

  node->kind = AST_EXPR_LIT_STR;

  return node;
}

void ast_expr_lit_str_free(ast_expr_lit_str_t* node)
{
  free(node->value);
  free(node);
}

void ast_expr_lit_str_dump_json(FILE* stream, ast_expr_lit_str_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"value\":\"%s\"", node->value);
  fputc('}', stream);
}