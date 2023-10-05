/**
 * \file typedesc.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "typedesc.h"

#include <string.h>

#include "memtrace.h"
#include "vector.h"
#include "util.h"

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
  case TYPEDESC_GEN:    desc_size = sizeof(typedesc_gen_t   ); break;
  case TYPEDESC_STRUCT: desc_size = sizeof(typedesc_struct_t); break;
  case TYPEDESC_UNION:  desc_size = sizeof(typedesc_union_t ); break;
  case TYPEDESC_ENUM:   desc_size = sizeof(typedesc_enum_t  ); break;
  case TYPEDESC_MOD:    desc_size = sizeof(typedesc_mod_t   ); break;
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
    list_free(((typedesc_fun_t*)desc)->param_types);
    break;
  case TYPEDESC_GEN:
    list_free(((typedesc_gen_t*)desc)->param_types);
    break;
  case TYPEDESC_STRUCT:
    list_free(((typedesc_struct_t*)desc)->field_types);
    break;
  case TYPEDESC_UNION:
    list_free(((typedesc_union_t*)desc)->field_types);
    break;
  case TYPEDESC_ENUM:
    break;
  case TYPEDESC_MOD:
    list_free(((typedesc_mod_t*)desc)->member_types);
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
  case TYPEDESC_GEN:
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
  case TYPEDESC_MOD:
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
  case TYPEDESC_GEN:
  case TYPEDESC_STRUCT:
  case TYPEDESC_UNION:
  case TYPEDESC_ENUM:
  case TYPEDESC_MOD:
    return true;
  default:
    return false;
  }
}

typedesc_t* typedesc_remove_modifier(typedesc_t* desc)
{
  return typedesc_is_modifier(desc) ? ((typedesc_modifier_t*)desc)->base_type : desc;
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

typedesc_t* typedesc_remove_const_mut_ref(typedesc_t* desc)
{
  return typedesc_remove_ref(typedesc_remove_const_mut(desc));
}

typedesc_t* typedesc_underlying_type(typedesc_t* desc)
{
  while (desc->kind == TYPEDESC_MUT ||
         desc->kind == TYPEDESC_CONST ||
         desc->kind == TYPEDESC_REF ||
         desc->kind == TYPEDESC_OPT)
    desc = ((typedesc_modifier_t*)desc)->base_type;

  return desc;
}

bool typedesc_check_can_add_modifier(typedesc_kind_t kind, typedesc_t* desc)
{
  switch (kind)
  {
  case TYPEDESC_MUT:   return typedesc_check_can_add_mut  (desc);
  case TYPEDESC_CONST: return typedesc_check_can_add_const(desc);
  case TYPEDESC_PTR:   return typedesc_check_can_add_ptr  (desc);
  case TYPEDESC_ARRAY: return typedesc_check_can_add_array(desc);
  case TYPEDESC_REF:   return typedesc_check_can_add_ref  (desc);
  case TYPEDESC_OPT:   return typedesc_check_can_add_opt  (desc);
  default: unreachable();
  }

  return false;
}

bool typedesc_check_can_add_mut(typedesc_t* desc)
{
  return desc->kind != TYPEDESC_MUT &&
         desc->kind != TYPEDESC_CONST;
}

bool typedesc_check_can_add_const(typedesc_t* desc)
{
  return desc->kind != TYPEDESC_CONST;
}

bool typedesc_check_can_add_ptr(typedesc_t* desc)
{
  if (desc->kind == TYPEDESC_MUT)
    return ((typedesc_mut_t*)desc)->base_type->kind != TYPEDESC_REF;

  return desc->kind != TYPEDESC_CONST &&
         desc->kind != TYPEDESC_REF;
}

bool typedesc_check_can_add_array(typedesc_t* desc)
{
  return typedesc_check_can_add_ptr(desc);
}

bool typedesc_check_can_add_ref(typedesc_t* desc)
{
  if (desc->kind == TYPEDESC_MUT)
    return ((typedesc_mut_t*)desc)->base_type->kind != TYPEDESC_REF;
  
  return desc->kind != TYPEDESC_CONST &&
         desc->kind != TYPEDESC_REF;
}

bool typedesc_check_can_add_opt(typedesc_t* desc)
{
  if (desc->kind == TYPEDESC_MUT)
    return ((typedesc_mut_t*)desc)->base_type->kind != TYPEDESC_REF &&
           ((typedesc_mut_t*)desc)->base_type->kind != TYPEDESC_OPT;

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
