/**
 * \file fun.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/fun.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_decl_fun_t* ast_decl_fun_init(void)
{
  ast_decl_fun_t* node = (ast_decl_fun_t*)malloc(sizeof(ast_decl_fun_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_FUN;

  return node;
}

void ast_decl_fun_free(ast_decl_fun_t* node)
{
  vector_free(node->params);
  free(node);
}

void ast_decl_fun_dump_json(FILE* stream, ast_decl_fun_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"is_extern\":%s", node->is_extern ? "true" : "false");
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fprintf(stream, ",\"callconv\":\"%s\"", callconv_kind_to_cstr(node->callconv));
  fprintf(stream, ",\"params\":");
  ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, ",\"return_type\":");
  ast_node_dump_json(stream, node->return_type);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
