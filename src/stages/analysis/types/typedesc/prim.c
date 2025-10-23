/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/prim.h"

static tau_typedesc_prim_t* tau_typedesc_prim_init(tau_typedesc_kind_t kind)
{
  tau_typedesc_prim_t* desc = (tau_typedesc_prim_t*)malloc(sizeof(tau_typedesc_prim_t));
  TAU_CLEAROBJ(desc);

  desc->kind = kind;

  return desc;
}

tau_typedesc_prim_t* tau_typedesc_prim_i8_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_I8);
}

tau_typedesc_prim_t* tau_typedesc_prim_i16_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_I16);
}

tau_typedesc_prim_t* tau_typedesc_prim_i32_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_I32);
}

tau_typedesc_prim_t* tau_typedesc_prim_i64_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_I64);
}

tau_typedesc_prim_t* tau_typedesc_prim_isize_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_ISIZE);
}

tau_typedesc_prim_t* tau_typedesc_prim_u8_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_U8);
}

tau_typedesc_prim_t* tau_typedesc_prim_u16_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_U16);
}

tau_typedesc_prim_t* tau_typedesc_prim_u32_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_U32);
}

tau_typedesc_prim_t* tau_typedesc_prim_u64_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_U64);
}

tau_typedesc_prim_t* tau_typedesc_prim_usize_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_USIZE);
}

tau_typedesc_prim_t* tau_typedesc_prim_f32_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_F32);
}

tau_typedesc_prim_t* tau_typedesc_prim_f64_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_F64);
}

tau_typedesc_prim_t* tau_typedesc_prim_c64_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_C64);
}

tau_typedesc_prim_t* tau_typedesc_prim_c128_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_C128);
}

tau_typedesc_prim_t* tau_typedesc_prim_char_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_CHAR);
}

tau_typedesc_prim_t* tau_typedesc_prim_bool_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_BOOL);
}

tau_typedesc_prim_t* tau_typedesc_prim_unit_init(void)
{
  return tau_typedesc_prim_init(TAU_TYPEDESC_UNIT);
}

void tau_typedesc_prim_free(tau_typedesc_prim_t* desc)
{
  free(desc);
}

bool tau_typedesc_prim_is_implicitly_direct_convertible(tau_typedesc_prim_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_typedesc_is_implicitly_direct_convertible((tau_typedesc_t*)src_desc, tau_typedesc_remove_opt(dst_desc));

  dst_desc = tau_typedesc_remove_mut(dst_desc);

  if ((tau_typedesc_t*)src_desc == dst_desc)
    return true;

  if (tau_typedesc_is_arithmetic((tau_typedesc_t*)src_desc) && tau_typedesc_is_arithmetic(dst_desc))
  {
    if (tau_typedesc_is_float((tau_typedesc_t*)src_desc))
    {
      if (tau_typedesc_is_float(dst_desc))
        return !(src_desc->kind == TAU_TYPEDESC_F64 && dst_desc->kind == TAU_TYPEDESC_F32);

      if (tau_typedesc_is_integer(dst_desc))
        return false;

      if (tau_typedesc_is_complex(dst_desc))
        return !(src_desc->kind == TAU_TYPEDESC_F64 && dst_desc->kind == TAU_TYPEDESC_C64);
    }

    if (tau_typedesc_is_integer((tau_typedesc_t*)src_desc))
    {
      if (tau_typedesc_is_integer(dst_desc))
      {
        if (tau_typedesc_is_signed((tau_typedesc_t*)src_desc) == tau_typedesc_is_signed(dst_desc))
          return tau_typedesc_integer_bits((tau_typedesc_t*)src_desc) <= tau_typedesc_integer_bits(dst_desc);

        if (tau_typedesc_is_signed((tau_typedesc_t*)src_desc) && tau_typedesc_is_unsigned(dst_desc))
          return false;

        return tau_typedesc_integer_bits((tau_typedesc_t*)src_desc) < tau_typedesc_integer_bits(dst_desc);
      }

      if (dst_desc->kind == TAU_TYPEDESC_F32)
        return tau_typedesc_integer_bits((tau_typedesc_t*)src_desc) <= 16;

      if (dst_desc->kind == TAU_TYPEDESC_F64)
        return tau_typedesc_integer_bits((tau_typedesc_t*)src_desc) <= 32;

      if (dst_desc->kind == TAU_TYPEDESC_C64)
        return tau_typedesc_integer_bits((tau_typedesc_t*)src_desc) <= 16;

      if (dst_desc->kind == TAU_TYPEDESC_C128)
        return tau_typedesc_integer_bits((tau_typedesc_t*)src_desc) <= 32;
    }
  }

  return false;
}

bool tau_typedesc_prim_is_implicitly_indirect_convertible(tau_typedesc_prim_t* src_desc, tau_typedesc_t* dst_desc)
{
  return (tau_typedesc_t*)src_desc == dst_desc;
}

bool tau_typedesc_prim_is_explicitly_convertible(tau_typedesc_prim_t* TAU_UNUSED(src_desc), tau_typedesc_t* dst_desc)
{
  return tau_typedesc_is_arithmetic(tau_typedesc_remove_mut(dst_desc));
}
