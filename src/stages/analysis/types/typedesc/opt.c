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

bool typedesc_opt_is_implicitly_direct_convertible(typedesc_opt_t* src_desc, typedesc_t* dst_desc)
{
  if (!typedesc_is_opt(dst_desc))
    return false;

  return typedesc_is_implicitly_indirect_convertible(src_desc->base_type, typedesc_remove_opt(dst_desc));
}

bool typedesc_opt_is_implicitly_indirect_convertible(typedesc_opt_t* src_desc, typedesc_t* dst_desc)
{
  if (!typedesc_is_opt(dst_desc))
    return false;

  return typedesc_is_implicitly_indirect_convertible(src_desc->base_type, typedesc_remove_opt(dst_desc));
}

bool typedesc_opt_is_explicitly_convertible(typedesc_opt_t* src_desc, typedesc_t* dst_desc)
{
  if (!typedesc_is_opt(dst_desc))
    return false;

  return typedesc_is_explicitly_convertible(src_desc->base_type, typedesc_remove_opt(dst_desc));
}
