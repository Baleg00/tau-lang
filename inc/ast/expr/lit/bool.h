/**
 * \file bool.h
 * 
 * \brief Abstract syntax tree literal boolean expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_BOOL_H
#define TAU_AST_EXPR_LIT_BOOL_H

#include "ast/expr/lit/lit.h"

/**
 * \brief AST literal boolean expression node.
 */
typedef struct ast_expr_lit_bool_t
{
  AST_EXPR_LIT_HEADER;
  bool value; // The associated boolean value.
} ast_expr_lit_bool_t;

/**
 * \brief Initializes a new AST literal boolean expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_lit_bool_t* ast_expr_lit_bool_init(void);

/**
 * \brief Writes a JSON dump of an AST literal boolean expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_lit_bool_dump_json(FILE* stream, ast_expr_lit_bool_t* node);

#endif