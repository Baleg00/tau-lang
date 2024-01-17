/**
 * \file lit.h
 * 
 * \brief Abstract syntax tree literal expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_H
#define TAU_AST_EXPR_LIT_H

#include "stages/parser/ast/expr/expr.h"

/**
 * \brief AST literal expression node.
 */
typedef struct ast_expr_lit_t
{
  AST_EXPR_HEADER;
} ast_expr_lit_t;

#endif