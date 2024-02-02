/**
 * \file
 * 
 * \brief Abstract syntax tree binary operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_BIN_H
#define TAU_AST_EXPR_OP_BIN_H

#include "ast/expr/op/op.h"

/**
 * \brief AST binary operation expression node.
 */
typedef struct ast_expr_op_bin_t
{
  AST_EXPR_OP_HEADER;
  ast_node_t* lhs; // The associated left-hand side expression node.
  ast_node_t* rhs; // The associated right-hand side expression node.
} ast_expr_op_bin_t;

/**
 * \brief Initializes a new AST binary operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_op_bin_t* ast_expr_op_bin_init(void);

/**
 * \brief Frees all memory allocated by an AST binary operation expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_expr_op_bin_free(ast_expr_op_bin_t* node);

/**
 * \brief Performs name resolution pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs type check pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Performs code generation pass on an AST binary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_op_bin_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node);

/**
 * \brief Writes a JSON dump of an AST binary operation expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_op_bin_dump_json(FILE* stream, ast_expr_op_bin_t* node);

#endif