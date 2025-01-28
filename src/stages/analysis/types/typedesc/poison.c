/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/poison.h"

typedesc_poison_t* typedesc_poison_init(void)
{
  typedesc_poison_t* desc = (typedesc_poison_t*)malloc(sizeof(typedesc_poison_t));
  CLEAROBJ(desc);

  desc->kind = TYPEDESC_POISON;

  return desc;
}

void typedesc_poison_free(typedesc_poison_t* desc)
{
  free(desc);
}

bool typedesc_poison_is_implicitly_direct_convertible(typedesc_poison_t* UNUSED(src_desc), typedesc_t* UNUSED(dst_desc))
{
  return false;
}

bool typedesc_poison_is_implicitly_indirect_convertible(typedesc_poison_t* UNUSED(src_desc), typedesc_t* UNUSED(dst_desc))
{
  return false;
}

bool typedesc_poison_is_explicitly_convertible(typedesc_poison_t* UNUSED(src_desc), typedesc_t* UNUSED(dst_desc))
{
  return false;
}
