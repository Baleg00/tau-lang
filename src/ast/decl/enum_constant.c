/**
 * \file enum_constant.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/enum_constant.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_decl_enum_constant_t* ast_decl_enum_constant_init(void)
{
  ast_decl_enum_constant_t* node = (ast_decl_enum_constant_t*)malloc(sizeof(ast_decl_enum_constant_t));
  assert(node != NULL);

  node->kind = AST_DECL_ENUM_CONSTANT;

  return node;
}

void ast_decl_enum_constant_dump_json(FILE* stream, ast_decl_enum_constant_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"type\":");
  ast_node_dump_json(stream, node->type);
  fputc('}', stream);
}
