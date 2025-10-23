/**
 * \file
 * 
 * \brief Abstract syntax tree path list node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_PATH_LIST_H
#define TAU_AST_PATH_LIST_H

#include "ast/node.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST path list node.
 */
typedef struct tau_ast_path_list_t
{
  TAU_AST_NODE_HEADER;
  tau_vector_t* paths; // Vector of associated sub-paths.
} tau_ast_path_list_t;

/**
 * \brief Initializes a new AST path list node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_path_list_t* tau_ast_path_list_init(void);

/**
 * \brief Frees all memory allocated by an AST path list node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_path_list_free(tau_ast_path_list_t* node);

/**
 * \brief Writes a JSON dump of an AST path list node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_path_list_dump_json(FILE* stream, tau_ast_path_list_t* node);

TAU_EXTERN_C_END

#endif
