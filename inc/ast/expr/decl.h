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

#include "ast/expr/expr.h"

/**
 * \brief AST declaration expression node.
 */
typedef struct ast_expr_decl_t
{
  AST_EXPR_HEADER;
  ast_node_t* decl; // The associated declaration node.
} ast_expr_decl_t;

/**
 * \brief Initializes a new AST declaration expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_decl_t* ast_expr_decl_init(void);

/**
 * \brief Writes a JSON dump of an AST declaration expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_decl_dump_json(FILE* stream, ast_expr_decl_t* node);

#endif