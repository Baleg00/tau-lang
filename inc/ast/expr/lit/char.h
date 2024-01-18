/**
 * \file str.h
 * 
 * \brief Abstract syntax tree literal character expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_CHAR_H
#define TAU_AST_EXPR_LIT_CHAR_H

#include "ast/expr/lit/lit.h"

/**
 * \brief AST literal character expression node.
 */
typedef struct ast_expr_lit_char_t
{
  AST_EXPR_LIT_HEADER;
  uint32_t value; // The associated character value.
} ast_expr_lit_char_t;

/**
 * \brief Initializes a new AST literal character expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_lit_char_t* ast_expr_lit_char_init(void);

/**
 * \brief Writes a JSON dump of an AST literal character expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_lit_char_dump_json(FILE* stream, ast_expr_lit_char_t* node);

#endif