/**
 * \file expr.h
 * 
 * \brief Abstract syntax tree expression statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_EXPR_H
#define TAU_AST_STMT_EXPR_H

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST expression statement node.
 */
typedef struct ast_stmt_expr_t
{
  AST_STMT_HEADER;
  ast_node_t* expr; // The associated expression.
} ast_stmt_expr_t;

#endif