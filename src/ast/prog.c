/**
 * \file prog.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/prog.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_prog_t* ast_prog_init(void)
{
  ast_prog_t* node = (ast_prog_t*)malloc(sizeof(ast_prog_t));
  assert(node != NULL);

  node->kind = AST_PROG;

  return node;
}

void ast_prog_dump_json(FILE* stream, ast_prog_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"decls\":");
  ast_list_dump_json(stream, node->decls);
  fputc('}', stream);
}
