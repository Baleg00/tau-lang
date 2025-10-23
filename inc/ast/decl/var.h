/**
 * \file
 * 
 * \brief Abstract syntax tree variable declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_VAR_H
#define TAU_AST_DECL_VAR_H

#include "ast/decl/decl.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST variable declaration node.
 */
typedef struct tau_ast_decl_var_t
{
  TAU_AST_DECL_HEADER;
  tau_ast_node_t* type; // The associated type.
  tau_ast_node_t* expr; // The associated initializer expression.
  bool is_extern; // Is variable external.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.
} tau_ast_decl_var_t;

/**
 * \brief Initializes a new AST variable declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_decl_var_t* tau_ast_decl_var_init(void);

/**
 * \brief Frees all memory allocated by an AST variable declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_decl_var_free(tau_ast_decl_var_t* node);

/**
 * \brief Performs name resolution pass on an AST variable declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_var_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_var_t* node);

/**
 * \brief Performs type check pass on an AST variable declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_var_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_var_t* node);

/**
 * \brief Performs code generation pass on an AST variable declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_var_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_var_t* node);

/**
 * \brief Writes a JSON dump of an AST variable declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_decl_var_dump_json(FILE* stream, tau_ast_decl_var_t* node);

TAU_EXTERN_C_END

#endif
