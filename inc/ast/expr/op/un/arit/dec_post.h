/**
 * \file
 * 
 * \brief Abstract syntax tree unary post-decrement operation expression node
 * interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_UN_DEC_POST_H
#define TAU_AST_EXPR_OP_UN_DEC_POST_H

#include "ast/expr/op/un/un.h"

/**
 * \brief AST unary post-decrement operation expression node.
 */
typedef struct ast_expr_op_un_arit_dec_post_t
{
  AST_EXPR_OP_UN_HEADER;
} ast_expr_op_un_arit_dec_post_t;

/**
 * \brief Initializes a new AST unary post-decrement operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_un_arit_dec_post_t* ast_expr_op_un_arit_dec_post_init(void);

/**
 * \brief Performs name resolution pass on an AST unary post-decrement operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_arit_dec_post_nameres(nameres_ctx_t* ctx, ast_expr_op_un_arit_dec_post_t* node);

/**
 * \brief Performs type check pass on an AST unary post-decrement operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_arit_dec_post_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_arit_dec_post_t* node);

/**
 * \brief Performs code generation pass on an AST unary post-decrement operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_arit_dec_post_codegen(codegen_ctx_t* ctx, ast_expr_op_un_arit_dec_post_t* node);

#endif