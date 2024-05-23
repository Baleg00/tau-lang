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
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_OPT;

  return desc;
}

void typedesc_opt_free(typedesc_opt_t* desc)
{
  free(desc);
}

bool typedesc_opt_is_implicitly_convertible(typedesc_opt_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind != TYPEDESC_OPT)
    return false;

  return typedesc_is_implicitly_convertible(from_desc->base_type, typedesc_remove_opt(to_desc));
}
