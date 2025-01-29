/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/base.h"

#include "ast/ast.h"

void typedesc_free(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_MUT:    typedesc_mut_free   ((typedesc_mut_t*   )desc); break;
  case TYPEDESC_PTR:    typedesc_ptr_free   ((typedesc_ptr_t*   )desc); break;
  case TYPEDESC_ARRAY:  typedesc_array_free ((typedesc_array_t* )desc); break;
  case TYPEDESC_REF:    typedesc_ref_free   ((typedesc_ref_t*   )desc); break;
  case TYPEDESC_OPT:    typedesc_opt_free   ((typedesc_opt_t*   )desc); break;
  case TYPEDESC_VEC:    typedesc_vec_free   ((typedesc_vec_t*   )desc); break;
  case TYPEDESC_MAT:    typedesc_mat_free   ((typedesc_mat_t*   )desc); break;
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
  case TYPEDESC_C64:
  case TYPEDESC_C128:
  case TYPEDESC_CHAR:
  case TYPEDESC_BOOL:
  case TYPEDESC_UNIT:   typedesc_prim_free  ((typedesc_prim_t*  )desc); break;
  case TYPEDESC_FUN:    typedesc_fun_free   ((typedesc_fun_t*   )desc); break;
  case TYPEDESC_STRUCT: typedesc_struct_free((typedesc_struct_t*)desc); break;
  case TYPEDESC_UNION:  typedesc_union_free ((typedesc_union_t* )desc); break;
  case TYPEDESC_ENUM:   typedesc_enum_free  ((typedesc_enum_t*  )desc); break;
  case TYPEDESC_VAR:    typedesc_var_free   ((typedesc_var_t*   )desc); break;
  case TYPEDESC_POISON: typedesc_poison_free((typedesc_poison_t*)desc); break;
  default: UNREACHABLE();
  }
}

bool typedesc_is_modifier(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_MUT:
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
  case TYPEDESC_VEC:
  case TYPEDESC_MAT:
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
  case TYPEDESC_C64:
  case TYPEDESC_C128:
  case TYPEDESC_CHAR:
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

bool typedesc_is_complex(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_C64:
  case TYPEDESC_C128:
    return true;
  default:
    return false;
  }
}

bool typedesc_is_vector(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_VEC;
}

