/**
 * \file opt.h
 * 
 * \brief Abstract syntax tree optional type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_OPT_H
#define TAU_AST_TYPE_OPT_H

#include "stages/parser/ast/type/modif.h"

/**
 * \brief AST optional type node.
 */
typedef struct ast_type_opt_t
{
  AST_TYPE_MODIFIER_HEADER;
} ast_type_opt_t;

#endif