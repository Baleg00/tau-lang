/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/vec.h"

typedesc_vec_t* typedesc_vec_init(void)
{
  typedesc_vec_t* desc = (typedesc_vec_t*)malloc(sizeof(typedesc_vec_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_VEC;

  return desc;
}

void typedesc_vec_free(typedesc_vec_t* desc)
{
  free(desc);
}

bool typedesc_vec_is_implicitly_direct_convertible(typedesc_vec_t* src_desc, typedesc_t* dst_desc)
{
  if (typedesc_is_opt(dst_desc))
    return typedesc_is_implicitly_direct_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  if (!typedesc_is_vector(dst_desc))
    return false;

  typedesc_vec_t* dst_vec_desc = (typedesc_vec_t*)dst_desc;

  if (src_desc->size != dst_vec_desc->size)
    return false;

  return typedesc_is_implicitly_direct_convertible(src_desc->base_type, dst_vec_desc->base_type);
}

bool typedesc_vec_is_implicitly_indirect_convertible(typedesc_vec_t* src_desc, typedesc_t* dst_desc)
{
  if (!typedesc_is_vector(dst_desc))
    return false;

  typedesc_vec_t* dst_vec_desc = (typedesc_vec_t*)dst_desc;

  if (src_desc->size != dst_vec_desc->size)
    return false;

  return typedesc_is_implicitly_indirect_convertible(src_desc->base_type, dst_vec_desc->base_type);
}

bool typedesc_vec_is_explicitly_convertible(typedesc_vec_t* src_desc, typedesc_t* dst_desc)
{
  if (typedesc_is_opt(dst_desc))
    return typedesc_is_explicitly_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  if (!typedesc_is_vector(dst_desc))
    return false;

  typedesc_vec_t* dst_vec_desc = (typedesc_vec_t*)dst_desc;

  return typedesc_is_explicitly_convertible(src_desc->base_type, dst_vec_desc->base_type);
}
