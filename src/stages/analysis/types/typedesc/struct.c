/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/struct.h"

tau_typedesc_struct_t* tau_typedesc_struct_init(void)
{
  tau_typedesc_struct_t* desc = (tau_typedesc_struct_t*)malloc(sizeof(tau_typedesc_struct_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_STRUCT;

  return desc;
}

void tau_typedesc_struct_free(tau_typedesc_struct_t* desc)
{
  if (desc->field_types != NULL)
    tau_vector_free(desc->field_types);

  free(desc);
}

bool tau_typedesc_struct_is_implicitly_direct_convertible(tau_typedesc_struct_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)src_desc, tau_typedesc_remove_opt(dst_desc));

  return (tau_typedesc_t*)src_desc == dst_desc;
}

bool tau_typedesc_struct_is_implicitly_indirect_convertible(tau_typedesc_struct_t* src_desc, tau_typedesc_t* dst_desc)
{
  return (tau_typedesc_t*)src_desc == dst_desc;
}

bool tau_typedesc_struct_is_explicitly_convertible(tau_typedesc_struct_t* src_desc, tau_typedesc_t* dst_desc)
{
  return (tau_typedesc_t*)src_desc == tau_typedesc_remove_mut(dst_desc);
}
