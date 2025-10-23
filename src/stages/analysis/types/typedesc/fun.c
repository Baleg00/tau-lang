/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/fun.h"

tau_typedesc_fun_t* tau_typedesc_fun_init(void)
{
  tau_typedesc_fun_t* desc = (tau_typedesc_fun_t*)malloc(sizeof(tau_typedesc_fun_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_FUN;

  return desc;
}

void tau_typedesc_fun_free(tau_typedesc_fun_t* desc)
{
  if (desc->param_types != NULL)
    tau_vector_free(desc->param_types);

  free(desc);
}

bool tau_typedesc_fun_is_implicitly_direct_convertible(tau_typedesc_fun_t* src_desc, tau_typedesc_t* dst_desc)
{
  return (tau_typedesc_t*)src_desc == dst_desc;
}

bool tau_typedesc_fun_is_implicitly_indirect_convertible(tau_typedesc_fun_t* src_desc, tau_typedesc_t* dst_desc)
{
  return (tau_typedesc_t*)src_desc == dst_desc;
}

bool tau_typedesc_fun_is_explicitly_convertible(tau_typedesc_fun_t* src_desc, tau_typedesc_t* dst_desc)
{
  return (tau_typedesc_t*)src_desc == dst_desc;
}
