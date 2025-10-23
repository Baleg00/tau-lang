/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/use.h"

#include "ast/registry.h"

tau_ast_use_t* tau_ast_use_init(void)
{
  tau_ast_use_t* node = (tau_ast_use_t*)malloc(sizeof(tau_ast_use_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_USE;

  return node;
}

void tau_ast_use_free(tau_ast_use_t* node)
{
  free(node);
}

void tau_ast_use_dump_json(FILE* stream, tau_ast_use_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"path\":");
  tau_ast_node_dump_json(stream, node->path);
  fputc('}', stream);
}
