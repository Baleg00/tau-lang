/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/enum.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_enum_t* typedesc_enum_init(void)
{
  typedesc_enum_t* desc = (typedesc_enum_t*)malloc(sizeof(typedesc_enum_t));
  clearobj(desc);

  desc->kind = TYPEDESC_ENUM;

  return desc;
}

void typedesc_enum_free(typedesc_enum_t* desc)
{
  free(desc);
}
