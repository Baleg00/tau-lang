/**
 * \file
 * 
 * \brief Abstract syntax tree binary operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_H
#define TAU_AST_EXPR_OP_BIN_H

#include "ast/expr/op/op.h"

/**
 * \brief Header for all AST binary operation expression nodes.
 */
#define TAU_AST_EXPR_OP_BIN_HEADER\
  TAU_AST_EXPR_OP_HEADER;\
  tau_ast_node_t* lhs; /** The associated left-hand side expression node. */\
  tau_ast_node_t* rhs /** The associated right-hand side expression node. */

TAU_EXTERN_C_BEGIN

/**
 * \brief AST binary operation expression node.
 */
typedef struct tau_ast_expr_op_bin_t
{
  TAU_AST_EXPR_OP_BIN_HEADER;
} tau_ast_expr_op_bin_t;

/**
 * \brief Initializes a new AST binary operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_expr_op_bin_t* tau_ast_expr_op_bin_init(void);

/**
 * \brief Frees all memory allocated by an AST binary operation expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_expr_op_bin_free(tau_ast_expr_op_bin_t* node);

/**
 * \brief Performs name resolution pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_t* node);

/**
 * \brief Writes a JSON dump of an AST binary operation expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_expr_op_bin_dump_json(FILE* stream, tau_ast_expr_op_bin_t* node);

TAU_EXTERN_C_END

#endif
