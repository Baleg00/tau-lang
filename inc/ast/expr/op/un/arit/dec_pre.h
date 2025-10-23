/**
 * \file
 * 
 * \brief Abstract syntax tree unary pre-decrement operation expression node
 * interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_UN_DEC_PRE_H
#define TAU_AST_EXPR_OP_UN_DEC_PRE_H

#include "ast/expr/op/un/un.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST unary pre-decrement operation expression node.
 */
typedef struct tau_ast_expr_op_un_arit_dec_pre_t
{
  TAU_AST_EXPR_OP_UN_HEADER;
} tau_ast_expr_op_un_arit_dec_pre_t;

/**
 * \brief Initializes a new AST unary pre-decrement operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_expr_op_un_arit_dec_pre_t* tau_ast_expr_op_un_arit_dec_pre_init(void);

/**
 * \brief Performs name resolution pass on an AST unary pre-decrement operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_un_arit_dec_pre_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_arit_dec_pre_t* node);

/**
 * \brief Performs type check pass on an AST unary pre-decrement operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_un_arit_dec_pre_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_arit_dec_pre_t* node);

/**
 * \brief Performs code generation pass on an AST unary pre-decrement operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_un_arit_dec_pre_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_arit_dec_pre_t* node);

TAU_EXTERN_C_END

#endif
