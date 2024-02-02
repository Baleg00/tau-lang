/**
 * \file
 * 
 * \brief Abstract syntax tree pointer type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_PTR_H
#define TAU_AST_TYPE_PTR_H

#include "ast/type/modif.h"

/**
 * \brief AST pointer type node.
 */
typedef struct ast_type_ptr_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_ptr_t;

/**
 * \brief Initializes a new AST pointer type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_ptr_t* ast_type_ptr_init(void);

/**
 * \brief Frees all memory allocated by an AST pointer type node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_type_ptr_free(ast_type_ptr_t* node);

/**
 * \brief Performs name resolution pass on an AST pointer type node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_type_ptr_nameres(nameres_ctx_t* ctx, ast_type_ptr_t* node);

/**
 * \brief Performs type check pass on an AST pointer type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_ptr_typecheck(typecheck_ctx_t* ctx, ast_type_ptr_t* node);

/**
 * \brief Performs code generation pass on an AST pointer type node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_type_ptr_codegen(codegen_ctx_t* ctx, ast_type_ptr_t* node);

/**
 * \brief Writes at most `len` characters (including the null-terminator) of the
 * mangled name of an AST pointer type node into `buf`.
 * 
 * \details If `len` is zero, `buf` and `len` are ignored and nothing is written,
 * however the return value is still calculated.
 * 
 * \param[in] node Pointer to the AST pointer type node.
 * \param[in,out] buf Pointer to the buffer where the mangled name is to be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters (excluding the null-terminator) that would
 * have been written to the buffer if `len` was ignored.
 */
size_t ast_type_ptr_mangle(ast_type_ptr_t* node, char* buf, size_t len);

/**
 * \brief Writes a JSON dump of an AST pointer type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_ptr_dump_json(FILE* stream, ast_type_ptr_t* node);

#endif