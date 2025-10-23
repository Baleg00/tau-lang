/**
 * \file
 *
 * \brief Abstract syntax tree matrix type node interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_MAT_H
#define TAU_AST_TYPE_MAT_H

#include "ast/type/type.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST matrix type node.
 */
typedef struct tau_ast_type_mat_t
{
  TAU_AST_TYPE_HEADER;
  size_t rows;           ///< The number of rows in the matrix.
  size_t cols;           ///< The number of columns in the matrix.
  tau_ast_node_t* base_type; ///< The primitive base type of the matrix.
} tau_ast_type_mat_t;

/**
 * \brief Initializes a new AST matrix type node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_mat_t* tau_ast_type_mat_init(void);

/**
 * \brief Frees all memory allocated by an AST matrix type node.
 *
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_type_mat_free(tau_ast_type_mat_t* node);

/**
 * \brief Performs name resolution pass on an AST matrix type node.
 *
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_type_mat_nameres(tau_nameres_ctx_t* ctx, tau_ast_type_mat_t* node);

/**
 * \brief Performs type check pass on an AST matrix type node.
 *
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_type_mat_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_mat_t* node);

/**
 * \brief Performs type check pass on an AST matrix type node.
 *
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_type_mat_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_mat_t* node);

/**
 * \brief Writes a JSON dump of an AST matrix type node into a stream.
 *
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_type_mat_dump_json(FILE* stream, tau_ast_type_mat_t* node);

TAU_EXTERN_C_END

#endif
