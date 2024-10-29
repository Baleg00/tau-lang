/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/generic/spec.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_expr_generic_spec_t* ast_expr_generic_spec_init(void)
{
  ast_expr_generic_spec_t* node = (ast_expr_generic_spec_t*)malloc(sizeof(ast_expr_generic_spec_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_GENERIC_SPEC;
  node->params = vector_init();

  return node;
}

void ast_expr_generic_spec_free(ast_expr_generic_spec_t* node)
{
  vector_free(node->params);
  free(node);
}

void ast_expr_generic_spec_nameres(nameres_ctx_t* UNUSED(ctx), ast_expr_generic_spec_t* UNUSED(node))
{
  UNREACHABLE();
}

void ast_expr_generic_spec_typecheck(typecheck_ctx_t* UNUSED(ctx), ast_expr_generic_spec_t* UNUSED(node))
{
  UNREACHABLE();
}

void ast_expr_generic_spec_codegen(codegen_ctx_t* UNUSED(ctx), ast_expr_generic_spec_t* UNUSED(node))
{
  UNREACHABLE();
}

void ast_expr_generic_spec_dump_json(FILE* stream, ast_expr_generic_spec_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"generic\":", ast_kind_to_cstr(node->kind));
  ast_node_dump_json(stream, node->generic);
  fprintf(stream, ",\"params\":");
  ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, "}");
}
