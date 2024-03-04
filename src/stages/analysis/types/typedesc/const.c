/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/const.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_const_t* typedesc_const_init(void)
{
  typedesc_const_t* desc = (typedesc_const_t*)malloc(sizeof(typedesc_const_t));
  clearobj(desc);

  desc->kind = TYPEDESC_CONST;

  return desc;
}

void typedesc_const_free(typedesc_const_t* desc)
{
  free(desc);
}

bool typedesc_const_is_implicitly_convertible(typedesc_const_t* desc, typedesc_t* target_desc)
{
  return typedesc_is_implicitly_convertible(desc->base_type, typedesc_remove_const(target_desc));
}
