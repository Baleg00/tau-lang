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
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_REF;

  return desc;
}

void typedesc_ref_free(typedesc_ref_t* desc)
{
  free(desc);
}

bool typedesc_ref_is_implicitly_convertible(typedesc_ref_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind != TYPEDESC_REF)
    return typedesc_is_implicitly_convertible(from_desc->base_type, to_desc);

  typedesc_ref_t* target_ref_desc = (typedesc_ref_t*)to_desc;

  if (from_desc->base_type->kind != TYPEDESC_MUT && target_ref_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_implicitly_convertible(from_desc->base_type, typedesc_remove_ref(to_desc));
}
