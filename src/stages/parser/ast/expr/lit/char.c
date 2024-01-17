/**
 * \file char.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/expr/lit/char.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_expr_lit_char_t* ast_expr_lit_char_init(void)
{
  ast_expr_lit_char_t* node = (ast_expr_lit_char_t*)malloc(sizeof(ast_expr_lit_char_t));
  assert(node != NULL);

  node->kind = AST_EXPR_LIT_CHAR;

  return node;
}

void ast_expr_lit_char_dump_json(FILE* stream, ast_expr_lit_char_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"value\":%u", node->value);
  fputc('}', stream);
}
