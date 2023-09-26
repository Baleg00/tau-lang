/**
 * \file typetable.h
 * 
 * \brief Typetable library interface.
 * 
 * \details A typetable is a data structure used to store type information
 * associated with Abstract Syntax Tree (AST) nodes. It allows the compiler to
 * map AST nodes to their corresponding type descriptors, enabling efficient
 * retrieval and modification of type information during various compilation
 * passes. This is crucial for performing type checking, type inference, and
 * generating optimized code based on the inferred types.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#ifndef TAU_TYPETABLE_H
#define TAU_TYPETABLE_H

#include <stdbool.h>

#include "ast.h"
#include "typedesc.h"

/**
 * \brief Represents a type table.
 *
 * \details This structure represents a type table, which is used to store and
 * manage type information. The type table is responsible for associating AST
 * nodes with their corresponding type descriptors. It plays a critical role in
 * type checking and resolution during compilation. It owns neither the AST nodes
 * nor the associated type descriptors.
 */
typedef struct typetable_s typetable_t;

/**
 * \brief Initializes a new typetable.
 *
 * \returns A pointer to the created typetable.
 */
typetable_t* typetable_init(void);

/**
 * \brief Frees the memory associated with a typetable.
 *
 * \param[in] table Pointer to the typetable to free.
 */
void typetable_free(typetable_t* table);

/**
 * \brief Inserts a type descriptor associated with an AST node into the
 * typetable.
 *
 * \param[in] table Pointer to the typetable.
 * \param[in] node Pointer to the AST node.
 * \param[in] desc Pointer to the type descriptor to insert.
 *
 * \returns `true` if the insertion was successful, `false` otherwise.
 */
bool typetable_insert(typetable_t* table, ast_node_t* node, typedesc_t* desc);

/**
 * \brief Looks up and retrieves the type descriptor associated with an AST node
 * from the typetable.
 *
 * \param[in] table Pointer to the typetable.
 * \param[in] node Pointer to the AST node to look up.
 *
 * \returns A pointer to the type descriptor if found, or NULL if not found.
 */
typedesc_t* typetable_lookup(typetable_t* table, ast_node_t* node);

#endif
