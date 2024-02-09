/**
 * \file
 * 
 * \brief Abstract syntax tree while-loop statement node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_STMT_WHILE_H
#define TAU_AST_STMT_WHILE_H

#include "ast/stmt/stmt.h"
#include "llvm.h"

/**
 * \brief AST while-loop statement node.
 */
typedef struct ast_stmt_while_t
{
  AST_STMT_HEADER;
  symtable_t* scope; // The associated scope.
  ast_node_t* cond; // The associated condition expression.
  ast_node_t* stmt; // The associated body statement.

  LLVMBasicBlockRef llvm_cond; // LLVM block for the condition expression.
  LLVMBasicBlockRef llvm_loop; // LLVM block for the body statement.
  LLVMBasicBlockRef llvm_end; // LLVM block for the end of the loop.
} ast_stmt_while_t;

/**
 * \brief Initializes a new AST while statement node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_stmt_while_t* ast_stmt_while_init(void);

/**
 * \brief Frees all memory allocated by an AST while statement node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_stmt_while_free(ast_stmt_while_t* node);

/**
 * \brief Performs name resolution pass on an AST while statement node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_while_nameres(nameres_ctx_t* ctx, ast_stmt_while_t* node);

/**
 * \brief Performs type check pass on an AST while statement node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_while_typecheck(typecheck_ctx_t* ctx, ast_stmt_while_t* node);

/**
 * \brief Performs control flow analysis pass on an AST while statement node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_stmt_while_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_while_t* node);

/**
 * \brief Performs code generation pass on an AST while statement node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_stmt_while_codegen(codegen_ctx_t* ctx, ast_stmt_while_t* node);

/**
 * \brief Writes a JSON dump of an AST while statement node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_stmt_while_dump_json(FILE* stream, ast_stmt_while_t* node);

#endif