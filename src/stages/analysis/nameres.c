/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/nameres.h"

tau_nameres_ctx_t* tau_nameres_ctx_init(tau_symtable_t* symtable, tau_error_bag_t* errors)
{
  tau_nameres_ctx_t* ctx = (tau_nameres_ctx_t*)malloc(sizeof(tau_nameres_ctx_t));
  TAU_CLEAROBJ(ctx);

  ctx->global_scope = symtable;
  ctx->scopes = tau_stack_init();
  ctx->errors = errors;

  tau_stack_push(ctx->scopes, ctx->global_scope);

  return ctx;
}

void tau_nameres_ctx_free(tau_nameres_ctx_t* ctx)
{
  tau_stack_free(ctx->scopes);
  free(ctx);
}

tau_symtable_t* tau_nameres_ctx_scope_begin(tau_nameres_ctx_t* ctx)
{
  tau_symtable_t* current_scope = (tau_symtable_t*)tau_stack_top(ctx->scopes);

  tau_symtable_t* inner_scope = tau_symtable_init(current_scope);

  tau_stack_push(ctx->scopes, inner_scope);

  return inner_scope;
}

void tau_nameres_ctx_scope_end(tau_nameres_ctx_t* ctx)
{
  tau_stack_pop(ctx->scopes);
}

tau_symtable_t* tau_nameres_ctx_scope_cur(tau_nameres_ctx_t* ctx)
{
  return (tau_symtable_t*)tau_stack_top(ctx->scopes);
}
