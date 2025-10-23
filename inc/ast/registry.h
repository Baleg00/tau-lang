/**
 * \file
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

TAU_EXTERN_C_BEGIN

/**
 * \brief Adds an AST node to the registry.
 * 
 * \param[in] node Pointer to the node to be added.
 */
void tau_ast_registry_register(tau_ast_node_t* node);

/**
 * \brief Frees all registered AST nodes. 
 */
void tau_ast_registry_free(void);

TAU_EXTERN_C_END

#endif
