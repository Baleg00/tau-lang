/**
 * \file continue.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/stmt/continue.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_stmt_continue_t* ast_stmt_continue_init(void)
{
  ast_stmt_continue_t* node = (ast_stmt_continue_t*)malloc(sizeof(ast_stmt_continue_t));
  assert(node != NULL);

  node->kind = AST_STMT_CONTINUE;

  return node;
}

void ast_stmt_continue_dump_json(FILE* stream, ast_stmt_continue_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
