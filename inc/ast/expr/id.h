/**
 * \file decl.h
 * 
 * \brief Abstract syntax tree identifier expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_ID_H
#define TAU_AST_EXPR_ID_H

#include "ast/expr/expr.h"

/**
 * \brief AST identifier expression node.
 */
typedef struct ast_expr_id_t
{
  AST_EXPR_HEADER;
  ast_node_t* decl; // The associated declaration node.
} ast_expr_id_t;

/**
 * \brief Initializes a new AST identifier expression node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_expr_id_t* ast_expr_id_init(void);

/**
 * \brief Frees all memory allocated by an AST identifier expression node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_expr_id_free(ast_expr_id_t* node);

/**
 * \brief Performs name resolution pass on an AST identifier expression node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_expr_id_nameres(nameres_ctx_t* ctx, ast_expr_id_t* node);

/**
 * \brief Performs type check pass on an AST identifier expression node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_expr_id_typecheck(typecheck_ctx_t* ctx, ast_expr_id_t* node);

/**
 * \brief Writes a JSON dump of an AST identifier expression node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_expr_id_dump_json(FILE* stream, ast_expr_id_t* node);

#endif