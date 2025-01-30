/**
 * \file
 *
 * \brief Abstract syntax tree literal matrix expression node interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_MAT_H
#define TAU_AST_EXPR_LIT_MAT_H

#include "ast/expr/lit/lit.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST literal matrix expression node.
 */
typedef struct ast_expr_lit_mat_t
{
  AST_EXPR_LIT_HEADER;
  size_t rows;
  size_t cols;
  vector_t* values;
} ast_expr_lit_mat_t;

/**
 * \brief Initializes a new AST literal matrix expression node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_lit_mat_t* ast_expr_lit_mat_init(void);

/**
 * \brief Frees all memory allocated by an AST literal matrix expression node.
 *
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_expr_lit_mat_free(ast_expr_lit_mat_t* node);

/**
 * \brief Performs name resolution pass on an AST literal matrix expression node.
 *
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_mat_nameres(nameres_ctx_t* ctx, ast_expr_lit_mat_t* node);

/**
 * \brief Performs type check pass on an AST literal matrix expression node.
 *
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_mat_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_mat_t* node);

/**
 * \brief Performs code generation pass on an AST literal matrix expression node.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_mat_codegen(codegen_ctx_t* ctx, ast_expr_lit_mat_t* node);

/**
 * \brief Writes a JSON dump of an AST literal matrix expression node into a stream.
 *
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_lit_mat_dump_json(FILE* stream, ast_expr_lit_mat_t* node);

TAU_EXTERN_C_END

#endif
