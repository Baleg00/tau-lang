/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/id.h"

#include "ast/registry.h"

tau_ast_id_t* tau_ast_id_init(void)
{
  tau_ast_id_t* node = (tau_ast_id_t*)malloc(sizeof(tau_ast_id_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_ID;

  return node;
}

void tau_ast_id_free(tau_ast_id_t* node)
{
  free(node);
}

void tau_ast_id_dump_json(FILE* stream, tau_ast_id_t* node)
{
  tau_location_t loc = tau_token_location(node->tok);

  fprintf(stream, "{\"kind\":\"%s\",\"id\":\"%.*s\"}",
    tau_ast_kind_to_cstr(node->kind),
    (int)loc.len, loc.ptr);
}
