/**
 * \file nameres.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/nameres.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

nameres_ctx_t* nameres_ctx_init(void)
{
  nameres_ctx_t* ctx = (nameres_ctx_t*)malloc(sizeof(nameres_ctx_t));
  clearobj(ctx);

  ctx->scopes = stack_init();

  return ctx;
}

void nameres_ctx_free(nameres_ctx_t* ctx)
{
  stack_free(ctx->scopes);
  free(ctx);
}

symtable_t* nameres_ctx_scope_begin(nameres_ctx_t* ctx)
{
  symtable_t* scope = symtable_init(stack_empty(ctx->scopes) ? NULL : (symtable_t*)stack_peek(ctx->scopes));
  stack_push(ctx->scopes, scope);
  return scope;
}

void nameres_ctx_scope_end(nameres_ctx_t* ctx)
{
  stack_pop(ctx->scopes);
}

symtable_t* nameres_ctx_scope_cur(nameres_ctx_t* ctx)
{
  return (symtable_t*)stack_peek(ctx->scopes);
}
