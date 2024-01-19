/**
 * \file nameres.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/nameres.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

nameres_ctx_t* nameres_ctx_init(void)
{
  nameres_ctx_t* ctx = (nameres_ctx_t*)malloc(sizeof(nameres_ctx_t));
  clearobj(ctx);

  ctx->scope = symtable_init(NULL);

  return ctx;
}

void nameres_ctx_free(nameres_ctx_t* ctx)
{
  symtable_free(ctx->scope);
  free(ctx);
}
