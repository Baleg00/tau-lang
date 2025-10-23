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

TAU_EXTERN_C_BEGIN

/**
 * \brief AST use directive node.
 */
typedef struct tau_ast_use_t
{
  TAU_AST_NODE_HEADER;
  tau_ast_node_t* path; // Pointer to the associated AST path node.
} tau_ast_use_t;

/**
 * \brief Initializes a new AST use directive node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_use_t* tau_ast_use_init(void);

/**
 * \brief Frees all memory allocated by an AST use directive node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_use_free(tau_ast_use_t* node);

/**
 * \brief Writes a JSON dump of an AST use directive node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_use_dump_json(FILE* stream, tau_ast_use_t* node);

TAU_EXTERN_C_END

#endif
