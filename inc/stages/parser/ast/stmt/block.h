/**
 * \file block.h
 * 
 * \brief Abstract syntax tree block statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_BLOCK_H
#define TAU_AST_STMT_BLOCK_H

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST block statement node.
 */
typedef struct ast_stmt_block_t
{
  AST_STMT_HEADER;
  list_t* stmts; // The list of associated statements.
} ast_stmt_block_t;

#endif