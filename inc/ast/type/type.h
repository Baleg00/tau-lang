/**
 * \file
 * 
 * \brief Abstract syntax tree type node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_TYPE_H
#define TAU_AST_TYPE_H

#include "ast/node.h"

/**
 * \brief Header for all AST type nodes.
 */
#define TAU_AST_TYPE_HEADER\
  TAU_AST_NODE_HEADER;\
  LLVMTypeRef llvm_type

TAU_EXTERN_C_BEGIN

/**
 * \brief AST type node.
 */
typedef struct tau_ast_type_t
{
  TAU_AST_TYPE_HEADER;
} tau_ast_type_t;

TAU_EXTERN_C_END

#endif
