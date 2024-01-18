/**
 * \file if.h
 * 
 * \brief Abstract syntax tree if statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_IF_H
#define TAU_AST_STMT_IF_H

#include <llvm-c/Types.h>

#include "ast/stmt/stmt.h"

/**
 * \brief AST if statement node.
 */
typedef struct ast_stmt_if_t
{
  AST_STMT_HEADER;
  ast_node_t* cond; // The associated condition expression.
  ast_node_t* stmt; // The associated consequent statement.
  ast_node_t* stmt_else; // The associated optional alternative statement.

  LLVMBasicBlockRef llvm_then; // LLVM block for the consequent branch.
  LLVMBasicBlockRef llvm_else; // LLVM block for the alternative branch (if present).
  LLVMBasicBlockRef llvm_end; // LLVM block for the end of the if statement.
} ast_stmt_if_t;

/**
 * \brief Initializes a new AST if statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_if_t* ast_stmt_if_init(void);

/**
 * \brief Writes a JSON dump of an AST if statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_if_dump_json(FILE* stream, ast_stmt_if_t* node);

#endif