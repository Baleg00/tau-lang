/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typecheck.h"

#include "utils/common.h"

typecheck_ctx_t* typecheck_ctx_init(typebuilder_t* typebuilder, typetable_t* typetable, error_bag_t* errors)
{
  typecheck_ctx_t* ctx = (typecheck_ctx_t*)malloc(sizeof(typecheck_ctx_t));
  CLEAROBJ(ctx);

  ctx->typebuilder = typebuilder;
  ctx->typetable = typetable;
  ctx->errors = errors;

  return ctx;
}

void typecheck_ctx_free(typecheck_ctx_t* ctx)
{
  free(ctx);
}
