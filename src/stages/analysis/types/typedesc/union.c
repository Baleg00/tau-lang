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

bool typedesc_union_is_implicitly_direct_convertible(typedesc_union_t* src_desc, typedesc_t* dst_desc)
{
  if (dst_desc->kind == TYPEDESC_OPT)
    return typedesc_is_implicitly_direct_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  return (typedesc_t*)src_desc == dst_desc;
}

bool typedesc_union_is_implicitly_indirect_convertible(typedesc_union_t* src_desc, typedesc_t* dst_desc)
{
  return (typedesc_t*)src_desc == dst_desc;
}

bool typedesc_union_is_explicitly_convertible(typedesc_union_t* src_desc, typedesc_t* dst_desc)
{
  return (typedesc_t*)src_desc == typedesc_remove_mut(dst_desc);
}
