/**
 * \file registry.h
 * 
 * \brief Abstract syntax tree node registry.
 * 
 * \details The AST registry is a singleton for managing and tracking AST nodes
 * in a centralized manner. This registry provides a convenient way to register
 * AST nodes and free them all at once.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_REGISTRY_H
#define TAU_AST_REGISTRY_H

#include "ast/node.h"

/**
 * \brief Adds an AST node to the registry.
 * 
 * \param[in] node Pointer to the node to be added.
 */
void ast_registry_register(ast_node_t* node);

/**
 * \brief Frees all registered AST nodes. 
 */
void ast_registry_free(void);

#endif