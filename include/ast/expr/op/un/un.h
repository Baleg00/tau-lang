/**
 * \file
 * 
 * \brief Abstract syntax tree unary operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_UN_H
#define TAU_AST_EXPR_OP_UN_H

#include "ast/expr/op/op.h"

/**
 * \brief Header for all AST unary operation expression nodes.
 */
#define TAU_AST_EXPR_OP_UN_HEADER\
  TAU_AST_EXPR_OP_HEADER;\
  tau_ast_node_t* expr /** The associated expression node. */

TAU_EXTERN_C_BEGIN

/**
 * \brief AST unary operation expression node.
 */
typedef struct tau_ast_expr_op_un_t
{
  TAU_AST_EXPR_OP_UN_HEADER;
} tau_ast_expr_op_un_t;

/**
 * \brief Initializes a new AST unary operation expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_expr_op_un_t* tau_ast_expr_op_un_init(void);

/**
 * \brief Frees all memory allocated by an AST unary operation expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_expr_op_un_free(tau_ast_expr_op_un_t* node);

/**
 * \brief Performs name resolution pass on an AST unary operation expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_un_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_t* node);

/**
 * \brief Performs type check pass on an AST unary operation expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_un_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_t* node);

/**
 * \brief Performs code generation pass on an AST unary operation expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_op_un_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_t* node);

/**
 * \brief Writes a JSON dump of an AST unary operation expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_expr_op_un_dump_json(FILE* stream, tau_ast_expr_op_un_t* node);

TAU_EXTERN_C_END

#endif
