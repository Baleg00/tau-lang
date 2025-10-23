/**
 * \file
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

#include "stages/analysis/types/typedesc/typedesc.h"
#include "utils/common.h"

TAU_EXTERN_C_BEGIN

/**
 * \see ast/node.h 
 */
typedef struct tau_ast_node_t tau_ast_node_t;

/**
 * \brief Represents a type table.
 *
 * \details This structure represents a type table, which is used to store and
 * manage type information. The type table is responsible for associating AST
 * nodes with their corresponding type descriptors. It plays a critical role in
 * type checking and resolution during compilation. It owns neither the AST nodes
 * nor the associated type descriptors.
 */
typedef struct tau_typetable_t tau_typetable_t;

/**
 * \brief Initializes a new typetable.
 *
 * \returns A pointer to the created typetable.
 */
tau_typetable_t* tau_typetable_init(void);

/**
 * \brief Frees the memory associated with a typetable.
 *
 * \param[in] table Pointer to the typetable to free.
 */
void tau_typetable_free(tau_typetable_t* table);

/**
 * \brief Inserts a type descriptor associated with an AST node into the
 * typetable.
 *
 * \param[in] table Pointer to the typetable.
 * \param[in] node Pointer to the AST node.
 * \param[in] desc Pointer to the type descriptor to insert.
 *
 * \returns Pointer to the old type descriptor if an overwrite occurred, `NULL`
 * otherwise.
 */
tau_typedesc_t* tau_typetable_insert(tau_typetable_t* table, tau_ast_node_t* node, tau_typedesc_t* desc);

/**
 * \brief Looks up and retrieves the type descriptor associated with an AST node
 * from the typetable.
 *
 * \param[in] table Pointer to the typetable.
 * \param[in] node Pointer to the AST node to look up.
 *
 * \returns A pointer to the type descriptor if found, or NULL if not found.
 */
tau_typedesc_t* tau_typetable_lookup(tau_typetable_t* table, tau_ast_node_t* node);

/**
 * \brief Merges a type table into another. The source type table is freed
 * in the process.
 *
 * \param[in,out] dest Pointer to the type table to merge into.
 * \param[in] src Pointer to the type table to be merged.
 */
void tau_typetable_merge(tau_typetable_t* dest, tau_typetable_t* src);

TAU_EXTERN_C_END

#endif
