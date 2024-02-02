/**
 * \file array.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/array.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_array_t* typedesc_array_init(void)
{
  typedesc_array_t* desc = (typedesc_array_t*)malloc(sizeof(typedesc_array_t));
  clearobj(desc);

  desc->kind = TYPEDESC_ARRAY;

  return desc;
}

void typedesc_array_free(typedesc_array_t* desc)
{
  free(desc);
}
