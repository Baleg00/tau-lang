/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/array.h"

tau_typedesc_array_t* tau_typedesc_array_init(void)
{
  tau_typedesc_array_t* desc = (tau_typedesc_array_t*)malloc(sizeof(tau_typedesc_array_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_ARRAY;

  return desc;
}

void tau_typedesc_array_free(tau_typedesc_array_t* desc)
{
  free(desc);
}

bool tau_typedesc_array_is_implicitly_direct_convertible(tau_typedesc_array_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)src_desc, tau_typedesc_remove_opt(dst_desc));

  if (!tau_typedesc_is_array(dst_desc))
    return false;

  tau_typedesc_array_t* dst_array_desc = (tau_typedesc_array_t*)dst_desc;

  if (src_desc->length < dst_array_desc->length)
    return false;

  if (!tau_typedesc_is_mut(src_desc->base_type) && tau_typedesc_is_mut(dst_array_desc->base_type))
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(tau_typedesc_remove_mut(src_desc->base_type), tau_typedesc_remove_mut(dst_desc));
}

bool tau_typedesc_array_is_implicitly_indirect_convertible(tau_typedesc_array_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_array(dst_desc))
    return false;

  tau_typedesc_array_t* dst_array_desc = (tau_typedesc_array_t*)dst_desc;

  if (src_desc->length < dst_array_desc->length)
    return false;

  if (!tau_typedesc_is_mut(src_desc->base_type) && tau_typedesc_is_mut(dst_array_desc->base_type))
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(tau_typedesc_remove_mut(src_desc->base_type), tau_typedesc_remove_mut(dst_desc));
}

bool tau_typedesc_array_is_explicitly_convertible(tau_typedesc_array_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_explicitly_convertible((tau_typedesc_t*)src_desc, tau_typedesc_remove_opt(dst_desc));

  if (!tau_typedesc_is_array(dst_desc))
    return false;

  return tau_typedesc_is_explicitly_convertible(src_desc->base_type, tau_typedesc_remove_array(dst_desc));
}
