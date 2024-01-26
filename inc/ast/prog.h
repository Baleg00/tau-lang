/**
 * \file prog.h
 * 
 * \brief Abstract syntax tree program node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_PROG_H
#define TAU_AST_PROG_H

#include "ast/node.h"

/**
 * \brief AST program node.
 */
typedef struct ast_prog_t
{
  AST_NODE_HEADER;
  symtable_t* scope; // The associated scope of declarations.
  vector_t* decls; // Vector of associated declarations.
} ast_prog_t;

/**
 * \brief Initializes a new AST program node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_prog_t* ast_prog_init(void);

/**
 * \brief Frees all memory allocated by an AST program node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_prog_free(ast_prog_t* node);

/**
 * \brief Performs name resolution pass on an AST program node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_prog_nameres(nameres_ctx_t* ctx, ast_prog_t* node);

/**
 * \brief Performs type check pass on an AST program node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_prog_typecheck(typecheck_ctx_t* ctx, ast_prog_t* node);

/**
 * \brief Performs code generation pass on an AST program node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_prog_codegen(codegen_ctx_t* ctx, ast_prog_t* node);

/**
 * \brief Writes a JSON dump of an AST program node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_prog_dump_json(FILE* stream, ast_prog_t* node);

#endif