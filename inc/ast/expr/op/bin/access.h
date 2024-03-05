/**
 * \file
 * 
 * \brief Abstract syntax tree binary access operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_ACCESS_H
#define TAU_AST_EXPR_OP_BIN_ACCESS_H

#include "ast/expr/op/bin/bin.h"

/**
 * \brief AST binary access operation expression node.
 */
typedef struct ast_expr_op_bin_access_t
{
  AST_EXPR_OP_BIN_HEADER;
  ast_node_t* decl; // Pointer to the declaration that contains the accessed member.
  size_t idx; // The index of the accessed member in the containing declaration.
} ast_expr_op_bin_access_t;

/**
 * \brief Initializes a new AST binary access operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_bin_access_t* ast_expr_op_bin_access_init(void);

/**
 * \brief Performs name resolution pass on an AST binary access operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_access_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_access_t* node);

/**
 * \brief Performs type check pass on an AST binary access operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_access_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_access_t* node);

/**
 * \brief Performs code generation pass on an AST binary access operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_access_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_access_t* node);

#endif