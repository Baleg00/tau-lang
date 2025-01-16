/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "compiler/environment.h"

#include "utils/common.h"

environment_t* environment_init(symtable_t* symtable, typebuilder_t* typebuilder, typetable_t* typetable, LLVMContextRef llvm_context, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_module, LLVMBuilderRef llvm_builder)
{
  environment_t* env = (environment_t*)malloc(sizeof(environment_t));
  ASSERT(env != NULL);

  env->paths = vector_init();
  env->sources = vector_init();
  env->tokens = vector_init();
  env->symtable = symtable;
  env->typebuilder = typebuilder;
  env->typetable = typetable;
  env->llvm_context = llvm_context;
  env->llvm_layout = llvm_layout;
  env->llvm_module = llvm_module;
  env->llvm_builder = llvm_builder;

  return env;
}

void environment_free(environment_t* env)
{
  vector_free(env->paths);
  vector_free(env->sources);
  vector_free(env->tokens);
  free(env);
}

void environment_merge(environment_t* dest, environment_t* src)
{
  symtable_merge(dest->symtable, src->symtable);
  typetable_merge(dest->typetable, src->typetable);

  if (LLVMLinkModules2(dest->llvm_module, src->llvm_module))
  {
    log_error("env", "Failed to link modules.");
  }

  environment_free(src);
}
