/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/prim.h"

static typedesc_prim_t* typedesc_prim_init(typedesc_kind_t kind)
{
  typedesc_prim_t* desc = (typedesc_prim_t*)malloc(sizeof(typedesc_prim_t));
  CLEAROBJ(desc);

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

typedesc_prim_t* typedesc_prim_c64_init(void)
{
  return typedesc_prim_init(TYPEDESC_C64);
}

typedesc_prim_t* typedesc_prim_c128_init(void)
{
  return typedesc_prim_init(TYPEDESC_C128);
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

bool typedesc_prim_is_implicitly_direct_convertible(typedesc_prim_t* src_desc, typedesc_t* dst_desc)
{
  if (typedesc_is_opt(dst_desc))
    return typedesc_is_implicitly_direct_convertible((typedesc_t*)src_desc, typedesc_remove_opt(dst_desc));

  dst_desc = typedesc_remove_mut(dst_desc);

  if ((typedesc_t*)src_desc == dst_desc)
    return true;

  if (typedesc_is_arithmetic((typedesc_t*)src_desc) && typedesc_is_arithmetic(dst_desc))
  {
    if (typedesc_is_float((typedesc_t*)src_desc))
    {
      if (typedesc_is_float(dst_desc))
        return !(src_desc->kind == TYPEDESC_F64 && dst_desc->kind == TYPEDESC_F32);

      if (typedesc_is_integer(dst_desc))
        return false;

      if (typedesc_is_complex(dst_desc))
        return !(src_desc->kind == TYPEDESC_F64 && dst_desc->kind == TYPEDESC_C64);
    }

    if (typedesc_is_integer((typedesc_t*)src_desc))
    {
      if (typedesc_is_integer(dst_desc))
      {
        if (typedesc_is_signed((typedesc_t*)src_desc) == typedesc_is_signed(dst_desc))
          return typedesc_integer_bits((typedesc_t*)src_desc) <= typedesc_integer_bits(dst_desc);

        if (typedesc_is_signed((typedesc_t*)src_desc) && typedesc_is_unsigned(dst_desc))
          return false;

        return typedesc_integer_bits((typedesc_t*)src_desc) < typedesc_integer_bits(dst_desc);
      }

      if (dst_desc->kind == TYPEDESC_F32)
        return typedesc_integer_bits((typedesc_t*)src_desc) <= 16;

      if (dst_desc->kind == TYPEDESC_F64)
        return typedesc_integer_bits((typedesc_t*)src_desc) <= 32;

      if (dst_desc->kind == TYPEDESC_C64)
        return typedesc_integer_bits((typedesc_t*)src_desc) <= 16;

      if (dst_desc->kind == TYPEDESC_C128)
        return typedesc_integer_bits((typedesc_t*)src_desc) <= 32;
    }
  }

  return false;
}

bool typedesc_prim_is_implicitly_indirect_convertible(typedesc_prim_t* src_desc, typedesc_t* dst_desc)
{
  return (typedesc_t*)src_desc == dst_desc;
}

bool typedesc_prim_is_explicitly_convertible(typedesc_prim_t* UNUSED(src_desc), typedesc_t* dst_desc)
{
  return typedesc_is_arithmetic(typedesc_remove_mut(dst_desc));
}
