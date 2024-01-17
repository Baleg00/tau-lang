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

#include "stages/parser/ast/node.h"

/**
 * \brief AST program node.
 */
typedef struct ast_prog_t
{
  AST_NODE_HEADER;
  list_t* decls; // List of associated declarations.
} ast_prog_t;

/**
 * \brief Initializes a new AST program node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_prog_t* ast_prog_init(void);

/**
 * \brief Writes a JSON dump of an AST program node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_prog_dump_json(FILE* stream, ast_prog_t* node);

#endif