/**
 * \file
 * 
 * \brief Abstract syntax tree generic parameter node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_GENERIC_PARAM_H
#define TAU_AST_GENERIC_PARAM_H

#include "ast/node.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST generic parameter node.
 */
typedef struct ast_generic_param_t
{
  AST_NODE_HEADER;
  ast_node_t* type; ///< Pointer to the associated type.
  ast_node_t* expr; ///< Pointer to the associated default value expression, or NULL.
} ast_generic_param_t;

/**
 * \brief Initializes a new AST generic parameter node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_generic_param_t* ast_generic_param_init(void);

/**
 * \brief Frees all memory allocated by an AST generic parameter node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_generic_param_free(ast_generic_param_t* node);

/**
 * \brief Performs name resolution pass on an AST generic parameter node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_generic_param_nameres(nameres_ctx_t* ctx, ast_generic_param_t* node);

/**
 * \brief Performs type check pass on an AST generic parameter node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_generic_param_typecheck(typecheck_ctx_t* ctx, ast_generic_param_t* node);

/**
 * \brief Performs code generation pass on an AST generic parameter node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_generic_param_codegen(codegen_ctx_t* ctx, ast_generic_param_t* node);

/**
 * \brief Writes a JSON dump of an AST generic parameter node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_generic_param_dump_json(FILE* stream, ast_generic_param_t* node);

TAU_EXTERN_C_END

#endif
