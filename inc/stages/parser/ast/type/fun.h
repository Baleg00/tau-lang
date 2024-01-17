/**
 * \file fun.h
 * 
 * \brief Abstract syntax tree function type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_FUN_H
#define TAU_AST_TYPE_FUN_H

#include "stages/parser/callconv.h"
#include "stages/parser/ast/type/type.h"

/**
 * \brief AST function type node.
 */
typedef struct ast_type_fun_t
{
  AST_TYPE_HEADER;
  list_t* params; // List of associated parameter types.
  ast_node_t* return_type; // The associated return type.
  callconv_kind_t callconv; // The associated calling convention.
  bool is_vararg; // Is function variadic (C-style, only works with specific calling conventions).
} ast_type_fun_t;

#endif