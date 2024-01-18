/**
 * \file decl.h
 * 
 * \brief Abstract syntax tree declaration type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_DECL_H
#define TAU_AST_TYPE_DECL_H

#include "ast/type/type.h"

/**
 * \brief AST declaration type node.
 */
typedef struct ast_type_decl_t
{
  AST_TYPE_HEADER;
  ast_node_t* decl; // The associated declaration node.
} ast_type_decl_t;

/**
 * \brief Initializes a new AST declaration type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_decl_t* ast_type_decl_init(void);

/**
 * \brief Writes a JSON dump of an AST declaration type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_decl_dump_json(FILE* stream, ast_type_decl_t* node);

#endif