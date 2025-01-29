/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/mat.h"

typedesc_mat_t* typedesc_mat_init(void)
{
  typedesc_mat_t* desc = (typedesc_mat_t*)malloc(sizeof(typedesc_mat_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_MAT;

  return desc;
}

void typedesc_mat_free(typedesc_mat_t* desc)
{
  free(desc);
}

bool typedesc_mat_is_implicitly_direct_convertible(typedesc_mat_t* src_desc, typedesc_t* dst_desc)
{
  if (typedesc_is_opt(dst_desc))
    return typedesc_is_implicitly_direct_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  if (!typedesc_is_matrix(dst_desc))
    return false;

  typedesc_mat_t* dst_mat_desc = (typedesc_mat_t*)dst_desc;

  if (src_desc->rows != dst_mat_desc->rows || src_desc->cols != dst_mat_desc->cols)
    return false;

  return typedesc_is_implicitly_direct_convertible(src_desc->base_type, dst_mat_desc->base_type);
}

bool typedesc_mat_is_implicitly_indirect_convertible(typedesc_mat_t* src_desc, typedesc_t* dst_desc)
{
  if (!typedesc_is_matrix(dst_desc))
    return false;

  typedesc_mat_t* dst_mat_desc = (typedesc_mat_t*)dst_desc;

  if (src_desc->rows != dst_mat_desc->rows || src_desc->cols != dst_mat_desc->cols)
    return false;

  return typedesc_is_implicitly_indirect_convertible(src_desc->base_type, dst_mat_desc->base_type);
}

bool typedesc_mat_is_explicitly_convertible(typedesc_mat_t* src_desc, typedesc_t* dst_desc)
{
  if (typedesc_is_opt(dst_desc))
    return typedesc_is_explicitly_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  if (!typedesc_is_matrix(dst_desc))
    return false;

  typedesc_mat_t* dst_mat_desc = (typedesc_mat_t*)dst_desc;

  return typedesc_is_explicitly_convertible(src_desc->base_type, dst_mat_desc->base_type);
}
