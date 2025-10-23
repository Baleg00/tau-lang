/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typecheck.h"

#include "utils/common.h"

tau_typecheck_ctx_t* tau_typecheck_ctx_init(tau_typebuilder_t* typebuilder, tau_typetable_t* typetable, tau_error_bag_t* errors)
{
  tau_typecheck_ctx_t* ctx = (tau_typecheck_ctx_t*)malloc(sizeof(tau_typecheck_ctx_t));
  TAU_CLEAROBJ(ctx);

  ctx->typebuilder = typebuilder;
  ctx->typetable = typetable;
  ctx->errors = errors;

  return ctx;
}

void tau_typecheck_ctx_free(tau_typecheck_ctx_t* ctx)
{
  free(ctx);
}

void tau_typecheck_poison(tau_typecheck_ctx_t* ctx, tau_ast_node_t* node)
{
  tau_typedesc_t* poison_desc = tau_typebuilder_build_poison(ctx->typebuilder);

  tau_typetable_insert(ctx->typetable, node, poison_desc);
}
