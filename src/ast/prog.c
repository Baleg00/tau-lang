/**
 * \file prog.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/prog.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_prog_t* ast_prog_init(void)
{
  ast_prog_t* node = (ast_prog_t*)malloc(sizeof(ast_prog_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_PROG;

  return node;
}

void ast_prog_free(ast_prog_t* node)
{
  vector_free(node->decls);
  free(node);
}

void ast_prog_nameres(nameres_ctx_t* ctx, ast_prog_t* node)
{
  VECTOR_FOR_LOOP(i, node->decls)
  {
    ast_node_t* decl = (ast_node_t*)vector_get(node->decls, i);
    ast_node_nameres(ctx, &decl);
    vector_set(node->decls, i, decl);
  }
}

void ast_prog_dump_json(FILE* stream, ast_prog_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"decls\":");
  ast_node_dump_json_vector(stream, node->decls);
  fputc('}', stream);
}
