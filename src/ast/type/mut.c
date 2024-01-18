/**
 * \file mut.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/mut.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_type_mut_t* ast_type_mut_init(void)
{
  ast_type_mut_t* node = (ast_type_mut_t*)malloc(sizeof(ast_type_mut_t));
  assert(node != NULL);

  node->kind = AST_TYPE_MUT;

  return node;
}

void ast_type_mut_dump_json(FILE* stream, ast_type_mut_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"base_type\":");
  ast_node_dump_json(stream, node->base_type);
  fputc('}', stream);
}
