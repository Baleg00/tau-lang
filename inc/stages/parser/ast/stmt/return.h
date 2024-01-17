/**
 * \file return.h
 * 
 * \brief Abstract syntax tree return statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_RETURN_H
#define TAU_AST_STMT_RETURN_H

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST return statement node.
 */
typedef struct ast_stmt_return_t
{
  AST_STMT_HEADER;
  ast_node_t* expr; // The associated optional expression.
} ast_stmt_return_t;

#endif