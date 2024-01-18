/**
 * \file decl.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/decl.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_expr_decl_t* ast_expr_decl_init(void)
{
  ast_expr_decl_t* node = (ast_expr_decl_t*)malloc(sizeof(ast_expr_decl_t));
  assert(node != NULL);

  node->kind = AST_EXPR_DECL;

  return node;
}

void ast_expr_decl_free(ast_expr_decl_t* node)
{
  free(node);
}

void ast_expr_decl_dump_json(FILE* stream, ast_expr_decl_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"decl\":");
  ast_node_dump_json(stream, node->decl);
  fputc('}', stream);
}
