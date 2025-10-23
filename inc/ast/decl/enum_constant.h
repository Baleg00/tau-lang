/**
 * \file
 * 
 * \brief Abstract syntax tree enum constant declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_ENUM_CONSTANT_H
#define TAU_AST_DECL_ENUM_CONSTANT_H

#include "ast/decl/decl.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST enum constant declaration node.
 */
typedef struct tau_ast_decl_enum_constant_t
{
  TAU_AST_DECL_HEADER;
  tau_ast_node_t* parent; // The associated parent enum declaration.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.
} tau_ast_decl_enum_constant_t;

/**
 * \brief Initializes a new AST enum constant declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_decl_enum_constant_t* tau_ast_decl_enum_constant_init(void);

/**
 * \brief Frees all memory allocated by an AST enum constant declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_decl_enum_constant_free(tau_ast_decl_enum_constant_t* node);

/**
 * \brief Performs name resolution pass on an AST enum constant declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_enum_constant_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_enum_constant_t* node);

/**
 * \brief Performs type check pass on an AST enum constant declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_enum_constant_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_enum_constant_t* node);

/**
 * \brief Performs code generation pass on an AST enum constant declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_enum_constant_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_enum_constant_t* node);

/**
 * \brief Writes a JSON dump of an AST enum constant declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_decl_enum_constant_dump_json(FILE* stream, tau_ast_decl_enum_constant_t* node);

TAU_EXTERN_C_END

#endif
