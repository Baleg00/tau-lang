/**
 * \file
 * 
 * \brief Abstract syntax tree path wildcard node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_PATH_WILDCARD_H
#define TAU_AST_PATH_WILDCARD_H

#include "ast/node.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST path wildcard node.
 */
typedef struct tau_ast_path_wildcard_t
{
  TAU_AST_NODE_HEADER;
} tau_ast_path_wildcard_t;

/**
 * \brief Initializes a new AST path wildcard node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_path_wildcard_t* tau_ast_path_wildcard_init(void);

/**
 * \brief Frees all memory allocated by an AST path wildcard node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_path_wildcard_free(tau_ast_path_wildcard_t* node);

/**
 * \brief Writes a JSON dump of an AST path wildcard node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_path_wildcard_dump_json(FILE* stream, tau_ast_path_wildcard_t* node);

TAU_EXTERN_C_END

#endif
