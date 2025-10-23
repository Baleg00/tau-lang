/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/path/segment.h"

#include "ast/registry.h"

tau_ast_path_segment_t* tau_ast_path_segment_init(void)
{
  tau_ast_path_segment_t* node = (tau_ast_path_segment_t*)malloc(sizeof(tau_ast_path_segment_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_PATH_SEGMENT;

  return node;
}

void tau_ast_path_segment_free(tau_ast_path_segment_t* node)
{
  free(node);
}

void tau_ast_path_segment_dump_json(FILE* stream, tau_ast_path_segment_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fputc('}', stream);
}
