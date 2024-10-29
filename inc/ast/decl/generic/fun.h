/**
 * \file
 *
 * \brief Abstract syntax tree generic function declaration node interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_GENERIC_FUN_H
#define TAU_AST_DECL_GENERIC_FUN_H

#include "ast/decl/decl.h"
#include "utils/collections/vector.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST generic function declaration node.
 */
typedef struct ast_decl_generic_fun_t
{
  AST_DECL_HEADER;
  symtable_t* scope;        ///< The associated scope of the generic function declaration.
  vector_t* generic_params; ///< Vector of generic parameter declarations.
  vector_t* params;         ///< Vector of function parameter declarations.
  ast_node_t* return_type;  ///< Pointer to the return type node.
  ast_node_t* stmt;         ///< Pointer to the body statement node.
} ast_decl_generic_fun_t;

/**
 * \brief Initializes a new AST generic function declaration node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
ast_decl_generic_fun_t* ast_decl_generic_fun_init(void);

/**
 * \brief Frees all memory allocated by an AST generic function declaration node.
 *
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_decl_generic_fun_free(ast_decl_generic_fun_t* node);

/**
 * \brief Performs name resolution pass on an AST generic function declaration node.
 *
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_decl_generic_fun_nameres(nameres_ctx_t* ctx, ast_decl_generic_fun_t* node);

/**
 * \brief Writes a JSON dump of an AST generic function declaration node into a stream.
 *
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_decl_generic_fun_dump_json(FILE* stream, ast_decl_generic_fun_t* node);

TAU_EXTERN_C_END

#endif
