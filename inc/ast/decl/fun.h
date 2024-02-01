/**
 * \file fun.h
 * 
 * \brief Abstract syntax tree function declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_FUN_H
#define TAU_AST_DECL_FUN_H

#include "ast/callconv.h"
#include "ast/decl/decl.h"
#include "llvm.h"

/**
 * \brief AST function declaration node.
 */
typedef struct ast_decl_fun_t
{
  AST_DECL_HEADER;
  symtable_t* scope; // The associated scope of parameters and funciton body.
  ast_node_t* parent; // The associated parent module declaration.
  vector_t* params; // Vector of associated parameter declarations.
  ast_node_t* return_type; // The associated return type.
  ast_node_t* stmt; // The associated body statement.
  callconv_kind_t callconv; // The associated calling convention.
  bool is_pub; // Is function public (in case it is a member).
  bool is_vararg; // Is function variadic (C-style, only works with specific calling conventions).
  bool is_extern; // Is function external.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.

  LLVMBasicBlockRef llvm_entry; // LLVM block for function entry.
  LLVMBasicBlockRef llvm_end; // LLVM block for function end.
} ast_decl_fun_t;

/**
 * \brief Initializes a new AST function declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_fun_t* ast_decl_fun_init(void);

/**
 * \brief Frees all memory allocated by an AST function declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_decl_fun_free(ast_decl_fun_t* node);

/**
 * \brief Performs name resolution pass on an AST function declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_fun_nameres(nameres_ctx_t* ctx, ast_decl_fun_t* node);

/**
 * \brief Performs type check pass on an AST function declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_decl_fun_typecheck(typecheck_ctx_t* ctx, ast_decl_fun_t* node);

/**
 * \brief Performs code generation pass on an AST function declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_fun_codegen(codegen_ctx_t* ctx, ast_decl_fun_t* node);

/**
 * \brief Writes at most `len` characters (including the null-terminator) of the
 * mangled name of an AST function declaration node into `buf`.
 * 
 * \details If `len` is zero, `buf` and `len` are ignored and nothing is written,
 * however the return value is still calculated.
 * 
 * \param[in] node Pointer to the AST function declaration node.
 * \param[in,out] buf Pointer to the buffer where the mangled name is to be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters (excluding the null-terminator) that would
 * have been written to the buffer if `len` was ignored.
 */
size_t ast_decl_fun_mangle(ast_decl_fun_t* node, char* buf, size_t len);

/**
 * \brief Writes a JSON dump of an AST function declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_fun_dump_json(FILE* stream, ast_decl_fun_t* node);

#endif