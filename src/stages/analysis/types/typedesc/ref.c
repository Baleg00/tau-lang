/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/ref.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_ref_t* typedesc_ref_init(void)
{
  typedesc_ref_t* desc = (typedesc_ref_t*)malloc(sizeof(typedesc_ref_t));
  clearobj(desc);

  desc->kind = TYPEDESC_REF;

  return desc;
}

void typedesc_ref_free(typedesc_ref_t* desc)
{
  free(desc);
}
