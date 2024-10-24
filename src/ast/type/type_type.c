/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/type_type.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_type_type_t* ast_type_type_init(void)
{
  ast_type_type_t* node = (ast_type_type_t*)malloc(sizeof(ast_type_type_t));
  CLEAROBJ(node);

  node->kind = AST_TYPE_TYPE;

  return node;
}

void ast_type_type_free(ast_type_type_t* node)
{
  free(node);
}

void ast_type_type_nameres(nameres_ctx_t* UNUSED(ctx), ast_type_type_t* UNUSED(node))
{
}

void ast_type_type_typecheck(typecheck_ctx_t* UNUSED(ctx), ast_type_type_t* UNUSED(node))
{
}

void ast_type_type_codegen(codegen_ctx_t* UNUSED(ctx), ast_type_type_t* UNUSED(node))
{
}

void ast_type_type_dump_json(FILE* stream, ast_type_type_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
