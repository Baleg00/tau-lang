/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/ctrlflow.h"

#include "utils/common.h"

tau_ctrlflow_ctx_t* tau_ctrlflow_ctx_init(tau_error_bag_t* errors)
{
  tau_ctrlflow_ctx_t* ctx = (tau_ctrlflow_ctx_t*)malloc(sizeof(tau_ctrlflow_ctx_t));
  TAU_CLEAROBJ(ctx);

  ctx->blocks = tau_stack_init();
  ctx->stmts = tau_vector_init_with_capacity(1);
  ctx->errors = errors;

  return ctx;
}

void tau_ctrlflow_ctx_free(tau_ctrlflow_ctx_t* ctx)
{
  tau_stack_free(ctx->blocks);
  tau_vector_free(ctx->stmts);
  free(ctx);
}

void tau_ctrlflow_ctx_block_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_block_t* node)
{
  tau_stack_push(ctx->blocks, node);
}

void tau_ctrlflow_ctx_block_end(tau_ctrlflow_ctx_t* ctx)
{
  tau_stack_pop(ctx->blocks);
}

tau_ast_stmt_block_t* tau_ctrlflow_ctx_block_cur(tau_ctrlflow_ctx_t* ctx)
{
  if (tau_stack_empty(ctx->blocks))
    return NULL;

  return (tau_ast_stmt_block_t*)tau_stack_top(ctx->blocks);
}

void tau_ctrlflow_ctx_while_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_while_t* node)
{
  tau_vector_push(ctx->stmts, node);
}

void tau_ctrlflow_ctx_while_end(tau_ctrlflow_ctx_t* ctx)
{
  tau_vector_pop(ctx->stmts);
}

void tau_ctrlflow_ctx_do_while_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_do_while_t* node)
{
  tau_vector_push(ctx->stmts, node);
}

void tau_ctrlflow_ctx_do_while_end(tau_ctrlflow_ctx_t* ctx)
{
  tau_vector_pop(ctx->stmts);
}

void tau_ctrlflow_ctx_loop_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_loop_t* node)
{
  tau_vector_push(ctx->stmts, node);
}

void tau_ctrlflow_ctx_loop_end(tau_ctrlflow_ctx_t* ctx)
{
  tau_vector_pop(ctx->stmts);
}

void tau_ctrlflow_ctx_for_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_for_t* node)
{
  tau_vector_push(ctx->stmts, node);
}

void tau_ctrlflow_ctx_for_end(tau_ctrlflow_ctx_t* ctx)
{
  tau_vector_pop(ctx->stmts);
}

void tau_ctrlflow_ctx_defer_begin(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_defer_t* node)
{
  tau_vector_push(ctx->stmts, node);
}

void tau_ctrlflow_ctx_defer_end(tau_ctrlflow_ctx_t* ctx)
{
  tau_vector_pop(ctx->stmts);
}
