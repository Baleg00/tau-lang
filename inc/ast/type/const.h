/**
 * \file const.h
 * 
 * \brief Abstract syntax tree constant type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_CONST_H
#define TAU_AST_TYPE_CONST_H

#include "ast/type/modif.h"

/**
 * \brief AST constant type node.
 */
typedef struct ast_type_const_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_const_t;

/**
 * \brief Initializes a new AST constant type node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_type_const_t* ast_type_const_init(void);

/**
 * \brief Frees all memory allocated by an AST constant type node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_type_const_free(ast_type_const_t* node);

/**
 * \brief Performs name resolution pass on an AST constant type node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_type_const_nameres(nameres_ctx_t* ctx, ast_type_const_t* node);

/**
 * \brief Writes a JSON dump of an AST constant type node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_type_const_dump_json(FILE* stream, ast_type_const_t* node);

#endif