/**
 * \file
 * 
 * \brief Abstract syntax tree identifier node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_ID_H
#define TAU_AST_ID_H

#include "ast/node.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST identifier node.
 */
typedef struct tau_ast_id_t
{
  TAU_AST_NODE_HEADER;
} tau_ast_id_t;

/**
 * \brief Initializes a new AST identifier node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_id_t* tau_ast_id_init(void);

/**
 * \brief Frees all memory allocated by an AST identifier node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_id_free(tau_ast_id_t* node);

/**
 * \brief Writes a JSON dump of an AST identifier node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_id_dump_json(FILE* stream, tau_ast_id_t* node);

TAU_EXTERN_C_END

#endif
