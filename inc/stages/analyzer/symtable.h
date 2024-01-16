/**
 * \file symtable.h
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

#include <stddef.h>

#include "stages/parser/ast/ast.h"
#include "utils/collections/list.h"
#include "utils/str_view.h"

/**
 * \brief Represents a symbol in a symbol table.
 * 
 * \details This structure holds information, such as the symbol's identifier
 * and associated AST node. Symbols are used to store and manage identifiers
 * in the symbol table.
 */
typedef struct symbol_t symbol_t;

/**
 * \brief Represents a symbol table.
 * 
 * \details A symbol table is a data structure used to store and manage symbols,
 * which are identifiers associated with their respective AST nodes. Symbol
 * tables are organized hierarchically and can have parent tables.
 */
typedef struct symtable_t symtable_t;

struct symbol_t
{
  symtable_t* parent; // The symbol's parent symbol table.
  const char* id; // The symbol's identifier.
  size_t len; // The length of the identifier.
  ast_node_t* node; // The AST node associated with the symbol.
  symbol_t* next; // The next symbol in the same bucket (for handling collisions).
};

struct symtable_t
{
  symtable_t* parent; // The parent symbol table (lexical scope).
  list_t* children; // List of child symbol tables (nested scopes).
  size_t size; // The number of symbols stored in the table.
  size_t capacity; // The capacity (number of buckets) of the symbol table.
  symbol_t** buckets; // An array of symbol pointers (hash buckets).
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
symbol_t* symbol_init(const char* id, size_t len, ast_node_t* node);

/**
 * \brief Initializes a new symbol with the given identifier and associated AST
 * node.
 *
 * \param[in] id String view of the identifier.
 * \param[in] node Pointer to the AST node associated with the symbol.
 * \returns A pointer to the newly initialized symbol.
 */
symbol_t* symbol_init_with_str_view(string_view_t id, ast_node_t* node);

/**
 * \brief Frees the resources associated with a symbol.
 *
 * \param[in] sym Pointer to the symbol to be freed.
 */
void symbol_free(symbol_t* sym);

/**
 * \brief Initializes a new symbol table with an optional parent symbol table.
 *
 * \param[in] parent Pointer to the parent symbol table, can be NULL.
 * \returns A pointer to the newly initialized symbol table.
 */
symtable_t* symtable_init(symtable_t* parent);

/**
 * \brief Frees the resources associated with a symbol table.
 *
 * \param[in] table Pointer to the symbol table to be freed.
 */
void symtable_free(symtable_t* table);

/**
 * \brief Inserts a symbol into the symbol table.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] new_sym Pointer to the symbol to be inserted.
 * \returns NULL if insertion was successful, otherwise a pointer to the
 * colliding symbol.
 */
symbol_t* symtable_insert(symtable_t* table, symbol_t* new_sym);

/**
 * \brief Retrieves a symbol from a symbol table without traversing the symbol
 * table hierarchy.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] id Pointer to the beginning of the identifier.
 * \param[in] len The length of the identifier.
 * \returns A pointer to the symbol or NULL if it was not found.
 */
symbol_t* symtable_get(symtable_t* table, const char* id, size_t len);

/**
 * \brief Retrieves a symbol from a symbol table without traversing the symbol
 * table hierarchy.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] id String view of the identifier.
 * \returns A pointer to the symbol or NULL if it was not found.
 */
symbol_t* symtable_get_with_str_view(symtable_t* table, string_view_t id);

/**
 * \brief Retrieves a symbol from a symbol table hierarchy.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] id Pointer to the beginning of the identifier.
 * \param[in] len The length of the identifier.
 * \returns A pointer to the symbol or NULL if it was not found.
 */
symbol_t* symtable_lookup(symtable_t* table, const char* id, size_t len);

/**
 * \brief Retrieves a symbol from a symbol table hierarchy.
 *
 * \param[in] table Pointer to the symbol table.
 * \param[in] id String view of the identifier.
 * \returns A pointer to the symbol or NULL if it was not found.
 */
symbol_t* symtable_lookup_with_str_view(symtable_t* table, string_view_t id);

/**
 * \brief Expands the capacity of the symbol table to accommodate more symbols.
 *
 * \param[in] table Pointer to the symbol table.
 */
void symtable_expand(symtable_t* table);

#endif