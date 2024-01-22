/**
 * \file opt.h
 * 
 * \brief Abstract syntax tree optional type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_OPT_H
#define TAU_AST_TYPE_OPT_H

#include "ast/type/modif.h"

/**
 * \brief AST optional type node.
 */
typedef struct ast_type_opt_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_opt_t;

/**
 * \brief Initializes a new AST optional type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_opt_t* ast_type_opt_init(void);

/**
 * \brief Frees all memory allocated by an AST optional type node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_type_opt_free(ast_type_opt_t* node);

/**
 * \brief Performs name resolution pass on an AST optional type node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_type_opt_nameres(nameres_ctx_t* ctx, ast_type_opt_t* node);

/**
 * \brief Performs type check pass on an AST optional type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_type_opt_typecheck(typecheck_ctx_t* ctx, ast_type_opt_t* node);

/**
 * \brief Writes a JSON dump of an AST optional type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_opt_dump_json(FILE* stream, ast_type_opt_t* node);

#endif