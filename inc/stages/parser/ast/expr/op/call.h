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

/**
 * \brief Initializes a new AST call operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_call_t* ast_expr_op_call_init(void);

/**
 * \brief Writes a JSON dump of an AST call operation expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_op_call_dump_json(FILE* stream, ast_expr_op_call_t* node);

#endif