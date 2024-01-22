/**
 * \file return.h
 * 
 * \brief Abstract syntax tree return statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_RETURN_H
#define TAU_AST_STMT_RETURN_H

#include "ast/stmt/stmt.h"

/**
 * \brief AST return statement node.
 */
typedef struct ast_stmt_return_t
{
  AST_STMT_HEADER;
  ast_node_t* expr; // The associated optional expression.
} ast_stmt_return_t;

/**
 * \brief Initializes a new AST return statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_return_t* ast_stmt_return_init(void);

/**
 * \brief Frees all memory allocated by an AST return statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_stmt_return_free(ast_stmt_return_t* node);

/**
 * \brief Performs name resolution pass on an AST return statement node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_return_nameres(nameres_ctx_t* ctx, ast_stmt_return_t* node);

/**
 * \brief Performs type check pass on an AST return statement node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_return_typecheck(typecheck_ctx_t* ctx, ast_stmt_return_t* node);

/**
 * \brief Writes a JSON dump of an AST return statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_return_dump_json(FILE* stream, ast_stmt_return_t* node);

#endif