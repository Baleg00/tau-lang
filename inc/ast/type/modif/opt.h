/**
 * \file
 * 
 * \brief Abstract syntax tree optional type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_OPT_H
#define TAU_AST_TYPE_OPT_H

#include "ast/type/modif/modif.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST optional type node.
 */
typedef struct tau_ast_type_opt_t
{
  TAU_AST_TYPE_MODIFIER_HEADER;
} tau_ast_type_opt_t;

/**
 * \brief Initializes a new AST optional type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_type_opt_t* tau_ast_type_opt_init(void);

/**
 * \brief Frees all memory allocated by an AST optional type node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_type_opt_free(tau_ast_type_opt_t* node);

/**
 * \brief Performs name resolution pass on an AST optional type node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_type_opt_nameres(tau_nameres_ctx_t* ctx, tau_ast_type_opt_t* node);

/**
 * \brief Performs type check pass on an AST optional type node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_type_opt_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_opt_t* node);

/**
 * \brief Performs code generation pass on an AST optional type node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_type_opt_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_opt_t* node);

/**
 * \brief Writes a JSON dump of an AST optional type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_type_opt_dump_json(FILE* stream, tau_ast_type_opt_t* node);

TAU_EXTERN_C_END

#endif
