/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/vec.h"

tau_typedesc_vec_t* tau_typedesc_vec_init(void)
{
  tau_typedesc_vec_t* desc = (tau_typedesc_vec_t*)malloc(sizeof(tau_typedesc_vec_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_VEC;

  return desc;
}

void tau_typedesc_vec_free(tau_typedesc_vec_t* desc)
{
  free(desc);
}

bool tau_typedesc_vec_is_implicitly_direct_convertible(tau_typedesc_vec_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)src_desc, tau_typedesc_remove_opt(dst_desc));

  if (!tau_typedesc_is_vector(dst_desc))
    return false;

  tau_typedesc_vec_t* dst_vec_desc = (tau_typedesc_vec_t*)dst_desc;

  if (src_desc->size != dst_vec_desc->size)
    return false;

  return tau_typedesc_is_implicitly_direct_convertible(src_desc->base_type, dst_vec_desc->base_type);
}

bool tau_typedesc_vec_is_implicitly_indirect_convertible(tau_typedesc_vec_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_vector(dst_desc))
    return false;

  tau_typedesc_vec_t* dst_vec_desc = (tau_typedesc_vec_t*)dst_desc;

  if (src_desc->size != dst_vec_desc->size)
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(src_desc->base_type, dst_vec_desc->base_type);
}

bool tau_typedesc_vec_is_explicitly_convertible(tau_typedesc_vec_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_explicitly_convertible((tau_typedesc_t*)src_desc, tau_typedesc_remove_opt(dst_desc));

  if (!tau_typedesc_is_vector(dst_desc))
    return false;

  tau_typedesc_vec_t* dst_vec_desc = (tau_typedesc_vec_t*)dst_desc;

  return tau_typedesc_is_explicitly_convertible(src_desc->base_type, dst_vec_desc->base_type);
}
