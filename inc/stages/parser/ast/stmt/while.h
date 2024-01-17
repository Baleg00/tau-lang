/**
 * \file while.h
 * 
 * \brief Abstract syntax tree while-loop statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_WHILE_H
#define TAU_AST_STMT_WHILE_H

#include <llvm-c/Types.h>

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST while-loop statement node.
 */
typedef struct ast_stmt_while_t
{
  AST_STMT_HEADER;
  ast_node_t* cond; // The associated condition expression.
  ast_node_t* stmt; // The associated body statement.

  LLVMBasicBlockRef llvm_cond; // LLVM block for the condition expression.
  LLVMBasicBlockRef llvm_loop; // LLVM block for the body statement.
  LLVMBasicBlockRef llvm_end; // LLVM block for the end of the loop.
} ast_stmt_while_t;

#endif