/**
 * \file
 * 
 * \brief Abstract syntax tree if statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_IF_H
#define TAU_AST_STMT_IF_H

#include "ast/stmt/stmt.h"
#include "llvm.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST if statement node.
 */
typedef struct ast_stmt_if_t
{
  AST_STMT_HEADER;
  symtable_t* scope; // The associated scope.
  ast_node_t* cond; // The associated condition expression.
  ast_node_t* stmt; // The associated consequent statement.
  ast_node_t* stmt_else; // The associated optional alternative statement.

  LLVMBasicBlockRef llvm_then; // LLVM block for the consequent branch.
  LLVMBasicBlockRef llvm_else; // LLVM block for the alternative branch (if present).
  LLVMBasicBlockRef llvm_end; // LLVM block for the end of the if statement.
} ast_stmt_if_t;

/**
 * \brief Initializes a new AST if statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_if_t* ast_stmt_if_init(void);

/**
 * \brief Frees all memory allocated by an AST if statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_stmt_if_free(ast_stmt_if_t* node);

/**
 * \brief Performs name resolution pass on an AST if statement node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_if_nameres(nameres_ctx_t* ctx, ast_stmt_if_t* node);

/**
 * \brief Performs type check pass on an AST if statement node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_if_typecheck(typecheck_ctx_t* ctx, ast_stmt_if_t* node);

/**
 * \brief Performs control flow analysis pass on an AST if statement node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_if_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_if_t* node);

/**
 * \brief Performs code generation pass on an AST if statement node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_if_codegen(codegen_ctx_t* ctx, ast_stmt_if_t* node);

/**
 * \brief Writes a JSON dump of an AST if statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_if_dump_json(FILE* stream, ast_stmt_if_t* node);

TAU_EXTERN_C_END

#endif
