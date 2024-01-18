/**
 * \file fun.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/fun.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_type_fun_t* ast_type_fun_init(void)
{
  ast_type_fun_t* node = (ast_type_fun_t*)malloc(sizeof(ast_type_fun_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_FUN;

  return node;
}

void ast_type_fun_free(ast_type_fun_t* node)
{
  list_free(node->params);
  free(node);
}

void ast_type_fun_dump_json(FILE* stream, ast_type_fun_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"params\":");
  ast_list_dump_json(stream, node->params);
  fprintf(stream, ",\"return_type\":");
  ast_node_dump_json(stream, node->return_type);
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fprintf(stream, ",\"callconv\":\"%s\"", callconv_kind_to_cstr(node->callconv));
  fputc('}', stream);
}
