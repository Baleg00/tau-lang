/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/struct.h"

typedesc_struct_t* typedesc_struct_init(void)
{
  typedesc_struct_t* desc = (typedesc_struct_t*)malloc(sizeof(typedesc_struct_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_STRUCT;

  return desc;
}

void typedesc_struct_free(typedesc_struct_t* desc)
{
  if (desc->field_types != NULL)
    vector_free(desc->field_types);

  free(desc);
}

bool typedesc_struct_is_implicitly_direct_convertible(typedesc_struct_t* src_desc, typedesc_t* dst_desc)
{
  if (typedesc_is_opt(dst_desc))
    return typedesc_is_implicitly_direct_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  return (typedesc_t*)src_desc == dst_desc;
}

bool typedesc_struct_is_implicitly_indirect_convertible(typedesc_struct_t* src_desc, typedesc_t* dst_desc)
{
  return (typedesc_t*)src_desc == dst_desc;
}

bool typedesc_struct_is_explicitly_convertible(typedesc_struct_t* src_desc, typedesc_t* dst_desc)
{
  return (typedesc_t*)src_desc == typedesc_remove_mut(dst_desc);
}
