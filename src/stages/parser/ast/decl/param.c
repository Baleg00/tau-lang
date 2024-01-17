/**
 * \file param.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/decl/param.h"

#include "utils/memory/memtrace.h"
#include "utils/common.h"

ast_decl_param_t* ast_decl_param_init(void)
{
  ast_decl_param_t* node = (ast_decl_param_t*)malloc(sizeof(ast_decl_param_t));
  assert(node != NULL);

  node->kind = AST_DECL_PARAM;

  return node;
}

void ast_decl_param_dump_json(FILE* stream, ast_decl_param_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"type\":");
  ast_node_dump_json(stream, node->type);
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fputc('}', stream); 
}
