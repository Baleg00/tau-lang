/**
 * \file
 * 
 * \brief Abstract syntax tree literal string expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_STR_H
#define TAU_AST_EXPR_LIT_STR_H

#include "ast/expr/lit/lit.h"

/**
 * \brief AST literal string expression node.
 */
typedef struct ast_expr_lit_str_t
{
  AST_EXPR_LIT_HEADER;
  char* value; // The associated string value.
} ast_expr_lit_str_t;

/**
 * \brief Initializes a new AST literal string expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_lit_str_t* ast_expr_lit_str_init(void);

/**
 * \brief Frees all memory allocated by an AST literal string expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_expr_lit_str_free(ast_expr_lit_str_t* node);

/**
 * \brief Performs name resolution pass on an AST literal string expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_str_nameres(nameres_ctx_t* ctx, ast_expr_lit_str_t* node);

/**
 * \brief Performs type check pass on an AST literal string expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_str_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_str_t* node);

/**
 * \brief Performs code generation pass on an AST literal string expression node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_lit_str_codegen(codegen_ctx_t* ctx, ast_expr_lit_str_t* node);

/**
 * \brief Writes a JSON dump of an AST literal string expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_lit_str_dump_json(FILE* stream, ast_expr_lit_str_t* node);

#endif