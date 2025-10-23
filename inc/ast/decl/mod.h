/**
 * \file
 * 
 * \brief Abstract syntax tree module declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_MOD_H
#define TAU_AST_DECL_MOD_H

#include "ast/decl/decl.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST module declaration node.
 */
typedef struct tau_ast_decl_mod_t
{
  TAU_AST_DECL_HEADER;
  tau_symtable_t* scope; // The associated scope of members.
  tau_ast_node_t* parent; // The associated parent module declaration.
  tau_vector_t* members; // Vector of associated member declarations.
} tau_ast_decl_mod_t;

/**
 * \brief Initializes a new AST module declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_decl_mod_t* tau_ast_decl_mod_init(void);

/**
 * \brief Frees all memory allocated by an AST module declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_decl_mod_free(tau_ast_decl_mod_t* node);

/**
 * \brief Performs name resolution pass on an AST module declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_mod_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_mod_t* node);

/**
 * \brief Performs type check pass on an AST module declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_mod_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_mod_t* node);

/**
 * \brief Performs control flow analysis pass on an AST module declaration node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_mod_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_decl_mod_t* node);

/**
 * \brief Performs code generation pass on an AST module declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_mod_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_mod_t* node);

/**
 * \brief Writes a JSON dump of an AST module declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_decl_mod_dump_json(FILE* stream, tau_ast_decl_mod_t* node);

TAU_EXTERN_C_END

#endif
