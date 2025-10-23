/**
 * \file
 * 
 * \brief Abstract syntax tree literal character expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_CHAR_H
#define TAU_AST_EXPR_LIT_CHAR_H

#include "ast/expr/lit/lit.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST literal character expression node.
 */
typedef struct tau_ast_expr_lit_char_t
{
  TAU_AST_EXPR_LIT_HEADER;
  uint32_t value; // The associated character value.
} tau_ast_expr_lit_char_t;

/**
 * \brief Initializes a new AST literal character expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_expr_lit_char_t* tau_ast_expr_lit_char_init(void);

/**
 * \brief Frees all memory allocated by an AST literal character expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_expr_lit_char_free(tau_ast_expr_lit_char_t* node);

/**
 * \brief Performs name resolution pass on an AST literal character expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_lit_char_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_lit_char_t* node);

/**
 * \brief Performs type check pass on an AST literal character expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_lit_char_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_lit_char_t* node);

/**
 * \brief Performs code generation pass on an AST literal character expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_expr_lit_char_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_lit_char_t* node);

/**
 * \brief Writes a JSON dump of an AST literal character expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_expr_lit_char_dump_json(FILE* stream, tau_ast_expr_lit_char_t* node);

TAU_EXTERN_C_END

#endif
