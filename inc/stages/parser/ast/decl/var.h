/**
 * \file var.h
 * 
 * \brief Abstract syntax tree variable declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_VAR_H
#define TAU_AST_DECL_VAR_H

#include <llvm-c/Types.h>

#include "stages/parser/ast/decl/decl.h"

/**
 * \brief AST variable declaration node.
 */
typedef struct ast_decl_var_t
{
  AST_DECL_HEADER;
  ast_node_t* type; // The associated type.
  ast_node_t* expr; // The associated initializer expression.
  bool is_pub; // Is variable public (in case it is a member).
  bool is_extern; // Is variable external.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.
} ast_decl_var_t;

/**
 * \brief Initializes a new AST variable declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_var_t* ast_decl_var_init(void);

/**
 * \brief Writes a JSON dump of an AST variable declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_var_dump_json(FILE* stream, ast_decl_var_t* node);

#endif