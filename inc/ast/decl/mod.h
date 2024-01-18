/**
 * \file mod.h
 * 
 * \brief Abstract syntax tree module declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_MOD_H
#define TAU_AST_DECL_MOD_H

#include "ast/decl/decl.h"

/**
 * \brief Forward declaration of symbol table to avoid circular includes.
 * 
 * \see symtable.h
 */
typedef struct symtable_t symtable_t;

/**
 * \brief AST module declaration node.
 */
typedef struct ast_decl_mod_t
{
  AST_DECL_HEADER;
  symtable_t* scope; // The associated scope of members.
  vector_t* members; // Vector of associated member declarations.
  bool is_pub; // Is enum public (in case it is a member).
} ast_decl_mod_t;

/**
 * \brief Initializes a new AST module declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_mod_t* ast_decl_mod_init(void);

/**
 * \brief Frees all memory allocated by an AST module declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_decl_mod_free(ast_decl_mod_t* node);

/**
 * \brief Writes a JSON dump of an AST module declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_mod_dump_json(FILE* stream, ast_decl_mod_t* node);

#endif