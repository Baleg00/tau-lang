/**
 * \file
 * 
 * \brief Abstract syntax tree expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_H
#define TAU_AST_EXPR_H

#include "ast/node.h"

/**
 * \brief Header for all AST expression nodes.
 */
#define TAU_AST_EXPR_HEADER\
  TAU_AST_NODE_HEADER;\
  LLVMTypeRef llvm_type;\
  LLVMValueRef llvm_value

TAU_EXTERN_C_BEGIN

/**
 * \brief AST expression node.
 */
typedef struct tau_ast_expr_t
{
  TAU_AST_EXPR_HEADER;
} tau_ast_expr_t;

TAU_EXTERN_C_END

#endif
