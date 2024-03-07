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

/**
 * \brief AST path wildcard node.
 */
typedef struct ast_path_wildcard_t
{
  AST_NODE_HEADER;
} ast_path_wildcard_t;

/**
 * \brief Initializes a new AST path wildcard node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_path_wildcard_t* ast_path_wildcard_init(void);

/**
 * \brief Frees all memory allocated by an AST path wildcard node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_path_wildcard_free(ast_path_wildcard_t* node);

/**
 * \brief Writes a JSON dump of an AST path wildcard node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_path_wildcard_dump_json(FILE* stream, ast_path_wildcard_t* node);

#endif