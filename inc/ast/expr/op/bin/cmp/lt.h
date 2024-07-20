/**
 * \file
 * 
 * \brief Abstract syntax tree binary less-than comparison operation expression
 * node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_CMP_LT_H
#define TAU_AST_EXPR_OP_BIN_CMP_LT_H

#include "ast/expr/op/bin/bin.h"

/**
 * \brief AST binary arithmetic less-than comparison expression node.
 */
typedef struct ast_expr_op_bin_cmp_lt_t
{
  AST_EXPR_OP_BIN_HEADER;
} ast_expr_op_bin_cmp_lt_t;

/**
 * \brief Initializes a new AST binary less-than comparison operation expression
 * node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_bin_cmp_lt_t* ast_expr_op_bin_cmp_lt_init(void);

/**
 * \brief Performs name resolution pass on an AST binary less-than comparison
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_lt_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_cmp_lt_t* node);

/**
 * \brief Performs type check pass on an AST binary less-than comparison
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_lt_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_cmp_lt_t* node);

/**
 * \brief Performs code generation pass on an AST binary less-than comparison
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_lt_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_cmp_lt_t* node);

#endif