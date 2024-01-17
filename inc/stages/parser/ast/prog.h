/**
 * \file prog.h
 * 
 * \brief Abstract syntax tree program node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_PROG_H
#define TAU_AST_PROG_H

#include "stages/parser/ast/node.h"

/**
 * \brief AST program node.
 */
typedef struct ast_prog_t
{
  AST_NODE_HEADER;
  list_t* decls; // List of associated declarations.
} ast_prog_t;

#endif