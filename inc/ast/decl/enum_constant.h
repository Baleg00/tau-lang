/**
 * \file enum_constant.h
 * 
 * \brief Abstract syntax tree enum constant declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_ENUM_CONSTANT_H
#define TAU_AST_DECL_ENUM_CONSTANT_H

#include <llvm-c/Types.h>

#include "ast/decl/decl.h"

/**
 * \brief AST enum constant declaration node.
 */
typedef struct ast_decl_enum_constant_t
{
  AST_DECL_HEADER;
  ast_node_t* type; // The associated enum type.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.
} ast_decl_enum_constant_t;

/**
 * \brief Initializes a new AST enum constant declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_enum_constant_t* ast_decl_enum_constant_init(void);

/**
 * \brief Frees all memory allocated by an AST enum constant declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_decl_enum_constant_free(ast_decl_enum_constant_t* node);

/**
 * \brief Performs name resolution pass on an AST enum constant declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_enum_constant_nameres(nameres_ctx_t* ctx, ast_decl_enum_constant_t* node);

/**
 * \brief Performs type check pass on an AST enum constant declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_decl_enum_constant_typecheck(typecheck_ctx_t* ctx, ast_decl_enum_constant_t* node);

/**
 * \brief Performs code generation pass on an AST enum constant declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_enum_constant_codegen(codegen_ctx_t* ctx, ast_decl_enum_constant_t* node);

/**
 * \brief Writes a JSON dump of an AST enum constant declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_enum_constant_dump_json(FILE* stream, ast_decl_enum_constant_t* node);

#endif