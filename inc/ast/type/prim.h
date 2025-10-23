/**
 * \file
 * 
 * \brief Abstract syntax tree primitive type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_PRIM_H
#define TAU_AST_TYPE_PRIM_H

#include "ast/type/type.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST primitive type node.
 */
typedef struct tau_ast_type_prim_t
{
  TAU_AST_TYPE_HEADER;
} tau_ast_type_prim_t;

/**
 * \brief Initializes a new AST primitive `i8` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_i8_init(void);

/**
 * \brief Initializes a new AST primitive `i16` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_i16_init(void);

/**
 * \brief Initializes a new AST primitive `i32` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_i32_init(void);

/**
 * \brief Initializes a new AST primitive `i64` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_i64_init(void);

/**
 * \brief Initializes a new AST primitive `isize` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_isize_init(void);

/**
 * \brief Initializes a new AST primitive `u8` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_u8_init(void);

/**
 * \brief Initializes a new AST primitive `u16` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_u16_init(void);

/**
 * \brief Initializes a new AST primitive `u32` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_u32_init(void);

/**
 * \brief Initializes a new AST primitive `u64` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_u64_init(void);

/**
 * \brief Initializes a new AST primitive `usize` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_usize_init(void);

/**
 * \brief Initializes a new AST primitive `f32` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_f32_init(void);

/**
 * \brief Initializes a new AST primitive `f64` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_f64_init(void);

/**
 * \brief Initializes a new AST primitive `c64` type node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_c64_init(void);

/**
 * \brief Initializes a new AST primitive `c128` type node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_c128_init(void);

/**
 * \brief Initializes a new AST primitive `char` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_char_init(void);

/**
 * \brief Initializes a new AST primitive `bool` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_bool_init(void);

/**
 * \brief Initializes a new AST primitive `unit` type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_prim_t* tau_ast_type_prim_unit_init(void);

/**
 * \brief Frees all memory allocated by an AST primitive type node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_type_prim_free(tau_ast_type_prim_t* node);

/**
 * \brief Performs name resolution pass on an AST primitive type node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_type_prim_nameres(tau_nameres_ctx_t* ctx, tau_ast_type_prim_t* node);

/**
 * \brief Performs type check pass on an AST primitive type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_type_prim_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_prim_t* node);

/**
 * \brief Performs type check pass on an AST primitive type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_type_prim_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_prim_t* node);

/**
 * \brief Writes a JSON dump of an AST primitive type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_type_prim_dump_json(FILE* stream, tau_ast_type_prim_t* node);

TAU_EXTERN_C_END

#endif
