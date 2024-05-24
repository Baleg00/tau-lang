/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/ptr.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_ptr_t* typedesc_ptr_init(void)
{
  typedesc_ptr_t* desc = (typedesc_ptr_t*)malloc(sizeof(typedesc_ptr_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_PTR;

  return desc;
}

void typedesc_ptr_free(typedesc_ptr_t* desc)
{
  free(desc);
}

bool typedesc_ptr_is_implicitly_convertible(typedesc_ptr_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_OPT)
    return typedesc_is_implicitly_convertible((typedesc_t*)from_desc, typedesc_remove_opt(to_desc));

  if (to_desc->kind != TYPEDESC_PTR)
    return false;

  typedesc_ptr_t* target_ptr_desc = (typedesc_ptr_t*)to_desc;

  if (from_desc->base_type->kind != TYPEDESC_MUT && target_ptr_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_remove_mut(from_desc->base_type) == typedesc_remove_mut(to_desc);
}

bool typedesc_ptr_is_explicitly_convertible(typedesc_ptr_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_OPT)
    return typedesc_is_explicitly_convertible((typedesc_t*)from_desc, typedesc_remove_opt(to_desc));
  
  if (to_desc->kind != TYPEDESC_PTR)
    return false;

  typedesc_ptr_t* target_ptr_desc = (typedesc_ptr_t*)to_desc;

  if (from_desc->base_type->kind != TYPEDESC_MUT && target_ptr_desc->base_type->kind == TYPEDESC_MUT)
    return false;

  return typedesc_is_explicitly_convertible(from_desc->base_type, typedesc_remove_ptr(to_desc));
}
