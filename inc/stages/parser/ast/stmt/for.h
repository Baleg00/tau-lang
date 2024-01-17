/**
 * \file for.h
 * 
 * \brief Abstract syntax tree for-loop statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_FOR_H
#define TAU_AST_STMT_FOR_H

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST for-loop statement node.
 */
typedef struct ast_stmt_for_t
{
  AST_STMT_HEADER;
  ast_node_t* var; // The associated loop variable declaration.
  ast_node_t* range; // The associated range expression.
  ast_node_t* stmt; // The associated body statement.
} ast_stmt_for_t;

#endif