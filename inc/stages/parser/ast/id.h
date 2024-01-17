/**
 * \file id.h
 * 
 * \brief Abstract syntax tree identifier node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_ID_H
#define TAU_AST_ID_H

#include "stages/parser/ast/node.h"

/**
 * \brief AST identifier node.
 */
typedef struct ast_id_t
{
  AST_NODE_HEADER;
} ast_id_t;

#endif