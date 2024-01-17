/**
 * \file gen.h
 * 
 * \brief Abstract syntax tree generator type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_GEN_H
#define TAU_AST_TYPE_GEN_H

#include "stages/parser/ast/type/type.h"

/**
 * \brief AST generator type node.
 */
typedef struct ast_type_gen_t
{
  AST_TYPE_HEADER;
  list_t* params; // List of parameter types.
  ast_node_t* yield_type; // Yield type.
} ast_type_gen_t;

#endif