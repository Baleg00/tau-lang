/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/registry.h"

#include "utils/collections/vector.h"

#define TAU_AST_REGISTRY_DEFAULT_CAPACITY ((size_t)(1 << 8))

/**
 * \brief Vector of all registered AST nodes.
 */
static tau_vector_t* g_ast_registry_nodes = NULL;

void tau_ast_registry_register(tau_ast_node_t* node)
{
  if (g_ast_registry_nodes == NULL)
    g_ast_registry_nodes = tau_vector_init_with_capacity(TAU_AST_REGISTRY_DEFAULT_CAPACITY);

  tau_vector_push(g_ast_registry_nodes, node);
}

void tau_ast_registry_free(void)
{
  for (size_t i = 0; i < tau_vector_size(g_ast_registry_nodes); i++)
    tau_ast_node_free((tau_ast_node_t*)tau_vector_get(g_ast_registry_nodes, i));

  tau_vector_free(g_ast_registry_nodes);
  g_ast_registry_nodes = NULL;
}
