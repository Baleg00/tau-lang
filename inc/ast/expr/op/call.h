/**
 * \file
 * 
 * \brief Abstract syntax tree call operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_CALL_H
#define TAU_AST_EXPR_OP_CALL_H

#include "ast/expr/op/op.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST call operation expression node.
 */
typedef struct tau_ast_expr_op_call_t
{
  TAU_AST_EXPR_OP_HEADER;
  tau_ast_node_t* callee; // Callee expression.
  tau_vector_t* params; // Vector of parameter expressions.
} tau_ast_expr_op_call_t;

/**
 * \brief Initializes a new AST call operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_expr_op_call_t* tau_ast_expr_op_call_init(void);

/**
 * \brief Frees all memory allocated by an AST call operation expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_expr_op_call_free(tau_ast_expr_op_call_t* node);

/**
 * \brief Performs name resolution pass on an AST call operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_call_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_call_t* node);

/**
 * \brief Performs type check pass on an AST call operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_call_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_call_t* node);

/**
 * \brief Performs code generation pass on an AST call operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_call_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_call_t* node);

/**
 * \brief Writes a JSON dump of an AST call operation expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_expr_op_call_dump_json(FILE* stream, tau_ast_expr_op_call_t* node);

TAU_EXTERN_C_END

#endif
