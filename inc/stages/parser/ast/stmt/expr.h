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

/**
 * \brief Initializes a new AST expression statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_expr_t* ast_stmt_expr_init(void);

/**
 * \brief Writes a JSON dump of an AST expression statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_expr_dump_json(FILE* stream, ast_stmt_expr_t* node);

#endif