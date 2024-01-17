/**
 * \file defer.h
 * 
 * \brief Abstract syntax tree defer statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_DEFER_H
#define TAU_AST_STMT_DEFER_H

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST defer statement node.
 */
typedef struct ast_stmt_defer_t
{
  AST_STMT_HEADER;
  ast_node_t* stmt; // The associated statement.
} ast_stmt_defer_t;

#endif