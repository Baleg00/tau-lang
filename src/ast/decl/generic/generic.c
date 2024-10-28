/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/generic/generic.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_decl_generic_t* ast_decl_generic_init(void)
{
  ast_decl_generic_t* node = (ast_decl_generic_t*)malloc(sizeof(ast_decl_generic_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_GENERIC;

  return node;
}

void ast_decl_generic_free(ast_decl_generic_t* node)
{
  free(node);
}

void ast_decl_generic_nameres(nameres_ctx_t* ctx, ast_decl_generic_t* node)
{
  ast_node_nameres(ctx, node->node);

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_t* param = (ast_node_t*)vector_get(node->params, i);

    ast_node_nameres(ctx, param);
  }
}

void ast_decl_generic_typecheck(typecheck_ctx_t* ctx, ast_decl_generic_t* node)
{
  ast_node_typecheck(ctx, node->node);

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_t* param = (ast_node_t*)vector_get(node->params, i);

    ast_node_typecheck(ctx, param);
  }
}

void ast_decl_generic_ctrlflow(ctrlflow_ctx_t* ctx, ast_decl_generic_t* node)
{
  ast_node_ctrlflow(ctx, node->node);

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_t* param = (ast_node_t*)vector_get(node->params, i);

    ast_node_ctrlflow(ctx, param);
  }
}

void ast_decl_generic_codegen(codegen_ctx_t* ctx, ast_decl_generic_t* node)
{
  ast_node_codegen(ctx, node->node);

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_t* param = (ast_node_t*)vector_get(node->params, i);

    ast_node_codegen(ctx, param);
  }
}

void ast_decl_generic_dump_json(FILE* stream, ast_decl_generic_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"node\":", ast_kind_to_cstr(node->kind));
  ast_node_dump_json(stream, node->node);
  fprintf(stream, ",\"params\":");
  ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, "}");
}
