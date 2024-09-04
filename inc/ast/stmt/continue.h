/**
 * \file
 * 
 * \brief Abstract syntax tree continue statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_CONTINUE_H
#define TAU_AST_STMT_CONTINUE_H

#include "ast/stmt/stmt.h"

/**
 * \brief AST continue statement node.
 */
typedef struct ast_stmt_continue_t
{
  AST_STMT_HEADER;
  ast_node_t* loop; // The associated loop statement.
} ast_stmt_continue_t;

/**
 * \brief Initializes a new AST continue statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_continue_t* ast_stmt_continue_init(void);

/**
 * \brief Frees all memory allocated by an AST continue statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_stmt_continue_free(ast_stmt_continue_t* node);

/**
 * \brief Performs name resolution pass on an AST continue statement node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_continue_nameres(nameres_ctx_t* ctx, ast_stmt_continue_t* node);

/**
 * \brief Performs type check pass on an AST continue statement node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_continue_typecheck(typecheck_ctx_t* ctx, ast_stmt_continue_t* node);

/**
 * \brief Performs control flow analysis pass on an AST continue statement node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_continue_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_continue_t* node);

/**
 * \brief Performs code generation pass on an AST continue statement node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_continue_codegen(codegen_ctx_t* ctx, ast_stmt_continue_t* node);

/**
 * \brief Writes a JSON dump of an AST continue statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_continue_dump_json(FILE* stream, ast_stmt_continue_t* node);

#endif
