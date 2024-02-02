/**
 * \file
 * 
 * \brief Abstract syntax tree expression statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_EXPR_H
#define TAU_AST_STMT_EXPR_H

#include "ast/stmt/stmt.h"

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
 * \brief Frees all memory allocated by an AST expression statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_stmt_expr_free(ast_stmt_expr_t* node);

/**
 * \brief Performs name resolution pass on an AST expression statement node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_expr_nameres(nameres_ctx_t* ctx, ast_stmt_expr_t* node);

/**
 * \brief Performs type check pass on an AST expression statement node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_expr_typecheck(typecheck_ctx_t* ctx, ast_stmt_expr_t* node);

/**
 * \brief Performs code generation pass on an AST expression statement node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_expr_codegen(codegen_ctx_t* ctx, ast_stmt_expr_t* node);

/**
 * \brief Writes a JSON dump of an AST expression statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_expr_dump_json(FILE* stream, ast_stmt_expr_t* node);

#endif