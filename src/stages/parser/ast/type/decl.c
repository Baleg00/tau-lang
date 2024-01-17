/**
 * \file decl.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/type/decl.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_type_decl_t* ast_type_decl_init(void)
{
  ast_type_decl_t* node = (ast_type_decl_t*)malloc(sizeof(ast_type_decl_t));
  assert(node != NULL);

  node->kind = AST_TYPE_DECL;

  return node;
}

void ast_type_decl_dump_json(FILE* stream, ast_type_decl_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