bool typedesc_is_matrix(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_MAT;
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
  case TYPEDESC_C64:
  case TYPEDESC_C128:
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
  case TYPEDESC_C64:
  case TYPEDESC_C128:
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

bool typedesc_is_mut(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_MUT;
}

bool typedesc_is_ptr(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_PTR;
}

bool typedesc_is_array(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_ARRAY;
}

bool typedesc_is_ref(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_REF;
}

bool typedesc_is_opt(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_OPT;
}

bool typedesc_is_poison(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_POISON;
}

typedesc_t* typedesc_remove_mut(typedesc_t* desc)
{
  return typedesc_is_mut(desc) ? ((typedesc_mut_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_ptr(typedesc_t* desc)
{
  return typedesc_is_ptr(desc) ? ((typedesc_ptr_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_array(typedesc_t* desc)
{
  return typedesc_is_array(desc) ? ((typedesc_array_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_ref(typedesc_t* desc)
{
  return typedesc_is_ref(desc) ? ((typedesc_ref_t*)desc)->base_type : desc;
}

typedesc_t* typedesc_remove_ref_mut(typedesc_t* desc)
{
  return typedesc_remove_mut(typedesc_remove_ref(desc));
}

typedesc_t* typedesc_remove_opt(typedesc_t* desc)
{
  return typedesc_is_opt(desc) ? ((typedesc_opt_t*)desc)->base_type : desc;
}

bool typedesc_can_add_modifier(typedesc_kind_t kind, typedesc_t* desc)
{
  switch (kind)
  {
  case TYPEDESC_MUT:   return typedesc_can_add_mut  (desc);
  case TYPEDESC_PTR:   return typedesc_can_add_ptr  (desc);
  case TYPEDESC_ARRAY: return typedesc_can_add_array(desc);
  case TYPEDESC_REF:   return typedesc_can_add_ref  (desc);
  case TYPEDESC_OPT:   return typedesc_can_add_opt  (desc);
  default: UNREACHABLE();
  }

  return false;
}

bool typedesc_can_add_mut(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_MUT:
  case TYPEDESC_REF:
  case TYPEDESC_FUN:
    return false;
  default:
    return true;
  }
}

bool typedesc_can_add_ptr(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_REF:
    return false;
  default:
    return true;
  }
}

bool typedesc_can_add_array(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_REF:
  case TYPEDESC_FUN:
    return false;
  default:
    return true;
  }
}

bool typedesc_can_add_ref(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_REF:
    return false;
  default:
    return true;
  }
}

bool typedesc_can_add_opt(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_MUT:
  case TYPEDESC_REF:
  case TYPEDESC_OPT:
  case TYPEDESC_FUN:
    return false;
  default:
    return true;
  }
}

bool typedesc_is_implicitly_direct_convertible(typedesc_t* src_desc, typedesc_t* dst_desc)
{
  switch (src_desc->kind)
  {
  case TYPEDESC_MUT:    return typedesc_mut_is_implicitly_direct_convertible   ((typedesc_mut_t*   )src_desc, dst_desc);
  case TYPEDESC_PTR:    return typedesc_ptr_is_implicitly_direct_convertible   ((typedesc_ptr_t*   )src_desc, dst_desc);
  case TYPEDESC_ARRAY:  return typedesc_array_is_implicitly_direct_convertible ((typedesc_array_t* )src_desc, dst_desc);
  case TYPEDESC_REF:    return typedesc_ref_is_implicitly_direct_convertible   ((typedesc_ref_t*   )src_desc, dst_desc);
  case TYPEDESC_OPT:    return typedesc_opt_is_implicitly_direct_convertible   ((typedesc_opt_t*   )src_desc, dst_desc);
  case TYPEDESC_VEC:    return typedesc_vec_is_implicitly_direct_convertible   ((typedesc_vec_t*   )src_desc, dst_desc);
  case TYPEDESC_MAT:    return typedesc_mat_is_implicitly_direct_convertible   ((typedesc_mat_t*   )src_desc, dst_desc);
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
  case TYPEDESC_C64:
  case TYPEDESC_C128:
  case TYPEDESC_CHAR:
  case TYPEDESC_BOOL:
  case TYPEDESC_UNIT:   return typedesc_prim_is_implicitly_direct_convertible  ((typedesc_prim_t*  )src_desc, dst_desc);
  case TYPEDESC_FUN:    return typedesc_fun_is_implicitly_direct_convertible   ((typedesc_fun_t*   )src_desc, dst_desc);
  case TYPEDESC_STRUCT: return typedesc_struct_is_implicitly_direct_convertible((typedesc_struct_t*)src_desc, dst_desc);
  case TYPEDESC_UNION:  return typedesc_union_is_implicitly_direct_convertible ((typedesc_union_t* )src_desc, dst_desc);
  case TYPEDESC_ENUM:   return typedesc_enum_is_implicitly_direct_convertible  ((typedesc_enum_t*  )src_desc, dst_desc);
  case TYPEDESC_POISON: return typedesc_poison_is_implicitly_direct_convertible((typedesc_poison_t*)src_desc, dst_desc);
  default: UNREACHABLE();
  }

  return false;
}

bool typedesc_is_implicitly_indirect_convertible(typedesc_t* src_desc, typedesc_t* dst_desc)
{
  switch (src_desc->kind)
  {
  case TYPEDESC_MUT:    return typedesc_mut_is_implicitly_indirect_convertible   ((typedesc_mut_t*   )src_desc, dst_desc);
  case TYPEDESC_PTR:    return typedesc_ptr_is_implicitly_indirect_convertible   ((typedesc_ptr_t*   )src_desc, dst_desc);
  case TYPEDESC_ARRAY:  return typedesc_array_is_implicitly_indirect_convertible ((typedesc_array_t* )src_desc, dst_desc);
  case TYPEDESC_REF:    return typedesc_ref_is_implicitly_indirect_convertible   ((typedesc_ref_t*   )src_desc, dst_desc);
  case TYPEDESC_OPT:    return typedesc_opt_is_implicitly_indirect_convertible   ((typedesc_opt_t*   )src_desc, dst_desc);
  case TYPEDESC_VEC:    return typedesc_vec_is_implicitly_indirect_convertible   ((typedesc_vec_t*   )src_desc, dst_desc);
  case TYPEDESC_MAT:    return typedesc_mat_is_implicitly_indirect_convertible   ((typedesc_mat_t*   )src_desc, dst_desc);
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
  case TYPEDESC_C64:
  case TYPEDESC_C128:
  case TYPEDESC_CHAR:
  case TYPEDESC_BOOL:
  case TYPEDESC_UNIT:   return typedesc_prim_is_implicitly_indirect_convertible  ((typedesc_prim_t*  )src_desc, dst_desc);
  case TYPEDESC_FUN:    return typedesc_fun_is_implicitly_indirect_convertible   ((typedesc_fun_t*   )src_desc, dst_desc);
  case TYPEDESC_STRUCT: return typedesc_struct_is_implicitly_indirect_convertible((typedesc_struct_t*)src_desc, dst_desc);
  case TYPEDESC_UNION:  return typedesc_union_is_implicitly_indirect_convertible ((typedesc_union_t* )src_desc, dst_desc);
  case TYPEDESC_ENUM:   return typedesc_enum_is_implicitly_indirect_convertible  ((typedesc_enum_t*  )src_desc, dst_desc);
  case TYPEDESC_POISON: return typedesc_poison_is_implicitly_indirect_convertible((typedesc_poison_t*)src_desc, dst_desc);
  default: UNREACHABLE();
  }

  return false;
}

bool typedesc_is_explicitly_convertible(typedesc_t* src_desc, typedesc_t* dst_desc)
{
  switch (src_desc->kind)
  {
  case TYPEDESC_MUT:    return typedesc_mut_is_explicitly_convertible   ((typedesc_mut_t*   )src_desc, dst_desc);
  case TYPEDESC_PTR:    return typedesc_ptr_is_explicitly_convertible   ((typedesc_ptr_t*   )src_desc, dst_desc);
  case TYPEDESC_ARRAY:  return typedesc_array_is_explicitly_convertible ((typedesc_array_t* )src_desc, dst_desc);
  case TYPEDESC_REF:    return typedesc_ref_is_explicitly_convertible   ((typedesc_ref_t*   )src_desc, dst_desc);
  case TYPEDESC_OPT:    return typedesc_opt_is_explicitly_convertible   ((typedesc_opt_t*   )src_desc, dst_desc);
  case TYPEDESC_VEC:    return typedesc_vec_is_explicitly_convertible   ((typedesc_vec_t*   )src_desc, dst_desc);
  case TYPEDESC_MAT:    return typedesc_mat_is_explicitly_convertible   ((typedesc_mat_t*   )src_desc, dst_desc);
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
  case TYPEDESC_C64:
  case TYPEDESC_C128:
  case TYPEDESC_CHAR:
  case TYPEDESC_BOOL:
  case TYPEDESC_UNIT:   return typedesc_prim_is_explicitly_convertible  ((typedesc_prim_t*  )src_desc, dst_desc);
  case TYPEDESC_FUN:    return typedesc_fun_is_explicitly_convertible   ((typedesc_fun_t*   )src_desc, dst_desc);
  case TYPEDESC_STRUCT: return typedesc_struct_is_explicitly_convertible((typedesc_struct_t*)src_desc, dst_desc);
  case TYPEDESC_UNION:  return typedesc_union_is_explicitly_convertible ((typedesc_union_t* )src_desc, dst_desc);
  case TYPEDESC_ENUM:   return typedesc_enum_is_explicitly_convertible  ((typedesc_enum_t*  )src_desc, dst_desc);
  case TYPEDESC_POISON: return typedesc_poison_is_explicitly_convertible((typedesc_poison_t*)src_desc, dst_desc);
  default: UNREACHABLE();
  }

  return false;
}

size_t typedesc_integer_bits(typedesc_t* desc)
{
  ASSERT(typedesc_is_integer(desc));

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
    UNREACHABLE();
  }

  return 0;
}

bool typedesc_is_callable(typedesc_t* desc)
{
  desc = typedesc_remove_mut(desc);

  if (typedesc_is_ref(desc))
  {
    desc = typedesc_remove_ptr(typedesc_remove_mut(typedesc_remove_ref(desc)));

    if (desc->kind == TYPEDESC_FUN)
      return true;
  }
  else if (typedesc_is_ptr(desc))
  {
    desc = typedesc_remove_ptr(desc);

    if (desc->kind == TYPEDESC_FUN)
      return true;
  }

  return false;
}

typedesc_t* typedesc_underlying_callable(typedesc_t* desc)
{
  desc = typedesc_remove_mut(desc);

  if (typedesc_is_ref(desc))
  {
    desc = typedesc_remove_ptr(typedesc_remove_mut(typedesc_remove_ref(desc)));

    if (desc->kind == TYPEDESC_FUN)
      return desc;
  }
  else if (typedesc_is_ptr(desc))
  {
    desc = typedesc_remove_ptr(desc);

    if (desc->kind == TYPEDESC_FUN)
      return desc;
  }

  UNREACHABLE();

  return NULL;
}
