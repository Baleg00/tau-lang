/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/path/list.h"

#include "ast/registry.h"

tau_ast_path_list_t* tau_ast_path_list_init(void)
{
  tau_ast_path_list_t* node = (tau_ast_path_list_t*)malloc(sizeof(tau_ast_path_list_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_PATH_LIST;
  node->paths = tau_vector_init();

  return node;
}

void tau_ast_path_list_free(tau_ast_path_list_t* node)
{
  tau_vector_free(node->paths);
  free(node);
}

void tau_ast_path_list_dump_json(FILE* stream, tau_ast_path_list_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"paths\":");
  tau_ast_node_dump_json_vector(stream, node->paths);
  fputc('}', stream);
}
