/**
 * \file const.h
 * 
 * \brief Abstract syntax tree constant type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_CONST_H
#define TAU_AST_TYPE_CONST_H

#include "stages/parser/ast/type/modif.h"

/**
 * \brief AST constant type node.
 */
typedef struct ast_type_const_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_const_t;

#endif