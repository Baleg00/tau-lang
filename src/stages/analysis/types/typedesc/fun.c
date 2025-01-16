/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/fun.h"

typedesc_fun_t* typedesc_fun_init(void)
{
  typedesc_fun_t* desc = (typedesc_fun_t*)malloc(sizeof(typedesc_fun_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_FUN;

  return desc;
}

void typedesc_fun_free(typedesc_fun_t* desc)
{
  if (desc->param_types != NULL)
    vector_free(desc->param_types);

  free(desc);
}

bool typedesc_fun_is_implicitly_direct_convertible(typedesc_fun_t* src_desc, typedesc_t* dst_desc)
{
  return (typedesc_t*)src_desc == dst_desc;
}

bool typedesc_fun_is_implicitly_indirect_convertible(typedesc_fun_t* src_desc, typedesc_t* dst_desc)
{
  return (typedesc_t*)src_desc == dst_desc;
}

bool typedesc_fun_is_explicitly_convertible(typedesc_fun_t* src_desc, typedesc_t* dst_desc)
{
  return (typedesc_t*)src_desc == dst_desc;
}
