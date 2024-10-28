/**
 * \file
 *
 * \brief Abstract syntax tree generic declaration node interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_GENERIC_H
#define TAU_AST_DECL_GENERIC_H

#include "ast/decl/decl.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST generic declaration node.
 */
typedef struct ast_decl_generic_t
{
  AST_DECL_HEADER;
  ast_node_t* node; ///< Pointer to the AST node being generalized.
  vector_t* params; ///< Vector of generic parameters.
} ast_decl_generic_t;

/**
 * \brief Initializes a new AST generic declaration node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_generic_t* ast_decl_generic_init(void);

/**
 * \brief Frees all memory allocated by an AST generic declaration node.
 *
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_decl_generic_free(ast_decl_generic_t* node);

/**
 * \brief Performs name resolution pass on an AST generic declaration node.
 *
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_generic_nameres(nameres_ctx_t* ctx, ast_decl_generic_t* node);

/**
 * \brief Performs type check pass on an AST generic declaration node.
 *
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_decl_generic_typecheck(typecheck_ctx_t* ctx, ast_decl_generic_t* node);

/**
 * \brief Performs control flow analysis pass on an AST generic declaration node.
 *
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_decl_generic_ctrlflow(ctrlflow_ctx_t* ctx, ast_decl_generic_t* node);

/**
 * \brief Performs code generation pass on an AST generic declaration node.
 *
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_generic_codegen(codegen_ctx_t* ctx, ast_decl_generic_t* node);

/**
 * \brief Writes a JSON dump of an AST generic declaration node into a stream.
 *
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_generic_dump_json(FILE* stream, ast_decl_generic_t* node);

TAU_EXTERN_C_END

#endif
