/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/union.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_union_t* typedesc_union_init(void)
{
  typedesc_union_t* desc = (typedesc_union_t*)malloc(sizeof(typedesc_union_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_UNION;

  return desc;
}

void typedesc_union_free(typedesc_union_t* desc)
{
  if (desc->field_types != NULL)
    vector_free(desc->field_types);

  free(desc);
}

bool typedesc_union_is_implicitly_convertible(typedesc_union_t* from_desc, typedesc_t* to_desc, bool through_ref)
{
  if (to_desc->kind == TYPEDESC_OPT)
  {
    if (through_ref)
      return false;

    return typedesc_is_implicitly_convertible((typedesc_t*)from_desc, typedesc_remove_opt(to_desc), through_ref);
  }

  return (typedesc_t*)from_desc == to_desc;
}

bool typedesc_union_is_explicitly_convertible(typedesc_union_t* from_desc, typedesc_t* to_desc)
{
  return (typedesc_t*)from_desc == typedesc_remove_mut(to_desc);
}
