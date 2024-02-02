/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/null.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_null_t* typedesc_null_init(void)
{
  typedesc_null_t* desc = (typedesc_null_t*)malloc(sizeof(typedesc_null_t));
  clearobj(desc);

  desc->kind = TYPEDESC_NULL;

  return desc;
}

void typedesc_null_free(typedesc_null_t* desc)
{
  free(desc);
}
