/**
 * \file prim.h
 * 
 * \brief Abstract syntax tree primitive type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_PRIM_H
#define TAU_AST_TYPE_PRIM_H

#include "ast/type/type.h"

/**
 * \brief AST primitive type node.
 */
typedef struct ast_type_prim_t
{
  AST_TYPE_HEADER;
} ast_type_prim_t;

/**
 * \brief Initializes a new AST primitive `i8` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_i8_init(void);

/**
 * \brief Initializes a new AST primitive `i16` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_i16_init(void);

/**
 * \brief Initializes a new AST primitive `i32` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_i32_init(void);

/**
 * \brief Initializes a new AST primitive `i64` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_i64_init(void);

/**
 * \brief Initializes a new AST primitive `isize` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_isize_init(void);

/**
 * \brief Initializes a new AST primitive `u8` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_u8_init(void);

/**
 * \brief Initializes a new AST primitive `u16` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_u16_init(void);

/**
 * \brief Initializes a new AST primitive `u32` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_u32_init(void);

/**
 * \brief Initializes a new AST primitive `u64` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_u64_init(void);

/**
 * \brief Initializes a new AST primitive `usize` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_usize_init(void);

/**
 * \brief Initializes a new AST primitive `f32` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_f32_init(void);

/**
 * \brief Initializes a new AST primitive `f64` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_f64_init(void);

/**
 * \brief Initializes a new AST primitive `char` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_char_init(void);

/**
 * \brief Initializes a new AST primitive `bool` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_bool_init(void);

/**
 * \brief Initializes a new AST primitive `unit` type node.
 * 
 * \param[in] kind The primitive type kind.
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_prim_t* ast_type_prim_unit_init(void);

/**
 * \brief Frees all memory allocated by an AST primitive type node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_type_prim_free(ast_type_prim_t* node);

/**
 * \brief Performs name resolution pass on an AST primitive type node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_prim_nameres(nameres_ctx_t* ctx, ast_type_prim_t* node);

/**
 * \brief Performs type check pass on an AST primitive type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_prim_typecheck(typecheck_ctx_t* ctx, ast_type_prim_t* node);

/**
 * \brief Performs type check pass on an AST primitive type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_prim_codegen(codegen_ctx_t* ctx, ast_type_prim_t* node);

/**
 * \brief Writes at most `len` characters (including the null-terminator) of the
 * mangled name of an AST primitive type node into `buf`.
 * 
 * \details If `buf` is NULL or `len` is zero, both `buf` and `len` are ignored
 * and nothing is written, however the return value is still calculated.
 * 
 * \param[in] node Pointer to the AST primitive type node.
 * \param[in,out] buf Pointer to the buffer where the mangled name is to be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters (excluding the null-terminator) that would
 * have been written to the buffer if `len` was ignored.
 */
size_t ast_type_prim_mangle(ast_type_prim_t* node, char* buf, size_t len);

/**
 * \brief Writes a JSON dump of an AST primitive type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_prim_dump_json(FILE* stream, ast_type_prim_t* node);

#endif