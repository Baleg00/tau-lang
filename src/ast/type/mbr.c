/**
 * \file mbr.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/mbr.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_type_mbr_t* ast_type_mbr_init(void)
{
  ast_type_mbr_t* node = (ast_type_mbr_t*)malloc(sizeof(ast_type_mbr_t));
  assert(node != NULL);

  node->kind = AST_TYPE_MEMBER;

  return node;
}

void ast_type_mbr_free(ast_type_mbr_t* node)
{
  free(node);
}

void ast_type_mbr_dump_json(FILE* stream, ast_type_mbr_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"parent\":");
  ast_node_dump_json(stream, node->parent);
  fprintf(stream, ",\"member\":");
  ast_node_dump_json(stream, node->member);
  fputc('}', stream);
}
