/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/struct.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_struct_t* typedesc_struct_init(void)
{
  typedesc_struct_t* desc = (typedesc_struct_t*)malloc(sizeof(typedesc_struct_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_STRUCT;

  return desc;
}

void typedesc_struct_free(typedesc_struct_t* desc)
{
  vector_free(desc->field_types);
  free(desc);
}

bool typedesc_struct_is_implicitly_convertible(typedesc_struct_t* desc, typedesc_t* target_desc)
{
  return (typedesc_t*)desc == typedesc_remove_mut(target_desc);
}
