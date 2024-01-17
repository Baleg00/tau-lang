/**
 * \file enum_constant.h
 * 
 * \brief Abstract syntax tree enum constant declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_ENUM_CONSTANT_H
#define TAU_AST_DECL_ENUM_CONSTANT_H

#include <llvm-c/Types.h>

#include "stages/parser/ast/decl/decl.h"

/**
 * \brief AST enum constant declaration node.
 */
typedef struct ast_decl_enum_constant_t
{
  AST_DECL_HEADER;
  ast_node_t* type; // The associated enum type.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.
} ast_decl_enum_constant_t;

#endif