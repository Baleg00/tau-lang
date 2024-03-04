/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/fun.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_fun_t* typedesc_fun_init(void)
{
  typedesc_fun_t* desc = (typedesc_fun_t*)malloc(sizeof(typedesc_fun_t));
  clearobj(desc);

  desc->kind = TYPEDESC_FUN;

  return desc;
}

void typedesc_fun_free(typedesc_fun_t* desc)
{
  vector_free(desc->param_types);
  free(desc);
}

bool typedesc_fun_is_implicitly_convertible(typedesc_fun_t* desc, typedesc_t* target_desc)
{
  return (typedesc_t*)desc == target_desc;
}
