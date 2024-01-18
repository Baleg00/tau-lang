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
  list_free(node->stmts);
  free(node);
}

void ast_stmt_block_dump_json(FILE* stream, ast_stmt_block_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"stmts\":");
  ast_list_dump_json(stream, node->stmts);
  fputc('}', stream);
}
