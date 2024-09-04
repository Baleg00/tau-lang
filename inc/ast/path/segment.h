/**
 * \file
 * 
 * \brief Abstract syntax tree path segment node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_PATH_SEGMENT_H
#define TAU_AST_PATH_SEGMENT_H

#include "ast/node.h"

/**
 * \brief AST path segment node.
 */
typedef struct ast_path_segment_t
{
  AST_NODE_HEADER;
  ast_node_t* id;
} ast_path_segment_t;

/**
 * \brief Initializes a new AST path segment node.
 * 
 * \returns Pointer to the newly initialized AST node.
 */
ast_path_segment_t* ast_path_segment_init(void);

/**
 * \brief Frees all memory allocated by an AST path segment node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_path_segment_free(ast_path_segment_t* node);

/**
 * \brief Writes a JSON dump of an AST path segment node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void ast_path_segment_dump_json(FILE* stream, ast_path_segment_t* node);

#endif
