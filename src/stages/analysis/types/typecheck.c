/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typecheck.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typecheck_ctx_t* typecheck_ctx_init(LLVMContextRef llvm_ctx, LLVMTargetDataRef llvm_layout)
{
  typecheck_ctx_t* ctx = (typecheck_ctx_t*)malloc(sizeof(typecheck_ctx_t));
  clearobj(ctx);

  ctx->typebuilder = typebuilder_init(llvm_ctx, llvm_layout);
  ctx->typetable = typetable_init();

  return ctx;
}

void typecheck_ctx_free(typecheck_ctx_t* ctx)
{
  typebuilder_free(ctx->typebuilder);
  typetable_free(ctx->typetable);
  free(ctx);
}
