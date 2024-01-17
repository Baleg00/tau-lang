/**
 * \file bin.h
 * 
 * \brief Abstract syntax tree binary operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_H
#define TAU_AST_EXPR_OP_BIN_H

#include "stages/parser/ast/expr/op/op.h"

/**
 * \brief AST binary operation expression node.
 */
typedef struct ast_expr_op_bin_t
{
  AST_EXPR_OP_HEADER;
  ast_node_t* lhs; // The associated left-hand side expression node.
  ast_node_t* rhs; // The associated right-hand side expression node.
} ast_expr_op_bin_t;

#endif