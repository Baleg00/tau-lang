/**
 * \file
 * 
 * \brief Abstract syntax tree binary greater-or-equal comparison operation
 * expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_CMP_GE_H
#define TAU_AST_EXPR_OP_BIN_CMP_GE_H

#include "ast/expr/op/bin/bin.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST binary arithmetic greater-or-equal comparison expression node.
 */
typedef struct ast_expr_op_bin_cmp_ge_t
{
  AST_EXPR_OP_BIN_HEADER;
} ast_expr_op_bin_cmp_ge_t;

/**
 * \brief Initializes a new AST binary greater-or-equal comparison operation
 * expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_bin_cmp_ge_t* ast_expr_op_bin_cmp_ge_init(void);

/**
 * \brief Performs name resolution pass on an AST binary greater-or-equal
 * comparison operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_ge_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_cmp_ge_t* node);

/**
 * \brief Performs type check pass on an AST binary greater-or-equal comparison
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_ge_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_cmp_ge_t* node);

/**
 * \brief Performs code generation pass on an AST binary greater-or-equal
 * comparison operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_cmp_ge_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_cmp_ge_t* node);

TAU_EXTERN_C_END

#endif
