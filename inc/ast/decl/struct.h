/**
 * \file
 * 
 * \brief Abstract syntax tree struct declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_STRUCT_H
#define TAU_AST_DECL_STRUCT_H

#include "ast/decl/decl.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST struct declaration node.
 */
typedef struct ast_decl_struct_t
{
  AST_DECL_HEADER;
  symtable_t* scope; // The associated scope of members.
  ast_node_t* parent; // The associated parent module declaration.
  vector_t* members; // Vector of associated member declarations.

  LLVMTypeRef llvm_type; // The associated LLVM type.
} ast_decl_struct_t;

/**
 * \brief Initializes a new AST struct declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_struct_t* ast_decl_struct_init(void);

/**
 * \brief Frees all memory allocated by an AST struct declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_decl_struct_free(ast_decl_struct_t* node);

/**
 * \brief Performs name resolution pass on an AST struct declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_struct_nameres(nameres_ctx_t* ctx, ast_decl_struct_t* node);

/**
 * \brief Performs type check pass on an AST struct declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_decl_struct_typecheck(typecheck_ctx_t* ctx, ast_decl_struct_t* node);

/**
 * \brief Performs code generation pass on an AST struct declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_struct_codegen(codegen_ctx_t* ctx, ast_decl_struct_t* node);

/**
 * \brief Writes a JSON dump of an AST struct declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_struct_dump_json(FILE* stream, ast_decl_struct_t* node);

TAU_EXTERN_C_END

#endif
