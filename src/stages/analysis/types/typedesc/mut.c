/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/mut.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_mut_t* typedesc_mut_init(void)
{
  typedesc_mut_t* desc = (typedesc_mut_t*)malloc(sizeof(typedesc_mut_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_MUT;

  return desc;
}

void typedesc_mut_free(typedesc_mut_t* desc)
{
  free(desc);
}

bool typedesc_mut_is_implicitly_direct_convertible(typedesc_mut_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_REF)
    return false;

  return typedesc_is_implicitly_direct_convertible(src_desc->base_type, typedesc_remove_mut(dst_desc));
}

bool typedesc_mut_is_implicitly_indirect_convertible(typedesc_mut_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_REF)
    return false;

  return typedesc_is_implicitly_indirect_convertible(src_desc->base_type, typedesc_remove_mut(dst_desc));
}

bool typedesc_mut_is_explicitly_convertible(typedesc_mut_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_REF)
    return false;

  return typedesc_is_explicitly_convertible(src_desc->base_type, typedesc_remove_mut(dst_desc));
}
