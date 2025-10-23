/**
 * \file
 * 
 * \brief Symbol table interface.
 * 
 * \details A symbol table is a data structure used to store information about
 * symbols, such as variables, functions, and types, encountered during the
 * compilation of a program. It serves as a crucial component for managing the
 * program's scope, resolving identifiers, and tracking symbol attributes like
 * data types, memory locations, and visibility. Symbol tables facilitate the
 * association between symbol names and their corresponding declarations or
 * definitions, aiding in tasks like type checking, scope resolution, and code
 * generation during the compilation process.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_SYMTABLE_H
#define TAU_SYMTABLE_H

#include "utils/common.h"
#include "utils/collections/vector.h"
#include "utils/str_view.h"

TAU_EXTERN_C_BEGIN

/**
 * \see ast/node.h
 */
typedef struct tau_ast_node_t tau_ast_node_t;

/**
 * \brief Represents a symbol in a symbol table.
 * 
 * \details This structure holds information, such as the symbol's identifier
 * and associated AST node. Symbols are used to store and manage identifiers
 * in the symbol table.
 */
typedef struct tau_symbol_t tau_symbol_t;

/**
 * \brief Represents a symbol table.
 * 
 * \details A symbol table is a data structure used to store and manage symbols,
 * which are identifiers associated with their respective AST nodes. Symbol
 * tables are organized hierarchically and can have parent tables.
 */
typedef struct tau_symtable_t tau_symtable_t;

struct tau_symbol_t
{
  tau_symtable_t* parent; ///< The symbol's parent symbol table.
  const char* id;     ///< The symbol's identifier.
  size_t len;         ///< The length of the identifier.
  tau_ast_node_t* node;   ///< The AST node associated with the symbol.
  tau_symbol_t* next;     ///< The next symbol in the same bucket (for handling collisions).
};

struct tau_symtable_t
{
  tau_symtable_t* parent; ///< Pointer to the parent symbol table.
  tau_vector_t* children; ///< Nested child symbol tables.
  size_t size;        ///< The number of symbols stored in the table.
  size_t capacity;    ///< The number of buckets in the symbol table.
  tau_symbol_t** buckets; ///< Array of hash table buckets.
};

/**
 * \brief Initializes a new symbol with the given identifier and associated AST
 * node.
 *
 * \param[in] id Pointer to the beginning of the identifier.
 * \param[in] len The length of the identifier.
 * \param[in] node Pointer to the AST node associated with the symbol.
 * \returns A pointer to the newly initialized symbol.
 */
tau_symbol_t* tau_symbol_init(const char* id, size_t len, tau_ast_node_t* node);

/**
 * \brief Initializes a new symbol with the given identifier and associated AST
 * node.
 *
 * \param[in] id String view of the identifier.
 * \param[in] node Pointer to the AST node associated with the symbol.
 * \returns A pointer to the newly initialized symbol.
 */
tau_symbol_t* tau_symbol_init_with_str_view(tau_string_view_t id, tau_ast_node_t* node);

/**
 * \brief Frees the resources associated with a symbol.
 *
 * \param[in] sym Pointer to the symbol to be freed.
 */
void tau_symbol_free(tau_symbol_t* sym);

/**
 * \brief Initializes a new symbol table with an optional parent symbol table.
 *
 * \param[in] parent Pointer to the parent symbol table, can be NULL.
 * \returns A pointer to the newly initialized symbol table.
 */
tau_symtable_t* tau_symtable_init(tau_symtable_t* parent);

/**
 * \brief Frees the resources associated with a symbol table.
 *
 * \param[in] table Pointer to the symbol table to be freed.
 */
void tau_symtable_free(tau_symtable_t* table);

/**
 * \brief Inserts a symbol into the symbol table.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] new_sym Pointer to the symbol to be inserted.
 * \returns NULL if insertion was successful, otherwise a pointer to the
 * colliding symbol.
 */
tau_symbol_t* tau_symtable_insert(tau_symtable_t* table, tau_symbol_t* new_sym);

/**
 * \brief Retrieves a symbol from a symbol table without traversing the symbol
 * table hierarchy.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] id Pointer to the beginning of the identifier.
 * \param[in] len The length of the identifier.
 * \returns A pointer to the symbol or NULL if it was not found.
 */
tau_symbol_t* tau_symtable_get(tau_symtable_t* table, const char* id, size_t len);

/**
 * \brief Retrieves a symbol from a symbol table without traversing the symbol
 * table hierarchy.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] id String view of the identifier.
 * \returns A pointer to the symbol or NULL if it was not found.
 */
tau_symbol_t* tau_symtable_get_with_str_view(tau_symtable_t* table, tau_string_view_t id);

/**
 * \brief Retrieves a symbol from a symbol table hierarchy.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] id Pointer to the beginning of the identifier.
 * \param[in] len The length of the identifier.
 * \returns A pointer to the symbol or NULL if it was not found.
 */
tau_symbol_t* tau_symtable_lookup(tau_symtable_t* table, const char* id, size_t len);

/**
 * \brief Retrieves a symbol from a symbol table hierarchy.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] id String view of the identifier.
 * \returns A pointer to the symbol or NULL if it was not found.
 */
tau_symbol_t* tau_symtable_lookup_with_str_view(tau_symtable_t* table, tau_string_view_t id);

/**
 * \brief Merges a symbol table into another. The source symbol table is
 * freed in the process.
 *
 * \param[in,out] dest Pointer to the symbol table to merge into.
 * \param[in] src Pointer to the symbol table to be merged.
 */
void tau_symtable_merge(tau_symtable_t* dest, tau_symtable_t* src);

TAU_EXTERN_C_END

#endif
