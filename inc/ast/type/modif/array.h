/**
 * \file
 * 
 * \brief Abstract syntax tree array type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_ARRAY_H
#define TAU_AST_TYPE_ARRAY_H

#include "ast/type/modif/modif.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST array type node.
 */
typedef struct ast_type_array_t
{
  AST_TYPE_MODIFIER_HEADER;
  ast_node_t* size;
} ast_type_array_t;

/**
 * \brief Initializes a new AST array type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_array_t* ast_type_array_init(void);

/**
 * \brief Frees all memory allocated by an AST array type node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_type_array_free(ast_type_array_t* node);

/**
 * \brief Performs name resolution pass on an AST array type node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_type_array_nameres(nameres_ctx_t* ctx, ast_type_array_t* node);

/**
 * \brief Performs type check pass on an AST array type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_array_typecheck(typecheck_ctx_t* ctx, ast_type_array_t* node);

/**
 * \brief Performs code generation pass on an AST array type node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_type_array_codegen(codegen_ctx_t* ctx, ast_type_array_t* node);

/**
 * \brief Writes at most `len` characters (including the null-terminator) of the
 * mangled name of an AST array type node into `buf`.
 * 
 * \details If `len` is zero, `buf` and `len` are ignored and nothing is written,
 * however the return value is still calculated.
 * 
 * \param[in] node Pointer to the AST array type node.
 * \param[in,out] buf Pointer to the buffer where the mangled name is to be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters (excluding the null-terminator) that would
 * have been written to the buffer if `len` was ignored.
 */
size_t ast_type_array_mangle(ast_type_array_t* node, char* buf, size_t len);

/**
 * \brief Writes a JSON dump of an AST array type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_array_dump_json(FILE* stream, ast_type_array_t* node);

TAU_EXTERN_C_END

#endif
