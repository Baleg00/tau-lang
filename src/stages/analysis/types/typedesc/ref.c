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

bool typedesc_ref_is_implicitly_direct_convertible(typedesc_ref_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind != TYPEDESC_REF)
    return typedesc_is_implicitly_direct_convertible(src_desc->base_type, dst_desc);

  typedesc_ref_t* dst_ref_desc = (typedesc_ref_t*)dst_desc;

  if (src_desc->base_type->kind != TYPEDESC_MUT && dst_ref_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_implicitly_indirect_convertible(typedesc_remove_mut(src_desc->base_type), typedesc_remove_mut(dst_ref_desc->base_type));
}

bool typedesc_ref_is_implicitly_indirect_convertible(typedesc_ref_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind != TYPEDESC_REF)
    return false;

  typedesc_ref_t* dst_ref_desc = (typedesc_ref_t*)dst_desc;

  if (src_desc->base_type->kind != TYPEDESC_MUT && dst_ref_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_implicitly_indirect_convertible(typedesc_remove_mut(src_desc->base_type), typedesc_remove_mut(dst_ref_desc->base_type));
}

bool typedesc_ref_is_explicitly_convertible(typedesc_ref_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_OPT)
    return typedesc_is_explicitly_convertible(src_desc->base_type, dst_desc);

  if (dst_desc->kind != TYPEDESC_REF)
    return typedesc_is_explicitly_convertible(src_desc->base_type, dst_desc);

  return typedesc_is_explicitly_convertible(src_desc->base_type, typedesc_remove_ref(dst_desc));
}
