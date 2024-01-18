/**
 * \file mut.h
 * 
 * \brief Abstract syntax tree mutable type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_MUT_H
#define TAU_AST_TYPE_MUT_H

#include "ast/type/modif.h"

/**
 * \brief AST mutable type node.
 */
typedef struct ast_type_mut_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_mut_t;

/**
 * \brief Initializes a new AST mutable type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_mut_t* ast_type_mut_init(void);

/**
 * \brief Writes a JSON dump of an AST mutable type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_mut_dump_json(FILE* stream, ast_type_mut_t* node);

#endif