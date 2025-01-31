/**
 * \file
 *
 * \brief Abstract syntax tree vector type node interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef AST_TYPE_VEC_H
#define AST_TYPE_VEC_H

#include "ast/type/type.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST vector type node.
 */
typedef struct ast_type_vec_t
{
  AST_TYPE_HEADER;
  bool is_row;           ///< `true` if the vector is a row vector, `false` otherwise.
  size_t size;           ///< The size of the vector.
  ast_node_t* base_type; ///< The primitive base type of the vector.
} ast_type_vec_t;

/**
 * \brief Initializes a new AST vector type node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_vec_t* ast_type_vec_init(void);

/**
 * \brief Frees all memory allocated by an AST vector type node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_type_vec_free(ast_type_vec_t* node);

/**
 * \brief Performs name resolution pass on an AST vector type node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_vec_nameres(nameres_ctx_t* ctx, ast_type_vec_t* node);

/**
 * \brief Performs type check pass on an AST vector type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_vec_typecheck(typecheck_ctx_t* ctx, ast_type_vec_t* node);

/**
 * \brief Performs type check pass on an AST vector type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_vec_codegen(codegen_ctx_t* ctx, ast_type_vec_t* node);

/**
 * \brief Writes a JSON dump of an AST vector type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_vec_dump_json(FILE* stream, ast_type_vec_t* node);

TAU_EXTERN_C_END

#endif
