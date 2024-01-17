/**
 * \file call.h
 * 
 * \brief Abstract syntax tree call operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_CALL_H
#define TAU_AST_EXPR_OP_CALL_H

#include "stages/parser/ast/expr/op/op.h"

/**
 * \brief AST call operation expression node.
 */
typedef struct ast_expr_op_call_t
{
  AST_EXPR_OP_HEADER;
  ast_node_t* callee; // Callee expression.
  list_t* params; // List of parameter expressions.
} ast_expr_op_call_t;

#endif