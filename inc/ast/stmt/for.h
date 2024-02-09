/**
 * \file
 * 
 * \brief Abstract syntax tree for-loop statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_FOR_H
#define TAU_AST_STMT_FOR_H

#include "ast/stmt/stmt.h"

/**
 * \brief AST for-loop statement node.
 */
typedef struct ast_stmt_for_t
{
  AST_STMT_HEADER;
  symtable_t* scope; // The associated scope of loop variable and loop body.
  ast_node_t* var; // The associated loop variable declaration.
  ast_node_t* range; // The associated range expression.
  ast_node_t* stmt; // The associated body statement.
} ast_stmt_for_t;

/**
 * \brief Initializes a new AST for-loop statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_for_t* ast_stmt_for_init(void);

/**
 * \brief Frees all memory allocated by an AST for-loop statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_stmt_for_free(ast_stmt_for_t* node);

/**
 * \brief Performs name resolution pass on an AST for-loop statement node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_for_nameres(nameres_ctx_t* ctx, ast_stmt_for_t* node);

/**
 * \brief Performs type check pass on an AST for-loop statement node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_for_typecheck(typecheck_ctx_t* ctx, ast_stmt_for_t* node);

/**
 * \brief Performs control flow analysis pass on an AST for-loop statement node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_for_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_for_t* node);

/**
 * \brief Performs code generation pass on an AST for-loop statement node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_for_codegen(codegen_ctx_t* ctx, ast_stmt_for_t* node);

/**
 * \brief Writes a JSON dump of an AST for-loop statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_for_dump_json(FILE* stream, ast_stmt_for_t* node);

#endif