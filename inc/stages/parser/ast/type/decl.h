/**
 * \file decl.h
 * 
 * \brief Abstract syntax tree declaration type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_DECL_H
#define TAU_AST_TYPE_DECL_H

#include "stages/parser/ast/type/type.h"

/**
 * \brief AST declaration type node.
 */
typedef struct ast_type_decl_t
{
  AST_TYPE_HEADER;
  ast_node_t* decl; // The associated declaration node.
} ast_type_decl_t;

#endif