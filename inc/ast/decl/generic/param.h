/**
 * \file
 * 
 * \brief Abstract syntax tree generic parameter declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_GENERIC_PARAM_H
#define TAU_AST_DECL_GENERIC_PARAM_H

#include "ast/decl/decl.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST generic parameter declaration node.
 */
typedef struct tau_ast_decl_generic_param_t
{
  TAU_AST_DECL_HEADER;
  tau_ast_node_t* type; ///< Pointer to the associated type.
  tau_ast_node_t* expr; ///< Pointer to the associated default value expression, or NULL.
} tau_ast_decl_generic_param_t;

/**
 * \brief Initializes a new AST generic parameter declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_decl_generic_param_t* tau_ast_decl_generic_param_init(void);

/**
 * \brief Frees all memory allocated by an AST generic parameter declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_decl_generic_param_free(tau_ast_decl_generic_param_t* node);

/**
 * \brief Performs name resolution pass on an AST generic parameter declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_generic_param_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_generic_param_t* node);

/**
 * \brief Writes a JSON dump of an AST generic parameter declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_decl_generic_param_dump_json(FILE* stream, tau_ast_decl_generic_param_t* node);

TAU_EXTERN_C_END

#endif
