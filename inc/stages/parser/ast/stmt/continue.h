/**
 * \file continue.h
 * 
 * \brief Abstract syntax tree continue statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_CONTINUE_H
#define TAU_AST_STMT_CONTINUE_H

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST continue statement node.
 */
typedef struct ast_stmt_continue_t
{
  AST_STMT_HEADER;
  ast_node_t* loop; // The associated loop statement.
} ast_stmt_continue_t;

#endif