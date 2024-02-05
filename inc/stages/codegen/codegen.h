/**
 * \file
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

#include "llvm.h"
#include "stages/analysis/types/typetable.h"

/**
 * \see ast/decl/fun.h
 */
typedef struct ast_decl_fun_t ast_decl_fun_t;

/**
 * \brief Code generation context.
 */
typedef struct codegen_ctx_t
{
  typetable_t* typetable;        // Pointer to the associated type table.
  ast_decl_fun_t* fun_node;      // Pointer to the current AST function declaration being visited.
  size_t param_idx;              // Parameter index in the current function being visited.
  size_t enum_idx;               // Enum constant index in the current enum being visited.

  LLVMContextRef llvm_ctx;       // Reference to the associated LLVM context.
  LLVMTargetDataRef llvm_layout; // Reference to the associated LLVM target data layout.
  LLVMModuleRef llvm_mod;        // Reference to the associated LLVM module.
  LLVMBuilderRef llvm_builder;   // Reference to the associated LLVM IR builder.
} codegen_ctx_t;

/**
 * \brief Initializes a new code generation context.
 * 
 * \param[in] llvm_ctx The LLVM context to be used.
 * \param[in] llvm_layout The LLVM target data layout to be used.
 * \param[in] llvm_mod The LLVM module to be used.
 * \returns Pointer to the newly initialized code generation context.
 */
codegen_ctx_t* codegen_ctx_init(typetable_t* typetable, LLVMContextRef llvm_ctx, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_mod);

/**
 * \brief Frees all memory allocated by a code generation context.
 * 
 * \param[in] ctx Pointer to the code generation context to be freed.
 */
void codegen_ctx_free(codegen_ctx_t* ctx);

#endif