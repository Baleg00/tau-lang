/**
 * \file
 * 
 * \brief Abstract syntax tree literal boolean expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_BOOL_H
#define TAU_AST_EXPR_LIT_BOOL_H

#include "ast/expr/lit/lit.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST literal boolean expression node.
 */
typedef struct ast_expr_lit_bool_t
{
  AST_EXPR_LIT_HEADER;
  bool value; // The associated boolean value.
} ast_expr_lit_bool_t;

/**
 * \brief Initializes a new AST literal boolean expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_lit_bool_t* ast_expr_lit_bool_init(void);

/**
 * \brief Frees all memory allocated by an AST literal boolean expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_expr_lit_bool_free(ast_expr_lit_bool_t* node);

/**
 * \brief Performs name resolution pass on an AST literal boolean expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_bool_nameres(nameres_ctx_t* ctx, ast_expr_lit_bool_t* node);

/**
 * \brief Performs type check pass on an AST literal boolean expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_bool_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_bool_t* node);

/**
 * \brief Performs code generation pass on an AST literal boolean expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_bool_codegen(codegen_ctx_t* ctx, ast_expr_lit_bool_t* node);

/**
 * \brief Writes a JSON dump of an AST literal boolean expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_lit_bool_dump_json(FILE* stream, ast_expr_lit_bool_t* node);

TAU_EXTERN_C_END

#endif
