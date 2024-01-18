/**
 * \file union.h
 * 
 * \brief Abstract syntax tree union declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_UNION_H
#define TAU_AST_DECL_UNION_H

#include <llvm-c/Types.h>

#include "ast/decl/decl.h"

/**
 * \brief Forward declaration of symbol table to avoid circular includes.
 * 
 * \see symtable.h
 */
typedef struct symtable_t symtable_t;

/**
 * \brief AST union declaration node.
 */
typedef struct ast_decl_union_t
{
  AST_DECL_HEADER;
  symtable_t* scope; // The associated scope of members.
  list_t* members; // List of associated member declarations.
  bool is_pub; // Is union public (in case it is a member).

  LLVMTypeRef llvm_type; // The associated LLVM type (same as the type of the largest member).
} ast_decl_union_t;

/**
 * \brief Initializes a new AST union declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_union_t* ast_decl_union_init(void);

/**
 * \brief Writes a JSON dump of an AST union declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_union_dump_json(FILE* stream, ast_decl_union_t* node);

#endif