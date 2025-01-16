/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/var.h"

typedesc_var_t* typedesc_var_init(void)
{
  typedesc_var_t* desc = (typedesc_var_t*)malloc(sizeof(typedesc_var_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_VAR;

  return desc;
}

void typedesc_var_free(typedesc_var_t* desc)
{
  free(desc);
}
