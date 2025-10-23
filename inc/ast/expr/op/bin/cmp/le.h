/**
 * \file
 * 
 * \brief Abstract syntax tree binary less-or-equal comparison operation
 * expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_CMP_LE_H
#define TAU_AST_EXPR_OP_BIN_CMP_LE_H

#include "ast/expr/op/bin/bin.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST binary arithmetic less-or-equal comparison expression node.
 */
typedef struct tau_ast_expr_op_bin_cmp_le_t
{
  TAU_AST_EXPR_OP_BIN_HEADER;
} tau_ast_expr_op_bin_cmp_le_t;

/**
 * \brief Initializes a new AST binary less-or-equal comparison operation expression
 * node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_expr_op_bin_cmp_le_t* tau_ast_expr_op_bin_cmp_le_init(void);

/**
 * \brief Performs name resolution pass on an AST binary less-or-equal comparison
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_cmp_le_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_cmp_le_t* node);

/**
 * \brief Performs type check pass on an AST binary less-or-equal comparison
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_cmp_le_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_cmp_le_t* node);

/**
 * \brief Performs code generation pass on an AST binary less-or-equal comparison
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_bin_cmp_le_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_cmp_le_t* node);

TAU_EXTERN_C_END

#endif
