/**
 * \file
 * 
 * \brief Abstract syntax tree union declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_UNION_H
#define TAU_AST_DECL_UNION_H

#include "ast/decl/decl.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST union declaration node.
 */
typedef struct tau_ast_decl_union_t
{
  TAU_AST_DECL_HEADER;
  tau_symtable_t* scope; // The associated scope of members.
  tau_ast_node_t* parent; // The associated parent module declaration.
  tau_vector_t* members; // Vector of associated member declarations.

  LLVMTypeRef llvm_type; // The associated LLVM type (same as the type of the largest member).
} tau_ast_decl_union_t;

/**
 * \brief Initializes a new AST union declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_decl_union_t* tau_ast_decl_union_init(void);

/**
 * \brief Frees all memory allocated by an AST union declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_decl_union_free(tau_ast_decl_union_t* node);

/**
 * \brief Performs name resolution pass on an AST union declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_union_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_union_t* node);

/**
 * \brief Performs type check pass on an AST union declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_union_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_union_t* node);

/**
 * \brief Performs code generation pass on an AST union declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_union_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_union_t* node);

/**
 * \brief Writes a JSON dump of an AST union declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_decl_union_dump_json(FILE* stream, tau_ast_decl_union_t* node);

TAU_EXTERN_C_END

#endif
