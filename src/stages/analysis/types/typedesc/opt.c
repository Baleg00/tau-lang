/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/opt.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_opt_t* typedesc_opt_init(void)
{
  typedesc_opt_t* desc = (typedesc_opt_t*)malloc(sizeof(typedesc_opt_t));
  clearobj(desc);

  desc->kind = TYPEDESC_OPT;

  return desc;
}

void typedesc_opt_free(typedesc_opt_t* desc)
{
  free(desc);
}

bool typedesc_opt_is_implicitly_convertible(typedesc_opt_t* desc, typedesc_t* target_desc)
{
  if (target_desc->kind != TYPEDESC_OPT)
    return false;

  return typedesc_is_implicitly_convertible(desc->base_type, typedesc_remove_opt(target_desc));
}
