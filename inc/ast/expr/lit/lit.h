/**
 * \file
 * 
 * \brief Abstract syntax tree literal expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_LIT_H
#define TAU_AST_EXPR_LIT_H

#include "ast/expr/expr.h"

/**
 * \brief Header for all AST literal expression nodes.
 */
#define AST_EXPR_LIT_HEADER\
  AST_EXPR_HEADER

TAU_EXTERN_C_BEGIN

/**
 * \brief AST literal expression node.
 */
typedef struct ast_expr_lit_t
{
  AST_EXPR_LIT_HEADER;
} ast_expr_lit_t;

TAU_EXTERN_C_END

#endif
