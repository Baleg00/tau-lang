/**
 * \file
 * 
 * \brief Abstract syntax tree binary bitwise right-shift operation expression
 * node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_BIT_RSH_H
#define TAU_AST_EXPR_OP_BIN_BIT_RSH_H

#include "ast/expr/op/bin/bin.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST binary arithmetic bitwise right-shift operation expression node.
 */
typedef struct tau_ast_expr_op_bin_bit_rsh_t
{
  TAU_AST_EXPR_OP_BIN_HEADER;
} tau_ast_expr_op_bin_bit_rsh_t;

/**
 * \brief Initializes a new AST binary bitwise right-shift operation expression
 * node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_expr_op_bin_bit_rsh_t* tau_ast_expr_op_bin_bit_rsh_init(void);

/**
 * \brief Performs name resolution pass on an AST binary bitwise right-shift
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_bit_rsh_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_bit_rsh_t* node);

/**
 * \brief Performs type check pass on an AST binary bitwise right-shift operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_bit_rsh_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_bit_rsh_t* node);

/**
 * \brief Performs code generation pass on an AST binary bitwise right-shift
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_bit_rsh_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_bit_rsh_t* node);

TAU_EXTERN_C_END

#endif
