/**
 * \file mbr.h
 * 
 * \brief Abstract syntax tree member type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_MBR_H
#define TAU_AST_TYPE_MBR_H

#include "ast/type/type.h"

/**
 * \brief AST member type node.
 */
typedef struct ast_type_mbr_t
{
  AST_TYPE_HEADER;
  ast_node_t* parent;
  ast_node_t* member;
} ast_type_mbr_t;

/**
 * \brief Initializes a new AST member type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_mbr_t* ast_type_mbr_init(void);

/**
 * \brief Writes a JSON dump of an AST member type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_mbr_dump_json(FILE* stream, ast_type_mbr_t* node);

#endif