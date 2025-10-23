/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/poison.h"

tau_typedesc_poison_t* tau_typedesc_poison_init(void)
{
  tau_typedesc_poison_t* desc = (tau_typedesc_poison_t*)malloc(sizeof(tau_typedesc_poison_t));
  TAU_CLEAROBJ(desc);

  desc->kind = TAU_TYPEDESC_POISON;

  return desc;
}

void tau_typedesc_poison_free(tau_typedesc_poison_t* desc)
{
  free(desc);
}

bool tau_typedesc_poison_is_implicitly_direct_convertible(tau_typedesc_poison_t* TAU_UNUSED(src_desc), tau_typedesc_t* TAU_UNUSED(dst_desc))
{
  return false;
}

bool tau_typedesc_poison_is_implicitly_indirect_convertible(tau_typedesc_poison_t* TAU_UNUSED(src_desc), tau_typedesc_t* TAU_UNUSED(dst_desc))
{
  return false;
}

bool tau_typedesc_poison_is_explicitly_convertible(tau_typedesc_poison_t* TAU_UNUSED(src_desc), tau_typedesc_t* TAU_UNUSED(dst_desc))
{
  return false;
}
