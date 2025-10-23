/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/mat.h"

tau_typedesc_mat_t* tau_typedesc_mat_init(void)
{
  tau_typedesc_mat_t* desc = (tau_typedesc_mat_t*)malloc(sizeof(tau_typedesc_mat_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_MAT;

  return desc;
}

void tau_typedesc_mat_free(tau_typedesc_mat_t* desc)
{
  free(desc);
}

bool tau_typedesc_mat_is_implicitly_direct_convertible(tau_typedesc_mat_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)src_desc, tau_typedesc_remove_opt(dst_desc));

  if (!tau_typedesc_is_matrix(dst_desc))
    return false;

  tau_typedesc_mat_t* dst_mat_desc = (tau_typedesc_mat_t*)dst_desc;

  if (src_desc->rows != dst_mat_desc->rows || src_desc->cols != dst_mat_desc->cols)
    return false;

  return tau_typedesc_is_implicitly_direct_convertible(src_desc->base_type, dst_mat_desc->base_type);
}

bool tau_typedesc_mat_is_implicitly_indirect_convertible(tau_typedesc_mat_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_matrix(dst_desc))
    return false;

  tau_typedesc_mat_t* dst_mat_desc = (tau_typedesc_mat_t*)dst_desc;

  if (src_desc->rows != dst_mat_desc->rows || src_desc->cols != dst_mat_desc->cols)
    return false;

  return tau_typedesc_is_implicitly_indirect_convertible(src_desc->base_type, dst_mat_desc->base_type);
}

bool tau_typedesc_mat_is_explicitly_convertible(tau_typedesc_mat_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_explicitly_convertible((tau_typedesc_t*)src_desc, tau_typedesc_remove_opt(dst_desc));

  if (!tau_typedesc_is_matrix(dst_desc))
    return false;

  tau_typedesc_mat_t* dst_mat_desc = (tau_typedesc_mat_t*)dst_desc;

  return tau_typedesc_is_explicitly_convertible(src_desc->base_type, dst_mat_desc->base_type);
}
