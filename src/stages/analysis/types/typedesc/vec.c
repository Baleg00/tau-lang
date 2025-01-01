/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/vec.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

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

bool typedesc_vec_is_implicitly_convertible(typedesc_vec_t* from_desc, typedesc_t* to_desc, bool through_ref)
{
  if (to_desc->kind == TYPEDESC_OPT)
  {
    if (through_ref)
      return false;

    return typedesc_is_implicitly_convertible((typedesc_t*)from_desc, typedesc_remove_opt(to_desc), through_ref);
  }

  if (to_desc->kind != TYPEDESC_ARRAY)
    return false;

  typedesc_vec_t* target_array_desc = (typedesc_vec_t*)to_desc;

  if (from_desc->size < target_array_desc->size)
    return false;

  if (from_desc->base_type->kind != TYPEDESC_MUT && target_array_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_implicitly_convertible(typedesc_remove_mut(from_desc->base_type), typedesc_remove_mut(to_desc), true);
}

bool typedesc_vec_is_explicitly_convertible(typedesc_vec_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_OPT)
    return typedesc_is_explicitly_convertible((typedesc_t*)from_desc, typedesc_remove_opt(to_desc));

  if (to_desc->kind != TYPEDESC_ARRAY)
    return false;

  typedesc_vec_t* target_array_desc = (typedesc_vec_t*)to_desc;

  if (from_desc->size < target_array_desc->size)
    return false;

  if (from_desc->base_type->kind != TYPEDESC_MUT && target_array_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_explicitly_convertible(from_desc->base_type, target_array_desc->base_type);
}
