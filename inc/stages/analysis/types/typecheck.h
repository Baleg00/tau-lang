/**
 * \file
 * 
 * \brief Type check pass.
 * 
 * \details The type check pass is a stage during the compilation process where
 * the types of expressions, variables, and other language constructs are validated
 * for consistency and adherence to the language's type system. This pass ensures
 * that the program follows the rules of the language with respect to type
 * compatibility, assignment, and function invocation, preventing type-related
 * errors at runtime.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPECHECK_H
#define TAU_TYPECHECK_H

#include "llvm.h"
#include "stages/analysis/types/typebuilder.h"
#include "stages/analysis/types/typetable.h"

/**
 * \brief Type check context.
 */
typedef struct typecheck_ctx_t
{
  typebuilder_t* typebuilder;
  typetable_t* typetable;

  typedesc_fun_t* fun_desc; // Type descriptor of the containing function declaration.
  typedesc_enum_t* enum_desc; // Type descriptor of the containing enum declaration.
} typecheck_ctx_t;

/**
 * \brief Initializes a new type check context.
 * 
 * \param[in] llvm_ctx The LLVM context to be used.
 * \param[in] llvm_layout The LLVM target data layout to be used.
 * \returns Pointer to the newly initialized type check context.
 */
typecheck_ctx_t* typecheck_ctx_init(LLVMContextRef llvm_ctx, LLVMTargetDataRef llvm_layout);

/**
 * \brief Frees all memory allocated by a type check context.
 * 
 * \param[in] ctx Pointer to the type check context to be freed.
 */
void typecheck_ctx_free(typecheck_ctx_t* ctx);

#endif