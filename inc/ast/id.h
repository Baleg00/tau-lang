/**
 * \file id.h
 * 
 * \brief Abstract syntax tree identifier node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_ID_H
#define TAU_AST_ID_H

#include "ast/node.h"

/**
 * \brief AST identifier node.
 */
typedef struct ast_id_t
{
  AST_NODE_HEADER;
} ast_id_t;

/**
 * \brief Initializes a new AST identifier node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_id_t* ast_id_init(void);

/**
 * \brief Writes a JSON dump of an AST identifier node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_id_dump_json(FILE* stream, ast_id_t* node);

#endif