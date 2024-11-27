/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/codegen/codegen.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

codegen_ctx_t* codegen_ctx_init(typebuilder_t* typebuilder, typetable_t* typetable, LLVMContextRef llvm_ctx, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_mod, LLVMBuilderRef llvm_builder)
{
  codegen_ctx_t* ctx = (codegen_ctx_t*)malloc(sizeof(codegen_ctx_t));
  CLEAROBJ(ctx);

  ctx->typebuilder = typebuilder;
  ctx->typetable = typetable;
  ctx->llvm_ctx = llvm_ctx;
  ctx->llvm_layout = llvm_layout;
  ctx->llvm_mod = llvm_mod;
  ctx->llvm_builder = llvm_builder;

  return ctx;
}

void codegen_ctx_free(codegen_ctx_t* ctx)
{
  free(ctx);
}
