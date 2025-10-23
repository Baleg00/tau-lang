/**
 * \file
 *
 * \brief Abstract syntax tree literal vector expression node interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_VEC_H
#define TAU_AST_EXPR_LIT_VEC_H

#include "ast/expr/lit/lit.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST literal vector expression node.
 */
typedef struct tau_ast_expr_lit_vec_t
{
  TAU_AST_EXPR_LIT_HEADER;
  tau_vector_t* values;
} tau_ast_expr_lit_vec_t;

/**
 * \brief Initializes a new AST literal vector expression node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_expr_lit_vec_t* tau_ast_expr_lit_vec_init(void);

/**
 * \brief Frees all memory allocated by an AST literal vector expression node.
 *
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_expr_lit_vec_free(tau_ast_expr_lit_vec_t* node);

/**
 * \brief Performs name resolution pass on an AST literal vector expression node.
 *
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_lit_vec_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_lit_vec_t* node);

/**
 * \brief Performs type check pass on an AST literal vector expression node.
 *
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_lit_vec_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_lit_vec_t* node);

/**
 * \brief Performs code generation pass on an AST literal vector expression node.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_lit_vec_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_lit_vec_t* node);

/**
 * \brief Writes a JSON dump of an AST literal vector expression node into a stream.
 *
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_expr_lit_vec_dump_json(FILE* stream, tau_ast_expr_lit_vec_t* node);

TAU_EXTERN_C_END

#endif
