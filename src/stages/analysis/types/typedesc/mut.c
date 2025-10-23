/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/mut.h"

tau_typedesc_mut_t* tau_typedesc_mut_init(void)
{
  tau_typedesc_mut_t* desc = (tau_typedesc_mut_t*)malloc(sizeof(tau_typedesc_mut_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_MUT;

  return desc;
}

void tau_typedesc_mut_free(tau_typedesc_mut_t* desc)
{
  free(desc);
}

bool tau_typedesc_mut_is_implicitly_direct_convertible(tau_typedesc_mut_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_ref(dst_desc))
    return false;

  return tau_typedesc_is_implicitly_direct_convertible(src_desc->base_type, tau_typedesc_remove_mut(dst_desc));
}

bool tau_typedesc_mut_is_implicitly_indirect_convertible(tau_typedesc_mut_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_ref(dst_desc))
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(src_desc->base_type, tau_typedesc_remove_mut(dst_desc));
}

bool tau_typedesc_mut_is_explicitly_convertible(tau_typedesc_mut_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_ref(dst_desc))
    return false;

  return tau_typedesc_is_explicitly_convertible(src_desc->base_type, tau_typedesc_remove_mut(dst_desc));
}
