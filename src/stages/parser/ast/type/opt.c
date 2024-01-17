/**
 * \file opt.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/type/opt.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_type_opt_t* ast_type_opt_init(void)
{
  ast_type_opt_t* node = (ast_type_opt_t*)malloc(sizeof(ast_type_opt_t));
  assert(node != NULL);

  node->kind = AST_TYPE_OPT;

  return node;
}

void ast_type_opt_dump_json(FILE* stream, ast_type_opt_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"base_type\":");
  ast_node_dump_json(stream, node->base_type);
  fputc('}', stream);
}
