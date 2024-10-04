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
#include "llvm.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST union declaration node.
 */
typedef struct ast_decl_union_t
{
  AST_DECL_HEADER;
  symtable_t* scope; // The associated scope of members.
  ast_node_t* parent; // The associated parent module declaration.
  vector_t* members; // Vector of associated member declarations.

  LLVMTypeRef llvm_type; // The associated LLVM type (same as the type of the largest member).
} ast_decl_union_t;

/**
 * \brief Initializes a new AST union declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_union_t* ast_decl_union_init(void);

/**
 * \brief Frees all memory allocated by an AST union declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_decl_union_free(ast_decl_union_t* node);

/**
 * \brief Performs name resolution pass on an AST union declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_union_nameres(nameres_ctx_t* ctx, ast_decl_union_t* node);

/**
 * \brief Performs type check pass on an AST union declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_decl_union_typecheck(typecheck_ctx_t* ctx, ast_decl_union_t* node);

/**
 * \brief Performs code generation pass on an AST union declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_union_codegen(codegen_ctx_t* ctx, ast_decl_union_t* node);

/**
 * \brief Writes at most `len` characters (including the null-terminator) of the
 * mangled name of an AST union declaration node into `buf`.
 * 
 * \details If `len` is zero, `buf` and `len` are ignored and nothing is written,
 * however the return value is still calculated.
 * 
 * \param[in] node Pointer to the AST union declaration node.
 * \param[in,out] buf Pointer to the buffer where the mangled name is to be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters (excluding the null-terminator) that would
 * have been written to the buffer if `len` was ignored.
 */
size_t ast_decl_union_mangle(ast_decl_union_t* node, char* buf, size_t len);

/**
 * \brief Writes a JSON dump of an AST union declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_union_dump_json(FILE* stream, ast_decl_union_t* node);

TAU_EXTERN_C_END

#endif
