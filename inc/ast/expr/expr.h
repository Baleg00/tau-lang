/**
 * \file expr.h
 * 
 * \brief Abstract syntax tree expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_H
#define TAU_AST_EXPR_H

#include <llvm-c/Types.h>

#include "ast/node.h"

/**
 * \brief Header for all AST expression nodes.
 */
#define AST_EXPR_HEADER\
  AST_NODE_HEADER;\
  LLVMTypeRef llvm_type;\
  LLVMValueRef llvm_value

/**
 * \brief AST expression node.
 */
typedef struct ast_expr_t
{
  AST_EXPR_HEADER;
} ast_expr_t;

#endif