/**
 * \file
 * 
 * \brief Abstract syntax tree path alias node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_PATH_ALIAS_H
#define TAU_AST_PATH_ALIAS_H

#include "ast/node.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST path alias node.
 */
typedef struct tau_ast_path_alias_t
{
  TAU_AST_NODE_HEADER;
  tau_ast_node_t* path; // Pointer to the associated path node to be aliased.
  tau_ast_node_t* id; // Pointer to the associated identifier node to be used as an alias.
} tau_ast_path_alias_t;

/**
 * \brief Initializes a new AST path segment node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_path_alias_t* tau_ast_path_alias_init(void);

/**
 * \brief Frees all memory allocated by an AST path segment node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_path_alias_free(tau_ast_path_alias_t* node);

/**
 * \brief Writes a JSON dump of an AST path segment node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_path_alias_dump_json(FILE* stream, tau_ast_path_alias_t* node);

TAU_EXTERN_C_END

#endif
