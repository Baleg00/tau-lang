/**
 * \file decl.h
 * 
 * \brief Abstract syntax tree declaration expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_DECL_H
#define TAU_AST_EXPR_DECL_H

#include "stages/parser/ast/expr/expr.h"

/**
 * \brief AST declaration expression node.
 */
typedef struct ast_expr_decl_t
{
  AST_EXPR_HEADER;
  ast_node_t* decl; // The associated declaration node.
} ast_expr_decl_t;

#endif