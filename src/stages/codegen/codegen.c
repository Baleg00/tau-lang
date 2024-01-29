/**
 * \file codegen.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/codegen/codegen.h"

#include <llvm-c/Core.h>

#include "utils/common.h"
#include "utils/memory/memtrace.h"

codegen_ctx_t* codegen_ctx_init(typetable_t* typetable, LLVMContextRef llvm_ctx, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_mod)
{
  codegen_ctx_t* ctx = (codegen_ctx_t*)malloc(sizeof(codegen_ctx_t));
  clearobj(ctx);

  ctx->typetable = typetable;
  ctx->llvm_ctx = llvm_ctx;
  ctx->llvm_layout = llvm_layout;
  ctx->llvm_mod = llvm_mod;
  ctx->llvm_builder = LLVMCreateBuilderInContext(llvm_ctx);

  return ctx;
}

void codegen_ctx_free(codegen_ctx_t* ctx)
{
  LLVMDisposeBuilder(ctx->llvm_builder);
  free(ctx);
}
