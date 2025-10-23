/**
 * \file
 *
 * \brief Compilation environment.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_ENVIRONMENT_H
#define TAU_ENVIRONMENT_H

#include "llvm.h"
#include "stages/analysis/symtable.h"
#include "stages/analysis/types/types.h"
#include "utils/collections/vector.h"

/**
 * \brief Represents a compilation environment.
 *
 * \details The environment does not own any of its members. It is purely a
 * convenience type that holds objects needed throughout the compilation
 * process.
 */
typedef struct tau_environment_t
{
  tau_vector_t* paths;               ///< Vector of source file paths associated with the environment.
  tau_vector_t* sources;             ///< Vector of source file contents associated with the environment.
  tau_vector_t* tokens;              ///< Vector of tokens associated with the environment.

  tau_symtable_t* symtable;          ///< The symbol table associated with the environment.
  tau_typebuilder_t* typebuilder;    ///< The type builder associated with the environment.
  tau_typetable_t* typetable;        ///< The type table associated with the environment.

  LLVMContextRef llvm_context;   ///< The LLVM context associated with the environment.
  LLVMTargetDataRef llvm_layout; ///< The LLVM target data associated with the environment.
  LLVMModuleRef llvm_module;     ///< The LLVM module associated with the environment.
  LLVMBuilderRef llvm_builder;   ///< The LLVM IR builder associated with the environment.
} tau_environment_t;

/**
 * \brief Initializes a new compilation environment.
 *
 * \param[in] symtable Pointer to the symbol table to be used.
 * \param[in] typebuilder Pointer to the type builder to be used.
 * \param[in] typetable Pointer to the type table to be used.
 * \param[in] llvm_context The LLVM context to be used.
 * \param[in] llvm_layout The LLVM target data to be used.
 * \param[in] llvm_module The LLVM module to be used.
 * \param[in] llvm_builder The LLVM IR builder to be used.
 */
tau_environment_t* tau_environment_init(tau_symtable_t* symtable, tau_typebuilder_t* typebuilder, tau_typetable_t* typetable, LLVMContextRef llvm_context, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_module, LLVMBuilderRef llvm_builder);

/**
 * \brief Frees all resources associated with a compilation environment.
 *
 * \param[in] env Pointer to the environment to be freed.
 */
void tau_environment_free(tau_environment_t* env);

/**
 * \brief Merges a compilation environment into another. The source environment
 * is freed in the process.
 *
 * \param[in,out] dest Pointer to the compilation environment to merge into.
 * \param[in] src Pointer to the compilation environment to be merged.
 */
void tau_environment_merge(tau_environment_t* dest, tau_environment_t* src);

#endif
