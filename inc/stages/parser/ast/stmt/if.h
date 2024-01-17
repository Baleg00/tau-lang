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

#include "stages/parser/ast/stmt/stmt.h"

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

#endif