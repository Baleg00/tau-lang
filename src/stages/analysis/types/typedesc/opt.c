/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/opt.h"

tau_typedesc_opt_t* tau_typedesc_opt_init(void)
{
  tau_typedesc_opt_t* desc = (tau_typedesc_opt_t*)malloc(sizeof(tau_typedesc_opt_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_OPT;

  return desc;
}

void tau_typedesc_opt_free(tau_typedesc_opt_t* desc)
{
  free(desc);
}

bool tau_typedesc_opt_is_implicitly_direct_convertible(tau_typedesc_opt_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_opt(dst_desc))
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(src_desc->base_type, tau_typedesc_remove_opt(dst_desc));
}

bool tau_typedesc_opt_is_implicitly_indirect_convertible(tau_typedesc_opt_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_opt(dst_desc))
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(src_desc->base_type, tau_typedesc_remove_opt(dst_desc));
}

bool tau_typedesc_opt_is_explicitly_convertible(tau_typedesc_opt_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_opt(dst_desc))
    return false;

  return tau_typedesc_is_explicitly_convertible(src_desc->base_type, tau_typedesc_remove_opt(dst_desc));
}
