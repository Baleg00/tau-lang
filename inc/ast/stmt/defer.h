/**
 * \file defer.h
 * 
 * \brief Abstract syntax tree defer statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_DEFER_H
#define TAU_AST_STMT_DEFER_H

#include "ast/stmt/stmt.h"

/**
 * \brief AST defer statement node.
 */
typedef struct ast_stmt_defer_t
{
  AST_STMT_HEADER;
  ast_node_t* stmt; // The associated statement.
} ast_stmt_defer_t;

/**
 * \brief Initializes a new AST defer statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_defer_t* ast_stmt_defer_init(void);

/**
 * \brief Frees all memory allocated by an AST defer statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_stmt_defer_free(ast_stmt_defer_t* node);

/**
 * \brief Writes a JSON dump of an AST defer statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_defer_dump_json(FILE* stream, ast_stmt_defer_t* node);

#endif