/**
 * \file decl.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/decl.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_type_decl_t* ast_type_decl_init(void)
{
  ast_type_decl_t* node = (ast_type_decl_t*)malloc(sizeof(ast_type_decl_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_DECL;

  return node;
}

void ast_type_decl_free(ast_type_decl_t* node)
{
  free(node);
}

void ast_type_decl_dump_json(FILE* stream, ast_type_decl_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
