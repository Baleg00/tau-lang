/**
 * \file codegen.h
 * 
 * \brief Code generation pass.
 * 
 * \details 
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CODEGEN_H
#define TAU_CODEGEN_H

#include <llvm-c/Types.h>

#include "stages/analysis/typetable.h"

/**
 * \see fun.h
 */
typedef struct ast_decl_fun_t ast_decl_fun_t;

/**
 * \brief Code generation context.
 */
typedef struct codegen_ctx_t
{
  typetable_t* typetable;
  ast_decl_fun_t* fun_node;
  size_t param_idx;
  size_t enum_idx;

  LLVMContextRef llvm_ctx;
  LLVMModuleRef llvm_mod;
  LLVMBuilderRef llvm_builder;
} codegen_ctx_t;

/**
 * \brief Initializes a new code generation context.
 * 
 * \returns Pointer to the newly initialized code generation context.
 */
codegen_ctx_t* codegen_ctx_init(typetable_t* typetable, LLVMContextRef llvm_ctx, LLVMModuleRef llvm_mod);

/**
 * \brief Frees all memory allocated by a code generation context.
 * 
 * \param[in] ctx Pointer to the code generation context to be freed.
 */
void codegen_ctx_free(codegen_ctx_t* ctx);

#endif