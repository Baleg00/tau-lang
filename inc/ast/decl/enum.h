/**
 * \file enum.h
 * 
 * \brief Abstract syntax tree enum declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_ENUM_H
#define TAU_AST_DECL_ENUM_H

#include <llvm-c/Types.h>

#include "ast/decl/decl.h"

/**
 * \brief Forward declaration of symbol table to avoid circular includes.
 * 
 * \see symtable.h
 */
typedef struct symtable_t symtable_t;

/**
 * \brief AST enum declaration node.
 */
typedef struct ast_decl_enum_t
{
  AST_DECL_HEADER;
  symtable_t* scope; // The associated scope of members.
  vector_t* members; // Vector of associated member declarations.
  bool is_pub; // Is enum public (in case it is a member).

  LLVMTypeRef llvm_type; // The associated LLVM type.
} ast_decl_enum_t;

/**
 * \brief Initializes a new AST enum declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_enum_t* ast_decl_enum_init(void);

/**
 * \brief Frees all memory allocated by an AST enum declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_decl_enum_free(ast_decl_enum_t* node);

/**
 * \brief Writes a JSON dump of an AST enum declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_enum_dump_json(FILE* stream, ast_decl_enum_t* node);

#endif