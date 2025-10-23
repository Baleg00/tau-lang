/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/ref.h"

tau_typedesc_ref_t* tau_typedesc_ref_init(void)
{
  tau_typedesc_ref_t* desc = (tau_typedesc_ref_t*)malloc(sizeof(tau_typedesc_ref_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_REF;

  return desc;
}

void tau_typedesc_ref_free(tau_typedesc_ref_t* desc)
{
  free(desc);
}

bool tau_typedesc_ref_is_implicitly_direct_convertible(tau_typedesc_ref_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_ref(dst_desc))
    return tau_typedesc_is_implicitly_direct_convertible(src_desc->base_type, dst_desc);

  tau_typedesc_ref_t* dst_ref_desc = (tau_typedesc_ref_t*)dst_desc;

  if (!tau_typedesc_is_mut(src_desc->base_type) && tau_typedesc_is_mut(dst_ref_desc->base_type))
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(tau_typedesc_remove_mut(src_desc->base_type), tau_typedesc_remove_mut(dst_ref_desc->base_type));
}

bool tau_typedesc_ref_is_implicitly_indirect_convertible(tau_typedesc_ref_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_ref(dst_desc))
    return false;

  tau_typedesc_ref_t* dst_ref_desc = (tau_typedesc_ref_t*)dst_desc;

  if (!tau_typedesc_is_mut(src_desc->base_type) && tau_typedesc_is_mut(dst_ref_desc->base_type))
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(tau_typedesc_remove_mut(src_desc->base_type), tau_typedesc_remove_mut(dst_ref_desc->base_type));
}

bool tau_typedesc_ref_is_explicitly_convertible(tau_typedesc_ref_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_explicitly_convertible(src_desc->base_type, dst_desc);

  if (!tau_typedesc_is_ref(dst_desc))
    return tau_typedesc_is_explicitly_convertible(src_desc->base_type, dst_desc);

  return tau_typedesc_is_explicitly_convertible(src_desc->base_type, tau_typedesc_remove_ref(dst_desc));
}
