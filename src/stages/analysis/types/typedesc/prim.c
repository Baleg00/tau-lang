/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/prim.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

static typedesc_prim_t* typedesc_prim_init(typedesc_kind_t kind)
{
  typedesc_prim_t* desc = (typedesc_prim_t*)malloc(sizeof(typedesc_prim_t));
  clearobj(desc);

  desc->kind = kind;

  return desc;
}

typedesc_prim_t* typedesc_prim_i8_init(void)
{
  return typedesc_prim_init(TYPEDESC_I8);
}

typedesc_prim_t* typedesc_prim_i16_init(void)
{
  return typedesc_prim_init(TYPEDESC_I16);
}

typedesc_prim_t* typedesc_prim_i32_init(void)
{
  return typedesc_prim_init(TYPEDESC_I32);
}

typedesc_prim_t* typedesc_prim_i64_init(void)
{
  return typedesc_prim_init(TYPEDESC_I64);
}

typedesc_prim_t* typedesc_prim_isize_init(void)
{
  return typedesc_prim_init(TYPEDESC_ISIZE);
}

typedesc_prim_t* typedesc_prim_u8_init(void)
{
  return typedesc_prim_init(TYPEDESC_U8);
}

typedesc_prim_t* typedesc_prim_u16_init(void)
{
  return typedesc_prim_init(TYPEDESC_U16);
}

typedesc_prim_t* typedesc_prim_u32_init(void)
{
  return typedesc_prim_init(TYPEDESC_U32);
}

typedesc_prim_t* typedesc_prim_u64_init(void)
{
  return typedesc_prim_init(TYPEDESC_U64);
}

typedesc_prim_t* typedesc_prim_usize_init(void)
{
  return typedesc_prim_init(TYPEDESC_USIZE);
}

typedesc_prim_t* typedesc_prim_f32_init(void)
{
  return typedesc_prim_init(TYPEDESC_F32);
}

typedesc_prim_t* typedesc_prim_f64_init(void)
{
  return typedesc_prim_init(TYPEDESC_F64);
}

typedesc_prim_t* typedesc_prim_char_init(void)
{
  return typedesc_prim_init(TYPEDESC_CHAR);
}

typedesc_prim_t* typedesc_prim_bool_init(void)
{
  return typedesc_prim_init(TYPEDESC_BOOL);
}

typedesc_prim_t* typedesc_prim_unit_init(void)
{
  return typedesc_prim_init(TYPEDESC_UNIT);
}

void typedesc_prim_free(typedesc_prim_t* desc)
{
  free(desc);
}

bool typedesc_prim_is_implicitly_convertible(typedesc_prim_t* desc, typedesc_t* target_desc)
{
  return (typedesc_t*)desc == typedesc_remove_mut(target_desc);
}
