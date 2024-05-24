/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/array.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_array_t* typedesc_array_init(void)
{
  typedesc_array_t* desc = (typedesc_array_t*)malloc(sizeof(typedesc_array_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_ARRAY;

  return desc;
}

void typedesc_array_free(typedesc_array_t* desc)
{
  free(desc);
}

bool typedesc_array_is_implicitly_convertible(typedesc_array_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_OPT)
    return typedesc_is_implicitly_convertible((typedesc_t*)from_desc, typedesc_remove_opt(to_desc));
  
  if (to_desc->kind != TYPEDESC_ARRAY)
    return false;

  typedesc_array_t* target_array_desc = (typedesc_array_t*)to_desc;

  if (from_desc->length < target_array_desc->length)
    return false;
  
  if (from_desc->base_type->kind != TYPEDESC_MUT && target_array_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_remove_mut(from_desc->base_type) == typedesc_remove_mut(to_desc);
}

bool typedesc_array_is_explicitly_convertible(typedesc_array_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_OPT)
    return typedesc_is_explicitly_convertible((typedesc_t*)from_desc, typedesc_remove_opt(to_desc));

  if (to_desc->kind != TYPEDESC_ARRAY)
    return false;

  typedesc_array_t* target_array_desc = (typedesc_array_t*)to_desc;

  if (from_desc->length < target_array_desc->length)
    return false;
  
  if (from_desc->base_type->kind != TYPEDESC_MUT && target_array_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_explicitly_convertible(from_desc->base_type, target_array_desc->base_type);
}
