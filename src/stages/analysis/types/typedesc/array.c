/**
 * \file
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
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_ARRAY;

  return desc;
}

void typedesc_array_free(typedesc_array_t* desc)
{
  free(desc);
}

bool typedesc_array_is_implicitly_direct_convertible(typedesc_array_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_OPT)
    return typedesc_is_implicitly_direct_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  if (dst_desc->kind != TYPEDESC_ARRAY)
    return false;

  typedesc_array_t* dst_array_desc = (typedesc_array_t*)dst_desc;

  if (src_desc->length < dst_array_desc->length)
    return false;

  if (src_desc->base_type->kind != TYPEDESC_MUT && dst_array_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_implicitly_indirect_convertible(typedesc_remove_mut(src_desc->base_type), typedesc_remove_mut(dst_desc));
}

bool typedesc_array_is_implicitly_indirect_convertible(typedesc_array_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind != TYPEDESC_ARRAY)
    return false;

  typedesc_array_t* dst_array_desc = (typedesc_array_t*)dst_desc;

  if (src_desc->length < dst_array_desc->length)
    return false;

  if (src_desc->base_type->kind != TYPEDESC_MUT && dst_array_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_implicitly_indirect_convertible(typedesc_remove_mut(src_desc->base_type), typedesc_remove_mut(dst_desc));
}

bool typedesc_array_is_explicitly_convertible(typedesc_array_t* src_dest, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_OPT)
    return typedesc_is_explicitly_convertible((typedesc_t*)src_dest, typedesc_remove_opt(dst_desc));

  if (dst_desc->kind != TYPEDESC_ARRAY)
    return false;

  return typedesc_is_explicitly_convertible(src_dest->base_type, typedesc_remove_array(dst_desc));
}
