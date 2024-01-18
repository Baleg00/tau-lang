/**
 * \file typedesc.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analyzer/typedesc.h"

#include <string.h>

#include "utils/common.h"
#include "utils/memory/memtrace.h"

typedesc_t* typedesc_init(typedesc_kind_t kind)
{
  size_t desc_size = sizeof(typedesc_t);

  switch (kind)
  {
  case TYPEDESC_TYPE:   desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_MUT:    desc_size = sizeof(typedesc_mut_t   ); break;
  case TYPEDESC_CONST:  desc_size = sizeof(typedesc_const_t ); break;
  case TYPEDESC_PTR:    desc_size = sizeof(typedesc_ptr_t   ); break;
  case TYPEDESC_ARRAY:  desc_size = sizeof(typedesc_array_t ); break;
  case TYPEDESC_REF:    desc_size = sizeof(typedesc_ref_t   ); break;
  case TYPEDESC_OPT:    desc_size = sizeof(typedesc_opt_t   ); break;
  case TYPEDESC_I8:     desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_I16:    desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_I32:    desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_I64:    desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_ISIZE:  desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_U8:     desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_U16:    desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_U32:    desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_U64:    desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_USIZE:  desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_F32:    desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_F64:    desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_BOOL:   desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_UNIT:   desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_NULL:   desc_size = sizeof(typedesc_t       ); break;
  case TYPEDESC_FUN:    desc_size = sizeof(typedesc_fun_t   ); break;
  case TYPEDESC_STRUCT: desc_size = sizeof(typedesc_struct_t); break;
  case TYPEDESC_UNION:  desc_size = sizeof(typedesc_union_t ); break;
  case TYPEDESC_ENUM:   desc_size = sizeof(typedesc_enum_t  ); break;
  default: unreachable();
  }

  typedesc_t* desc = (typedesc_t*)malloc(desc_size);
  assert(desc != NULL);

  memset(desc, 0, desc_size);

  desc->kind = kind;

  return desc;
}

void typedesc_free(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_MUT:
  case TYPEDESC_CONST:
  case TYPEDESC_PTR:
  case TYPEDESC_ARRAY:
  case TYPEDESC_REF:
  case TYPEDESC_OPT:
  case TYPEDESC_I8:
  case TYPEDESC_I16:
  case TYPEDESC_I32:
  case TYPEDESC_I64:
  case TYPEDESC_ISIZE:
  case TYPEDESC_U8:
  case TYPEDESC_U16:
  case TYPEDESC_U32:
  case TYPEDESC_U64:
  case TYPEDESC_USIZE:
  case TYPEDESC_F32:
  case TYPEDESC_F64:
  case TYPEDESC_BOOL:
  case TYPEDESC_UNIT:
  case TYPEDESC_NULL:
  case TYPEDESC_TYPE:
    break;
  case TYPEDESC_FUN:
    vector_free(((typedesc_fun_t*)desc)->param_types);
    break;
  case TYPEDESC_STRUCT:
    vector_free(((typedesc_struct_t*)desc)->field_types);
    break;
  case TYPEDESC_UNION:
    vector_free(((typedesc_union_t*)desc)->field_types);
    break;
  case TYPEDESC_ENUM:
    break;
  default:
    unreachable();
  }

  free(desc);
}

bool typedesc_is_modifier(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_MUT:
  case TYPEDESC_CONST:
  case TYPEDESC_PTR:
  case TYPEDESC_ARRAY:
  case TYPEDESC_REF:
  case TYPEDESC_OPT:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_builtin(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_I8:
  case TYPEDESC_I16:
  case TYPEDESC_I32:
  case TYPEDESC_I64:
  case TYPEDESC_ISIZE:
  case TYPEDESC_U8:
  case TYPEDESC_U16:
  case TYPEDESC_U32:
  case TYPEDESC_U64:
  case TYPEDESC_USIZE:
  case TYPEDESC_F32:
  case TYPEDESC_F64:
  case TYPEDESC_BOOL:
  case TYPEDESC_UNIT:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_integer(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_I8:
  case TYPEDESC_I16:
  case TYPEDESC_I32:
  case TYPEDESC_I64:
  case TYPEDESC_ISIZE:
  case TYPEDESC_U8:
  case TYPEDESC_U16:
  case TYPEDESC_U32:
  case TYPEDESC_U64:
  case TYPEDESC_USIZE:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_float(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_F32:
  case TYPEDESC_F64:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_arithmetic(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_I8:
  case TYPEDESC_I16:
  case TYPEDESC_I32:
  case TYPEDESC_I64:
  case TYPEDESC_ISIZE:
  case TYPEDESC_U8:
  case TYPEDESC_U16:
  case TYPEDESC_U32:
  case TYPEDESC_U64:
  case TYPEDESC_USIZE:
  case TYPEDESC_F32:
  case TYPEDESC_F64:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_signed(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_I8:
  case TYPEDESC_I16:
  case TYPEDESC_I32:
  case TYPEDESC_I64:
  case TYPEDESC_ISIZE:
  case TYPEDESC_F32:
  case TYPEDESC_F64:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_unsigned(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_U8:
  case TYPEDESC_U16:
  case TYPEDESC_U32:
  case TYPEDESC_U64:
  case TYPEDESC_USIZE:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_invokable(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_FUN:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_composite(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_STRUCT:
  case TYPEDESC_UNION:
  case TYPEDESC_ENUM:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_decl(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_FUN:
  case TYPEDESC_STRUCT:
  case TYPEDESC_UNION:
  case TYPEDESC_ENUM:
    return true;
  default:
    return false;
  }
}

typedesc_t* typedesc_remove_mut(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_MUT ? ((typedesc_mut_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_const(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_CONST ? ((typedesc_const_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_ptr(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_PTR ? ((typedesc_ptr_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_array(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_ARRAY ? ((typedesc_array_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_ref(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_REF ? ((typedesc_ref_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_opt(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_OPT ? ((typedesc_opt_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_const_mut(typedesc_t* desc)
{
  return typedesc_remove_mut(typedesc_remove_const(desc));
}

typedesc_t* typedesc_remove_const_ref(typedesc_t* desc)
{
  return typedesc_remove_ref(typedesc_remove_const(desc));
}

typedesc_t* typedesc_remove_const_ref_mut(typedesc_t* desc)
{
  return typedesc_remove_mut(typedesc_remove_const_ref(desc));
}

bool typedesc_can_add_modifier(typedesc_kind_t kind, typedesc_t* desc)
{
  switch (kind)
  {
  case TYPEDESC_MUT:   return typedesc_can_add_mut  (desc);
  case TYPEDESC_CONST: return typedesc_can_add_const(desc);
  case TYPEDESC_PTR:   return typedesc_can_add_ptr  (desc);
  case TYPEDESC_ARRAY: return typedesc_can_add_array(desc);
  case TYPEDESC_REF:   return typedesc_can_add_ref  (desc);
  case TYPEDESC_OPT:   return typedesc_can_add_opt  (desc);
  default: unreachable();
  }

  return false;
}

bool typedesc_can_add_mut(typedesc_t* desc)
{
  return desc->kind != TYPEDESC_MUT &&
         desc->kind != TYPEDESC_REF &&
         desc->kind != TYPEDESC_FUN &&
         desc->kind != TYPEDESC_CONST;
}

bool typedesc_can_add_const(typedesc_t* desc)
{
  return desc->kind != TYPEDESC_CONST;
}

bool typedesc_can_add_ptr(typedesc_t* desc)
{
  desc = typedesc_remove_mut(desc);

  return desc->kind != TYPEDESC_CONST &&
         desc->kind != TYPEDESC_REF;
}

bool typedesc_can_add_array(typedesc_t* desc)
{
  return typedesc_can_add_ptr(desc);
}

bool typedesc_can_add_ref(typedesc_t* desc)
{
  desc = typedesc_remove_mut(desc);
  
  return desc->kind != TYPEDESC_CONST &&
         desc->kind != TYPEDESC_REF;
}

bool typedesc_can_add_opt(typedesc_t* desc)
{
  desc = typedesc_remove_mut(desc);

  return desc->kind != TYPEDESC_OPT &&
         desc->kind != TYPEDESC_CONST &&
         desc->kind != TYPEDESC_REF;
}

bool typedesc_is_implicitly_convertible(typedesc_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_CONST &&
      from_desc->kind != TYPEDESC_CONST)
    return false;

  to_desc = typedesc_remove_const_mut(to_desc);
  from_desc = typedesc_remove_const_mut(from_desc);

  if (to_desc->kind == TYPEDESC_REF)
  {
    if (from_desc->kind != TYPEDESC_REF)
      return false;

    to_desc = typedesc_remove_ref(to_desc);
    from_desc = typedesc_remove_ref(from_desc);

    if (to_desc->kind == TYPEDESC_MUT &&
        from_desc->kind != TYPEDESC_MUT)
      return false;
  }
  else
    from_desc = typedesc_remove_ref(from_desc);

  return to_desc == from_desc;
}

size_t typedesc_integer_bits(typedesc_t* desc)
{
  assert(typedesc_is_integer(desc));

  switch (desc->kind)
  {
  case TYPEDESC_I8:
  case TYPEDESC_U8:
    return 8;
  case TYPEDESC_I16:
  case TYPEDESC_U16:
    return 16;
  case TYPEDESC_I32:
  case TYPEDESC_U32:
    return 32;
  case TYPEDESC_I64:
  case TYPEDESC_U64:
    return 64;
  case TYPEDESC_ISIZE:
  case TYPEDESC_USIZE:
    return sizeof(void*) * 8;
  default:
    unreachable();
  }

  return 0;
}

typedesc_t* typedesc_arithmetic_promote(typedesc_t* lhs_desc, typedesc_t* rhs_desc)
{
  assert(typedesc_is_arithmetic(lhs_desc) && typedesc_is_arithmetic(rhs_desc));

  if (lhs_desc->kind == rhs_desc->kind)
    return lhs_desc;

  if (typedesc_is_float(lhs_desc) && !typedesc_is_float(rhs_desc))
    return lhs_desc;

  if (!typedesc_is_float(lhs_desc) && typedesc_is_float(rhs_desc))
    return rhs_desc;

  if (typedesc_is_float(lhs_desc) && typedesc_is_float(rhs_desc))
    if (lhs_desc->kind == TYPEDESC_F64)
      return lhs_desc;
    else
      return rhs_desc;

  if (typedesc_integer_bits(lhs_desc) < typedesc_integer_bits(rhs_desc))
    return rhs_desc;
  
  return lhs_desc;
}

bool typedesc_is_callable(typedesc_t* desc)
{
  desc = typedesc_remove_const_mut(desc);

  if (desc->kind == TYPEDESC_REF)
  {
    desc = typedesc_remove_ptr(typedesc_remove_mut(typedesc_remove_ref(desc)));

    if (desc->kind == TYPEDESC_FUN)
      return true;
  }
  else if (desc->kind == TYPEDESC_PTR)
  {
    desc = typedesc_remove_ptr(desc);

    if (desc->kind == TYPEDESC_FUN)
      return true;
  }

  return false;
}

typedesc_t* typedesc_underlying_callable(typedesc_t* desc)
{
  desc = typedesc_remove_const_mut(desc);

  if (desc->kind == TYPEDESC_REF)
  {
    desc = typedesc_remove_ptr(typedesc_remove_mut(typedesc_remove_ref(desc)));

    if (desc->kind == TYPEDESC_FUN)
      return desc;
  }
  else if (desc->kind == TYPEDESC_PTR)
  {
    desc = typedesc_remove_ptr(desc);

    if (desc->kind == TYPEDESC_FUN)
      return desc;
  }

  unreachable();

  return NULL;
}
