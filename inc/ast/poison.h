/**
 * \file
 *
 * \brief Abstract syntax tree poison node interface.
 *
 * \details The poison node represents an error or invalid part of the AST encountered
 * during parsing. It serves as a placeholder for unexpected or malformed syntax and
 * allows the parser to continue even after encountering an error.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_POISON_H
#define TAU_AST_POISON_H

#include "ast/node.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief AST poison node.
 */
typedef struct tau_ast_poison_t
{
  TAU_AST_NODE_HEADER;
} tau_ast_poison_t;

/**
 * \brief Initializes a new AST poison node.
 *
 * \returns Pointer to the newly initialized AST node.
 */
tau_ast_poison_t* tau_ast_poison_init(void);

/**
 * \brief Frees all memory allocated by an AST poison node.
 *
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_poison_free(tau_ast_poison_t* node);

/**
 * \brief Writes a JSON dump of an AST poison node into a stream.
 *
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
*/
void tau_ast_poison_dump_json(FILE* stream, tau_ast_poison_t* node);

TAU_EXTERN_C_END

#endif
