/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "compiler/environment.h"

#include "utils/common.h"

tau_environment_t* tau_environment_init(tau_symtable_t* symtable, tau_typebuilder_t* typebuilder, tau_typetable_t* typetable, LLVMContextRef llvm_context, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_module, LLVMBuilderRef llvm_builder)
{
  tau_environment_t* env = (tau_environment_t*)malloc(sizeof(tau_environment_t));
  TAU_ASSERT(env != NULL);

  env->paths = tau_vector_init();
  env->sources = tau_vector_init();
  env->tokens = tau_vector_init();
  env->symtable = symtable;
  env->typebuilder = typebuilder;
  env->typetable = typetable;
  env->llvm_context = llvm_context;
  env->llvm_layout = llvm_layout;
  env->llvm_module = llvm_module;
  env->llvm_builder = llvm_builder;

  return env;
}

void tau_environment_free(tau_environment_t* env)
{
  tau_vector_free(env->paths);
  tau_vector_free(env->sources);
  tau_vector_free(env->tokens);
  free(env);
}

void tau_environment_merge(tau_environment_t* dest, tau_environment_t* src)
{
  tau_symtable_merge(dest->symtable, src->symtable);
  tau_typetable_merge(dest->typetable, src->typetable);

  if (LLVMLinkModules2(dest->llvm_module, src->llvm_module))
  {
    tau_log_error("env", "Failed to link modules.");
  }

  tau_environment_free(src);
}
