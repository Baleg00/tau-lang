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
#include "utils/collections/vector.h"

/**
 * \brief AST path list node.
 */
typedef struct ast_path_list_t
{
  AST_NODE_HEADER;
  vector_t* paths; // Vector of associated sub-paths.
} ast_path_list_t;

/**
 * \brief Initializes a new AST path list node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_path_list_t* ast_path_list_init(void);

/**
 * \brief Frees all memory allocated by an AST path list node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_path_list_free(ast_path_list_t* node);

/**
 * \brief Writes a JSON dump of an AST path list node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_path_list_dump_json(FILE* stream, ast_path_list_t* node);

#endif
