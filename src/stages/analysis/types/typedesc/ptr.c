/**
 * \file ptr.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/ptr.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_ptr_t* typedesc_ptr_init(void)
{
  typedesc_ptr_t* desc = (typedesc_ptr_t*)malloc(sizeof(typedesc_ptr_t));
  clearobj(desc);

  desc->kind = TYPEDESC_PTR;

  return desc;
}

void typedesc_ptr_free(typedesc_ptr_t* desc)
{
  free(desc);
}
