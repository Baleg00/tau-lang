/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/for.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_stmt_for_t* ast_stmt_for_init(void)
{
  ast_stmt_for_t* node = (ast_stmt_for_t*)malloc(sizeof(ast_stmt_for_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_FOR;

  return node;
}

void ast_stmt_for_free(ast_stmt_for_t* node)
{
  free(node);
}

void ast_stmt_for_nameres(nameres_ctx_t* ctx, ast_stmt_for_t* node)
{
  unreachable();
  // TODO
}

void ast_stmt_for_typecheck(typecheck_ctx_t* ctx, ast_stmt_for_t* node)
{
  unreachable();
  // TODO
}

void ast_stmt_for_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_for_t* node)
{
  unreachable();
  // TODO
}

void ast_stmt_for_codegen(codegen_ctx_t* ctx, ast_stmt_for_t* node)
{
  unreachable();
  // TODO
}

void ast_stmt_for_dump_json(FILE* stream, ast_stmt_for_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"var\":");
  ast_node_dump_json(stream, node->var);
  fprintf(stream, ",\"range\":");
  ast_node_dump_json(stream, node->range);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
