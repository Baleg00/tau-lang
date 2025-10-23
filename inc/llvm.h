/**
 * \file
 * 
 * \brief LLVM context.
 * 
 * \details LLVM is an open-source compiler infrastructure project designed for
 * the development and optimization of programming languages. It provides a
 * collection of modular and reusable compiler and toolchain components,
 * enabling the creation of efficient and portable compilers.
 * 
 * \see https://llvm.org
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_LLVM_H
#define TAU_LLVM_H

#include <stdbool.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/BitReader.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>
#include <llvm-c/Error.h>
#include <llvm-c/ErrorHandling.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Linker.h>
#include <llvm-c/LLJIT.h>
#include <llvm-c/Object.h>
#include <llvm-c/Support.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/PassBuilder.h>
#include <llvm-c/Types.h>

#include "utils/extern_c.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Initializes LLVM and all of its components.
 * 
 * \returns `true` if LLVM and all of its components were initialized successfully,
 * otherwise `false`.
 */
bool tau_llvm_init(void);

/**
 * \brief Frees all resources associated with LLVM.
 */
void tau_llvm_free(void);

/**
 * \brief Gets the current LLVM context.
 * 
 * \returns The current LLVM context.
 */
LLVMContextRef tau_llvm_get_context(void);

/**
 * \brief Gets the current LLVM target.
 * 
 * \returns The current LLVM target.
 */
LLVMTargetRef tau_llvm_get_target(void);

/**
 * \brief Gets the current LLVM target data layout.
 * 
 * \returns The current LLVM target data layout.
 */
LLVMTargetDataRef tau_llvm_get_data(void);

/**
 * \brief Gets the current LLVM target machine.
 * 
 * \returns The current LLVM target machine.
 */
LLVMTargetMachineRef tau_llvm_get_machine(void);

/**
 * \brief Gets the current LLVM target triple.
 * 
 * \returns The current LLVM target triple.
 */
const char* tau_llvm_get_target_triple(void);

/**
 * \brief Gets the host CPU name.
 * 
 * \returns The host CPU name.
 */
const char* tau_llvm_get_cpu_name(void);

/**
 * \brief Gets the host CPU's features.
 * 
 * \returns The host CPU's features.
 */
const char* tau_llvm_get_cpu_features(void);

TAU_EXTERN_C_END

#endif
