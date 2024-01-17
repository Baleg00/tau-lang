/**
 * \file union.h
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/decl/union.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_decl_union_t* ast_decl_union_init(void)
{
  ast_decl_union_t* node = (ast_decl_union_t*)malloc(sizeof(ast_decl_union_t));
  assert(node != NULL);

  node->kind = AST_DECL_UNION;

  return node;
}

void ast_decl_union_dump_json(FILE* stream, ast_decl_union_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  ast_list_dump_json(stream, node->members);
  fputc('}', stream);
}
