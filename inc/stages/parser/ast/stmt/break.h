/**
 * \file break.h
 * 
 * \brief Abstract syntax tree break statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_BREAK_H
#define TAU_AST_STMT_BREAK_H

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST break statement node.
 */
typedef struct ast_stmt_break_t
{
  AST_STMT_HEADER;
  ast_node_t* loop; // The associated loop statement.
} ast_stmt_break_t;

#endif