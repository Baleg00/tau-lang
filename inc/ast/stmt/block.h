/**
 * \file
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
  symtable_t* scope; // The associated scope.
  vector_t* stmts;   // Collection of statements within the block.
} ast_stmt_block_t;

/**
 * \brief Initializes a new AST block statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_block_t* ast_stmt_block_init(void);

/**
 * \brief Frees all memory allocated by an AST block statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_stmt_block_free(ast_stmt_block_t* node);

/**
 * \brief Performs name resolution pass on an AST block statement node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_block_nameres(nameres_ctx_t* ctx, ast_stmt_block_t* node);

/**
 * \brief Performs type check pass on an AST block statement node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_block_typecheck(typecheck_ctx_t* ctx, ast_stmt_block_t* node);

/**
 * \brief Performs control flow analysis pass on an AST block statement node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_block_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_block_t* node);

/**
 * \brief Performs code generation pass on an AST block statement node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_block_codegen(codegen_ctx_t* ctx, ast_stmt_block_t* node);

/**
 * \brief Writes a JSON dump of an AST block statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_block_dump_json(FILE* stream, ast_stmt_block_t* node);

#endif