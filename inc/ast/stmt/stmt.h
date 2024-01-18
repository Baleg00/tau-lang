/**
 * \file stmt.h
 * 
 * \brief Abstract syntax tree statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_H
#define TAU_AST_STMT_H

#include "ast/node.h"

/**
 * \brief Header for all AST statement nodes.
 */
#define AST_STMT_HEADER\
  AST_NODE_HEADER

/**
 * \brief AST statement node.
 */
typedef struct ast_stmt_t
{
  AST_STMT_HEADER;
} ast_stmt_t;

#endif