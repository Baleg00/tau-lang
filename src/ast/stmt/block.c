/**
 * \file block.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/block.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_stmt_block_t* ast_stmt_block_init(void)
{
  ast_stmt_block_t* node = (ast_stmt_block_t*)malloc(sizeof(ast_stmt_block_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_BLOCK;

  return node;
}

void ast_stmt_block_free(ast_stmt_block_t* node)
{
  vector_free(node->stmts);
  free(node);
}

void ast_stmt_block_nameres(nameres_ctx_t* ctx, ast_stmt_block_t* node)
{
  VECTOR_FOR_LOOP(i, node->stmts)
  {
    ast_node_t* stmt = (ast_node_t*)vector_get(node->stmts, i);
    ast_node_nameres(ctx, &stmt);
    vector_set(node->stmts, i, stmt);
  }
}

void ast_stmt_block_dump_json(FILE* stream, ast_stmt_block_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"stmts\":");
  ast_node_dump_json_vector(stream, node->stmts);
  fputc('}', stream);
}
