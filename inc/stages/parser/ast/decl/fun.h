/**
 * \file fun.h
 * 
 * \brief Abstract syntax tree function declaration node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_DECL_FUN_H
#define TAU_AST_DECL_FUN_H

#include <llvm-c/Types.h>

#include "stages/parser/callconv.h"
#include "stages/parser/ast/decl/decl.h"

/**
 * \brief AST function declaration node.
 */
typedef struct ast_decl_fun_t
{
  AST_DECL_HEADER;
  list_t* params; // List of associated parameter declarations.
  ast_node_t* return_type; // The associated return type.
  ast_node_t* stmt; // The associated body statement.
  callconv_kind_t callconv; // The associated calling convention.
  bool is_pub; // Is function public (in case it is a member).
  bool is_vararg; // Is function variadic (C-style, only works with specific calling conventions).
  bool is_extern; // Is function external.

  LLVMTypeRef llvm_type; // The associated LLVM type.
  LLVMValueRef llvm_value; // The associated LLVM value.

  LLVMBasicBlockRef llvm_entry; // LLVM block for function entry.
  LLVMBasicBlockRef llvm_end; // LLVM block for function end.
} ast_decl_fun_t;

#endif