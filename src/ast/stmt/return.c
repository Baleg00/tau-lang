/**
 * \file return.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/return.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_stmt_return_t* ast_stmt_return_init(void)
{
  ast_stmt_return_t* node = (ast_stmt_return_t*)malloc(sizeof(ast_stmt_return_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_RETURN;

  return node;
}

void ast_stmt_return_free(ast_stmt_return_t* node)
{
  free(node);
}

void ast_stmt_return_nameres(nameres_ctx_t* ctx, ast_stmt_return_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_stmt_return_dump_json(FILE* stream, ast_stmt_return_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
