/**
 * \file ref.h
 * 
 * \brief Abstract syntax tree reference type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_REF_H
#define TAU_AST_TYPE_REF_H

#include "stages/parser/ast/type/modif.h"

/**
 * \brief AST reference type node.
 */
typedef struct ast_type_ref_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_ref_t;

/**
 * \brief Initializes a new AST reference type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_ref_t* ast_type_ref_init(void);

/**
 * \brief Writes a JSON dump of an AST reference type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_ref_dump_json(FILE* stream, ast_type_ref_t* node);

#endif