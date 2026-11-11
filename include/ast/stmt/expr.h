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

TAU_EXTERN_C_BEGIN

/**
 * \brief AST expression statement node.
 */
typedef struct tau_ast_stmt_expr_t
{
  TAU_AST_STMT_HEADER;
  tau_ast_node_t* expr; // The associated expression.
} tau_ast_stmt_expr_t;

/**
 * \brief Initializes a new AST expression statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_stmt_expr_t* tau_ast_stmt_expr_init(void);

/**
 * \brief Frees all memory allocated by an AST expression statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_stmt_expr_free(tau_ast_stmt_expr_t* node);

/**
 * \brief Performs name resolution pass on an AST expression statement node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_stmt_expr_nameres(tau_nameres_ctx_t* ctx, tau_ast_stmt_expr_t* node);

/**
 * \brief Performs type check pass on an AST expression statement node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_stmt_expr_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_stmt_expr_t* node);

/**
 * \brief Performs code generation pass on an AST expression statement node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_stmt_expr_codegen(tau_codegen_ctx_t* ctx, tau_ast_stmt_expr_t* node);

/**
 * \brief Writes a JSON dump of an AST expression statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_stmt_expr_dump_json(FILE* stream, tau_ast_stmt_expr_t* node);

TAU_EXTERN_C_END

#endif
