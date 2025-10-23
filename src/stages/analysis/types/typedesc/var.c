/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/var.h"

tau_typedesc_var_t* tau_typedesc_var_init(void)
{
  tau_typedesc_var_t* desc = (tau_typedesc_var_t*)malloc(sizeof(tau_typedesc_var_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_VAR;

  return desc;
}

void tau_typedesc_var_free(tau_typedesc_var_t* desc)
{
  free(desc);
}
