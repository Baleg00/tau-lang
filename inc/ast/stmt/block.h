/**
 * \file block.h
 * 
 * \brief Abstract syntax tree block statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_BLOCK_H
#define TAU_AST_STMT_BLOCK_H

#include "ast/stmt/stmt.h"

/**
 * \brief AST block statement node.
 */
typedef struct ast_stmt_block_t
{
  AST_STMT_HEADER;
  list_t* stmts; // The list of associated statements.
} ast_stmt_block_t;

/**
 * \brief Initializes a new AST block statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_block_t* ast_stmt_block_init(void);

/**
 * \brief Writes a JSON dump of an AST block statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_block_dump_json(FILE* stream, ast_stmt_block_t* node);

#endif