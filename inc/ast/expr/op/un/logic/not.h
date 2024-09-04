/**
 * \file
 * 
 * \brief Abstract syntax tree unary logical NOT operation expression node
 * interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_UN_LOGIC_NOT_H
#define TAU_AST_EXPR_OP_UN_LOGIC_NOT_H

#include "ast/expr/op/un/un.h"

/**
 * \brief AST unary logical NOT operation expression node.
 */
typedef struct ast_expr_op_un_logic_not_t
{
  AST_EXPR_OP_UN_HEADER;
} ast_expr_op_un_logic_not_t;

/**
 * \brief Initializes a new AST unary logical NOT operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_un_logic_not_t* ast_expr_op_un_logic_not_init(void);

/**
 * \brief Performs name resolution pass on an AST unary logical NOT operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_logic_not_nameres(nameres_ctx_t* ctx, ast_expr_op_un_logic_not_t* node);

/**
 * \brief Performs type check pass on an AST unary logical NOT operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_logic_not_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_logic_not_t* node);

/**
 * \brief Performs code generation pass on an AST unary logical NOT operation
 * expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_un_logic_not_codegen(codegen_ctx_t* ctx, ast_expr_op_un_logic_not_t* node);

#endif
