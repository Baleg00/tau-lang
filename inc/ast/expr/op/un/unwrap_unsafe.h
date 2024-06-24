/**
 * \file
 * 
 * \brief Abstract syntax tree unary unsafe optional unwrap operation expression
 * node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_UN_UNWRAP_UNSAFE_H
#define TAU_AST_EXPR_OP_UN_UNWRAP_UNSAFE_H

#include "ast/expr/op/un/un.h"

/**
 * \brief AST unary unsafe optional unwrap operation expression node.
 */
typedef struct ast_expr_op_un_unwrap_unsafe_t
{
  AST_EXPR_OP_UN_HEADER;
} ast_expr_op_un_unwrap_unsafe_t;

/**
 * \brief Initializes a new AST unary unsafe optional unwrap operation expression
 * node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_un_unwrap_unsafe_t* ast_expr_op_un_unwrap_unsafe_init(void);

/**
 * \brief Performs name resolution pass on an AST unary unsafe optional unwrap
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_unwrap_unsafe_nameres(nameres_ctx_t* ctx, ast_expr_op_un_unwrap_unsafe_t* node);

/**
 * \brief Performs type check pass on an AST unary unsafe optional unwrap operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_unwrap_unsafe_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_unwrap_unsafe_t* node);

/**
 * \brief Performs code generation pass on an AST unary unsafe optional unwrap
 * operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_unwrap_unsafe_codegen(codegen_ctx_t* ctx, ast_expr_op_un_unwrap_unsafe_t* node);

#endif