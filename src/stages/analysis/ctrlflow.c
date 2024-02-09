/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/ctrlflow.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

ctrlflow_ctx_t* ctrlflow_ctx_init(void)
{
  ctrlflow_ctx_t* ctx = (ctrlflow_ctx_t*)malloc(sizeof(ctrlflow_ctx_t));
  clearobj(ctx);

  ctx->blocks = stack_init();

  return ctx;
}

void ctrlflow_ctx_free(ctrlflow_ctx_t* ctx)
{
  stack_free(ctx->blocks);
  free(ctx);
}

void ctrlflow_ctx_block_begin(ctrlflow_ctx_t* ctx, ast_stmt_block_t* node)
{
  stack_push(ctx->blocks, node);
}

void ctrlflow_ctx_block_end(ctrlflow_ctx_t* ctx)
{
  stack_pop(ctx->blocks);
}

ast_stmt_block_t* ctrlflow_ctx_block_cur(ctrlflow_ctx_t* ctx)
{
  if (stack_empty(ctx->blocks))
    return NULL;

  return (ast_stmt_block_t*)stack_top(ctx->blocks);
}
