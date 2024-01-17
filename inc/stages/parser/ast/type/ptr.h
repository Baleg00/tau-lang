/**
 * \file ptr.h
 * 
 * \brief Abstract syntax tree pointer type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_PTR_H
#define TAU_AST_TYPE_PTR_H

#include "stages/parser/ast/type/modif.h"

/**
 * \brief AST pointer type node.
 */
typedef struct ast_type_ptr_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_ptr_t;

#endif