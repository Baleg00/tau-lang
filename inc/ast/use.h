/**
 * \file
 * 
 * \brief Abstract syntax tree use directive node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_USE_H
#define TAU_AST_USE_H

#include "ast/node.h"

/**
 * \brief AST use directive node.
 */
typedef struct ast_use_t
{
  AST_NODE_HEADER;
} ast_use_t;

/**
 * \brief Initializes a new AST use directive node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_use_t* ast_use_init(void);

/**
 * \brief Frees all memory allocated by an AST use directive node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_use_free(ast_use_t* node);

/**
 * \brief Writes a JSON dump of an AST use directive node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_use_dump_json(FILE* stream, ast_use_t* node);

#endif