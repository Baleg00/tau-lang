/**
 * \file
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

TAU_EXTERN_C_BEGIN

/**
 * \brief AST function declaration node.
 */
typedef struct tau_ast_decl_fun_t
{
  TAU_AST_DECL_HEADER;
  tau_symtable_t* scope;            // The associated scope of parameters and function body.
  tau_ast_node_t* parent;           // The associated parent module declaration.
  tau_vector_t* params;             // Vector of associated parameter declarations.
  tau_ast_node_t* return_type;      // The associated return type.
  tau_ast_node_t* stmt;             // The associated body statement.
  tau_callconv_kind_t callconv;     // The associated calling convention.
  bool is_vararg;               // Is function variadic (C-style, only works with specific calling conventions).
  bool is_extern;               // Is function external.

  LLVMTypeRef llvm_type;        // Reference to the associated LLVM function type.
  LLVMValueRef llvm_value;      // Reference to the associated LLVM function value.
  LLVMBasicBlockRef llvm_entry; // Reference to the associated LLVM entry block.
  LLVMBasicBlockRef llvm_end;   // Reference to the associated LLVM end block.
} tau_ast_decl_fun_t;

/**
 * \brief Initializes a new AST function declaration node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_decl_fun_t* tau_ast_decl_fun_init(void);

/**
 * \brief Frees all memory allocated by an AST function declaration node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_decl_fun_free(tau_ast_decl_fun_t* node);

/**
 * \brief Performs name resolution pass on an AST function declaration node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_fun_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_fun_t* node);

/**
 * \brief Performs type check pass on an AST function declaration node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_fun_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_fun_t* node);

/**
 * \brief Performs control flow analysis pass on an AST function declaration node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_fun_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_decl_fun_t* node);

/**
 * \brief Performs code generation pass on an AST function declaration node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_decl_fun_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_fun_t* node);

/**
 * \brief Writes a JSON dump of an AST function declaration node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_decl_fun_dump_json(FILE* stream, tau_ast_decl_fun_t* node);

TAU_EXTERN_C_END

#endif
