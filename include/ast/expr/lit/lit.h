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
#define TAU_AST_EXPR_LIT_HEADER\
  TAU_AST_EXPR_HEADER

TAU_EXTERN_C_BEGIN

/**
 * \brief AST literal expression node.
 */
typedef struct tau_ast_expr_lit_t
{
  TAU_AST_EXPR_LIT_HEADER;
} tau_ast_expr_lit_t;

TAU_EXTERN_C_END

#endif
