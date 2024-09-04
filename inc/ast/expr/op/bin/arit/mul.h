/**
 * \file
 * 
 * \brief Abstract syntax tree binary arithmetic multiplication operation
 * expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_ARIT_MUL_H
#define TAU_AST_EXPR_OP_BIN_ARIT_MUL_H

#include "ast/expr/op/bin/bin.h"

/**
 * \brief AST binary arithmetic multiplication operation expression node.
 */
typedef struct ast_expr_op_bin_arit_mul_t
{
  AST_EXPR_OP_BIN_HEADER;
} ast_expr_op_bin_arit_mul_t;

/**
 * \brief Initializes a new AST binary arithmetic multiplication operation expression
 * node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_bin_arit_mul_t* ast_expr_op_bin_arit_mul_init(void);

/**
 * \brief Performs name resolution pass on an AST binary arithmetic multiplication
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_mul_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node);

/**
 * \brief Performs type check pass on an AST binary arithmetic multiplication
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_mul_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node);

/**
 * \brief Performs code generation pass on an AST binary arithmetic multiplication
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_arit_mul_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_arit_mul_t* node);

#endif
