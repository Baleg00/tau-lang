/**
 * \file
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
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_PTR;

  return desc;
}

void typedesc_ptr_free(typedesc_ptr_t* desc)
{
  free(desc);
}

bool typedesc_ptr_is_implicitly_direct_convertible(typedesc_ptr_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_OPT)
    return typedesc_is_implicitly_direct_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  if (dst_desc->kind != TYPEDESC_PTR)
    return false;

  typedesc_ptr_t* dst_ptr_desc = (typedesc_ptr_t*)dst_desc;

  if (src_desc->base_type->kind != TYPEDESC_MUT && dst_ptr_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_implicitly_indirect_convertible(typedesc_remove_mut(src_desc->base_type), typedesc_remove_mut(dst_ptr_desc->base_type));
}

bool typedesc_ptr_is_implicitly_indirect_convertible(typedesc_ptr_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind != TYPEDESC_PTR)
    return false;

  typedesc_ptr_t* dst_ptr_desc = (typedesc_ptr_t*)dst_desc;

  if (src_desc->base_type->kind != TYPEDESC_MUT && dst_ptr_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_implicitly_indirect_convertible(typedesc_remove_mut(src_desc->base_type), typedesc_remove_mut(dst_ptr_desc->base_type));
}

bool typedesc_ptr_is_explicitly_convertible(typedesc_ptr_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_OPT)
    return typedesc_is_explicitly_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  if (dst_desc->kind != TYPEDESC_PTR)
    return false;

  return typedesc_is_explicitly_convertible(src_desc->base_type, typedesc_remove_ptr(dst_desc));
}
