/**
 * \file array.h
 * 
 * \brief Abstract syntax tree array type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_ARRAY_H
#define TAU_AST_TYPE_ARRAY_H

#include "stages/parser/ast/type/modif.h"

/**
 * \brief AST array type node.
 */
typedef struct ast_type_array_t
{
  AST_TYPE_MODIFIER_HEADER;
  ast_node_t* size;
} ast_type_array_t;

/**
 * \brief Initializes a new AST array type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_array_t* ast_type_array_init(void);

/**
 * \brief Writes a JSON dump of an AST array type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_array_dump_json(FILE* stream, ast_type_array_t* node);

#endif