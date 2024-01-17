/**
 * \file mod.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/decl/mod.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_decl_mod_t* ast_decl_mod_init(void)
{
  ast_decl_mod_t* node = (ast_decl_mod_t*)malloc(sizeof(ast_decl_mod_t));
  assert(node != NULL);

  node->kind = AST_DECL_MOD;

  return node;
}

void ast_decl_mod_dump_json(FILE* stream, ast_decl_mod_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  ast_list_dump_json(stream, node->members);
  fputc('}', stream);
}
