/**
 * \file registry.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/registry.h"

#include "utils/collections/vector.h"

#define AST_REGISTRY_DEFAULT_CAPACITY ((size_t)(1 << 8))

/**
 * \brief Vector of all registered AST nodes.
 */
static vector_t* g_ast_registry_nodes = NULL;

void ast_registry_register(ast_node_t* node)
{
  if (g_ast_registry_nodes == NULL)
    g_ast_registry_nodes = vector_init_with_capacity(AST_REGISTRY_DEFAULT_CAPACITY);

  vector_push(g_ast_registry_nodes, node);
}

void ast_registry_free(void)
{
  for (size_t i = 0; i < vector_size(g_ast_registry_nodes); i++)
    ast_node_free((ast_node_t*)vector_get(g_ast_registry_nodes, i));

  vector_free(g_ast_registry_nodes);
  g_ast_registry_nodes = NULL;
}
