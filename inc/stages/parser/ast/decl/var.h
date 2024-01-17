/**
 * \file var.h
 * 
 * \brief Abstract syntax tree variable declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_VAR_H
#define TAU_AST_DECL_VAR_H

#include "stages/parser/ast/decl/decl.h"

/**
 * \brief AST variable declaration node.
 */
typedef struct ast_decl_var_t
{
  AST_DECL_HEADER;
  ast_node_t* type; // The associated type.
  ast_node_t* expr; // The associated initializer expression.
  bool is_pub; // Is variable public (in case it is a member).
  bool is_extern; // Is variable external.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.
} ast_decl_var_t;

#endif