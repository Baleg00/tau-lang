/**
 * \file llvm.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "llvm.h"

#include "utils/common.h"
#include "utils/io/log.h"

static LLVMContextRef g_llvm_context = NULL;
static LLVMTargetRef g_llvm_target = NULL;
static LLVMTargetDataRef g_llvm_data = NULL;
static LLVMTargetMachineRef g_llvm_machine = NULL;
static char* g_llvm_target_triple = NULL;
static char* g_llvm_cpu_name = NULL;
static char* g_llvm_cpu_features = NULL;

static void llvm_fatal_error_handler(const char* reason)
{
  log_fatal("LLVM", reason);
  exit(EXIT_FAILURE);
}

bool llvm_init(void)
{
  char* error_str = NULL;

  if (LLVMLoadLibraryPermanently(NULL))
  {
    log_error("LLVM", "Failed to load known libraries.");
    return true;
  }

  if (LLVMInitializeNativeTarget())
  {
    log_error("LLVM", "Failed to initialize native target.");
    return true;
  }

  if (LLVMInitializeNativeAsmParser())
  {
    log_error("LLVM", "Failed to initialize native ASM parser.");
    return true;
  }

  if (LLVMInitializeNativeAsmPrinter())
  {
    log_error("LLVM", "Failed to initialize native ASM printer.");
    return true;
  }

  if (LLVMInitializeNativeDisassembler())
  {
    log_error("LLVM", "Failed to initialize native disassembler.");
    return true;
  }

  LLVMInstallFatalErrorHandler(llvm_fatal_error_handler);

  g_llvm_context = LLVMContextCreate();
  g_llvm_target_triple = LLVMGetDefaultTargetTriple();

  if (LLVMGetTargetFromTriple(g_llvm_target_triple, &g_llvm_target, &error_str))
  {
    log_error("LLVM", "Failed to get target from triple.");
    log_error("LLVM", error_str);
    return true;
  }

  g_llvm_cpu_name = LLVMGetHostCPUName();
  g_llvm_cpu_features = LLVMGetHostCPUFeatures();

  g_llvm_machine = LLVMCreateTargetMachine(
    g_llvm_target,
    g_llvm_target_triple,
    g_llvm_cpu_name,
    g_llvm_cpu_features,
    LLVMCodeGenLevelDefault,
    LLVMRelocDefault,
    LLVMCodeModelDefault
  );

  if (g_llvm_machine == NULL)
  {
    log_error("LLVM", "Failed to create target machine.");
    return true;
  }

  g_llvm_data = LLVMCreateTargetDataLayout(g_llvm_machine);

  if (g_llvm_data == NULL)
  {
    log_error("LLVM", "Failed to create target data layout.");
    return true;
  }

  return false;
}

void llvm_free(void)
{
  LLVMDisposeTargetData(g_llvm_data);
  LLVMDisposeTargetMachine(g_llvm_machine);
  LLVMDisposeMessage(g_llvm_cpu_features);
  LLVMDisposeMessage(g_llvm_cpu_name);
  LLVMDisposeMessage(g_llvm_target_triple);
  LLVMContextDispose(g_llvm_context);
  LLVMResetFatalErrorHandler();
  LLVMShutdown();
}

LLVMContextRef llvm_get_context(void)
{
  return g_llvm_context;
}

LLVMTargetRef llvm_get_target(void)
{
  return g_llvm_target;
}

LLVMTargetDataRef llvm_get_data(void)
{
  return g_llvm_data;
}

LLVMTargetMachineRef llvm_get_machine(void)
{
  return g_llvm_machine;
}

const char* llvm_get_target_triple(void)
{
  return g_llvm_target_triple;
}

const char* llvm_get_cpu_name(void)
{
  return g_llvm_cpu_name;
}

const char* llvm_get_cpu_features(void)
{
  return g_llvm_cpu_features;
}
