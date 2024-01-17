/**
 * \file un.h
 * 
 * \brief Abstract syntax tree unary operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_UN_H
#define TAU_AST_EXPR_OP_UN_H

#include "stages/parser/ast/expr/op/op.h"

/**
 * \brief AST unary operation expression node.
 */
typedef struct ast_expr_op_un_t
{
  AST_EXPR_OP_HEADER;
  ast_node_t* expr; // The associated expression node.
} ast_expr_op_un_t;

#endif