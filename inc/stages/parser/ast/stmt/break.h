/**
 * \file break.h
 * 
 * \brief Abstract syntax tree break statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_BREAK_H
#define TAU_AST_STMT_BREAK_H

#include "stages/parser/ast/stmt/stmt.h"

/**
 * \brief AST break statement node.
 */
typedef struct ast_stmt_break_t
{
  AST_STMT_HEADER;
  ast_node_t* loop; // The associated loop statement.
} ast_stmt_break_t;

/**
 * \brief Initializes a new AST break statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_break_t* ast_stmt_break_init(void);

/**
 * \brief Writes a JSON dump of an AST break statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_break_dump_json(FILE* stream, ast_stmt_break_t* node);

#endif