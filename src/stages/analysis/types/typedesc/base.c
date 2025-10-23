/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typedesc/base.h"

#include "ast/ast.h"

void tau_typedesc_free(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_MUT:    tau_typedesc_mut_free   ((tau_typedesc_mut_t*   )desc); break;
  case TAU_TYPEDESC_PTR:    tau_typedesc_ptr_free   ((tau_typedesc_ptr_t*   )desc); break;
  case TAU_TYPEDESC_ARRAY:  tau_typedesc_array_free ((tau_typedesc_array_t* )desc); break;
  case TAU_TYPEDESC_REF:    tau_typedesc_ref_free   ((tau_typedesc_ref_t*   )desc); break;
  case TAU_TYPEDESC_OPT:    tau_typedesc_opt_free   ((tau_typedesc_opt_t*   )desc); break;
  case TAU_TYPEDESC_VEC:    tau_typedesc_vec_free   ((tau_typedesc_vec_t*   )desc); break;
  case TAU_TYPEDESC_MAT:    tau_typedesc_mat_free   ((tau_typedesc_mat_t*   )desc); break;
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_U8:
  case TAU_TYPEDESC_U16:
  case TAU_TYPEDESC_U32:
  case TAU_TYPEDESC_U64:
  case TAU_TYPEDESC_USIZE:
  case TAU_TYPEDESC_F32:
  case TAU_TYPEDESC_F64:
  case TAU_TYPEDESC_C64:
  case TAU_TYPEDESC_C128:
  case TAU_TYPEDESC_CHAR:
  case TAU_TYPEDESC_BOOL:
  case TAU_TYPEDESC_UNIT:   tau_typedesc_prim_free  ((tau_typedesc_prim_t*  )desc); break;
  case TAU_TYPEDESC_FUN:    tau_typedesc_fun_free   ((tau_typedesc_fun_t*   )desc); break;
  case TAU_TYPEDESC_STRUCT: tau_typedesc_struct_free((tau_typedesc_struct_t*)desc); break;
  case TAU_TYPEDESC_UNION:  tau_typedesc_union_free ((tau_typedesc_union_t* )desc); break;
  case TAU_TYPEDESC_ENUM:   tau_typedesc_enum_free  ((tau_typedesc_enum_t*  )desc); break;
  case TAU_TYPEDESC_VAR:    tau_typedesc_var_free   ((tau_typedesc_var_t*   )desc); break;
  case TAU_TYPEDESC_POISON: tau_typedesc_poison_free((tau_typedesc_poison_t*)desc); break;
  default: TAU_UNREACHABLE();
  }
}

bool tau_typedesc_is_modifier(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_MUT:
  case TAU_TYPEDESC_PTR:
  case TAU_TYPEDESC_ARRAY:
  case TAU_TYPEDESC_REF:
  case TAU_TYPEDESC_OPT:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_builtin(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_VEC:
  case TAU_TYPEDESC_MAT:
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_U8:
  case TAU_TYPEDESC_U16:
  case TAU_TYPEDESC_U32:
  case TAU_TYPEDESC_U64:
  case TAU_TYPEDESC_USIZE:
  case TAU_TYPEDESC_F32:
  case TAU_TYPEDESC_F64:
  case TAU_TYPEDESC_C64:
  case TAU_TYPEDESC_C128:
  case TAU_TYPEDESC_CHAR:
  case TAU_TYPEDESC_BOOL:
  case TAU_TYPEDESC_UNIT:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_integer(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_U8:
  case TAU_TYPEDESC_U16:
  case TAU_TYPEDESC_U32:
  case TAU_TYPEDESC_U64:
  case TAU_TYPEDESC_USIZE:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_float(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_F32:
  case TAU_TYPEDESC_F64:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_complex(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_C64:
  case TAU_TYPEDESC_C128:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_vector(tau_typedesc_t* desc)
{
  return desc->kind == TAU_TYPEDESC_VEC;
}

bool tau_typedesc_is_matrix(tau_typedesc_t* desc)
{
  return desc->kind == TAU_TYPEDESC_MAT;
}

bool tau_typedesc_is_arithmetic(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_U8:
  case TAU_TYPEDESC_U16:
  case TAU_TYPEDESC_U32:
  case TAU_TYPEDESC_U64:
  case TAU_TYPEDESC_USIZE:
  case TAU_TYPEDESC_F32:
  case TAU_TYPEDESC_F64:
  case TAU_TYPEDESC_C64:
  case TAU_TYPEDESC_C128:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_signed(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_F32:
  case TAU_TYPEDESC_F64:
  case TAU_TYPEDESC_C64:
  case TAU_TYPEDESC_C128:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_unsigned(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_U8:
  case TAU_TYPEDESC_U16:
  case TAU_TYPEDESC_U32:
  case TAU_TYPEDESC_U64:
  case TAU_TYPEDESC_USIZE:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_invokable(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_FUN:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_composite(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_STRUCT:
  case TAU_TYPEDESC_UNION:
  case TAU_TYPEDESC_ENUM:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_decl(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_FUN:
  case TAU_TYPEDESC_STRUCT:
  case TAU_TYPEDESC_UNION:
  case TAU_TYPEDESC_ENUM:
    return true;
  default:
    return false;
  }
}

bool tau_typedesc_is_mut(tau_typedesc_t* desc)
{
  return desc->kind == TAU_TYPEDESC_MUT;
}

bool tau_typedesc_is_ptr(tau_typedesc_t* desc)
{
  return desc->kind == TAU_TYPEDESC_PTR;
}

bool tau_typedesc_is_array(tau_typedesc_t* desc)
{
  return desc->kind == TAU_TYPEDESC_ARRAY;
}

bool tau_typedesc_is_ref(tau_typedesc_t* desc)
{
  return desc->kind == TAU_TYPEDESC_REF;
}

bool tau_typedesc_is_opt(tau_typedesc_t* desc)
{
  return desc->kind == TAU_TYPEDESC_OPT;
}

bool tau_typedesc_is_poison(tau_typedesc_t* desc)
{
  return desc->kind == TAU_TYPEDESC_POISON;
}

tau_typedesc_t* tau_typedesc_remove_mut(tau_typedesc_t* desc)
{
  return tau_typedesc_is_mut(desc) ? ((tau_typedesc_mut_t*)desc)->base_type : desc;
}

tau_typedesc_t* tau_typedesc_remove_ptr(tau_typedesc_t* desc)
{
  return tau_typedesc_is_ptr(desc) ? ((tau_typedesc_ptr_t*)desc)->base_type : desc;
}

tau_typedesc_t* tau_typedesc_remove_array(tau_typedesc_t* desc)
{
  return tau_typedesc_is_array(desc) ? ((tau_typedesc_array_t*)desc)->base_type : desc;
}

tau_typedesc_t* tau_typedesc_remove_ref(tau_typedesc_t* desc)
{
  return tau_typedesc_is_ref(desc) ? ((tau_typedesc_ref_t*)desc)->base_type : desc;
}

tau_typedesc_t* tau_typedesc_remove_ref_mut(tau_typedesc_t* desc)
{
  return tau_typedesc_remove_mut(tau_typedesc_remove_ref(desc));
}

tau_typedesc_t* tau_typedesc_remove_opt(tau_typedesc_t* desc)
{
  return tau_typedesc_is_opt(desc) ? ((tau_typedesc_opt_t*)desc)->base_type : desc;
}

bool tau_typedesc_can_add_modifier(tau_typedesc_kind_t kind, tau_typedesc_t* desc)
{
  switch (kind)
  {
  case TAU_TYPEDESC_MUT:   return tau_typedesc_can_add_mut  (desc);
  case TAU_TYPEDESC_PTR:   return tau_typedesc_can_add_ptr  (desc);
  case TAU_TYPEDESC_ARRAY: return tau_typedesc_can_add_array(desc);
  case TAU_TYPEDESC_REF:   return tau_typedesc_can_add_ref  (desc);
  case TAU_TYPEDESC_OPT:   return tau_typedesc_can_add_opt  (desc);
  default: TAU_UNREACHABLE();
  }

  return false;
}

bool tau_typedesc_can_add_mut(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_MUT:
  case TAU_TYPEDESC_REF:
  case TAU_TYPEDESC_FUN:
    return false;
  default:
    return true;
  }
}

bool tau_typedesc_can_add_ptr(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_REF:
    return false;
  default:
    return true;
  }
}

bool tau_typedesc_can_add_array(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_REF:
  case TAU_TYPEDESC_FUN:
    return false;
  default:
    return true;
  }
}

bool tau_typedesc_can_add_ref(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_REF:
    return false;
  default:
    return true;
  }
}

bool tau_typedesc_can_add_opt(tau_typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TAU_TYPEDESC_MUT:
  case TAU_TYPEDESC_REF:
  case TAU_TYPEDESC_OPT:
  case TAU_TYPEDESC_FUN:
    return false;
  default:
    return true;
  }
}

bool tau_typedesc_is_implicitly_direct_convertible(tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  switch (src_desc->kind)
  {
  case TAU_TYPEDESC_MUT:    return tau_typedesc_mut_is_implicitly_direct_convertible   ((tau_typedesc_mut_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_PTR:    return tau_typedesc_ptr_is_implicitly_direct_convertible   ((tau_typedesc_ptr_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_ARRAY:  return tau_typedesc_array_is_implicitly_direct_convertible ((tau_typedesc_array_t* )src_desc, dst_desc);
  case TAU_TYPEDESC_REF:    return tau_typedesc_ref_is_implicitly_direct_convertible   ((tau_typedesc_ref_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_OPT:    return tau_typedesc_opt_is_implicitly_direct_convertible   ((tau_typedesc_opt_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_VEC:    return tau_typedesc_vec_is_implicitly_direct_convertible   ((tau_typedesc_vec_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_MAT:    return tau_typedesc_mat_is_implicitly_direct_convertible   ((tau_typedesc_mat_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_U8:
  case TAU_TYPEDESC_U16:
  case TAU_TYPEDESC_U32:
  case TAU_TYPEDESC_U64:
  case TAU_TYPEDESC_USIZE:
  case TAU_TYPEDESC_F32:
  case TAU_TYPEDESC_F64:
  case TAU_TYPEDESC_C64:
  case TAU_TYPEDESC_C128:
  case TAU_TYPEDESC_CHAR:
  case TAU_TYPEDESC_BOOL:
  case TAU_TYPEDESC_UNIT:   return tau_typedesc_prim_is_implicitly_direct_convertible  ((tau_typedesc_prim_t*  )src_desc, dst_desc);
  case TAU_TYPEDESC_FUN:    return tau_typedesc_fun_is_implicitly_direct_convertible   ((tau_typedesc_fun_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_STRUCT: return tau_typedesc_struct_is_implicitly_direct_convertible((tau_typedesc_struct_t*)src_desc, dst_desc);
  case TAU_TYPEDESC_UNION:  return tau_typedesc_union_is_implicitly_direct_convertible ((tau_typedesc_union_t* )src_desc, dst_desc);
  case TAU_TYPEDESC_ENUM:   return tau_typedesc_enum_is_implicitly_direct_convertible  ((tau_typedesc_enum_t*  )src_desc, dst_desc);
  case TAU_TYPEDESC_POISON: return tau_typedesc_poison_is_implicitly_direct_convertible((tau_typedesc_poison_t*)src_desc, dst_desc);
  default: TAU_UNREACHABLE();
  }

  return false;
}

bool tau_typedesc_is_implicitly_indirect_convertible(tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  switch (src_desc->kind)
  {
  case TAU_TYPEDESC_MUT:    return tau_typedesc_mut_is_implicitly_indirect_convertible   ((tau_typedesc_mut_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_PTR:    return tau_typedesc_ptr_is_implicitly_indirect_convertible   ((tau_typedesc_ptr_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_ARRAY:  return tau_typedesc_array_is_implicitly_indirect_convertible ((tau_typedesc_array_t* )src_desc, dst_desc);
  case TAU_TYPEDESC_REF:    return tau_typedesc_ref_is_implicitly_indirect_convertible   ((tau_typedesc_ref_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_OPT:    return tau_typedesc_opt_is_implicitly_indirect_convertible   ((tau_typedesc_opt_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_VEC:    return tau_typedesc_vec_is_implicitly_indirect_convertible   ((tau_typedesc_vec_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_MAT:    return tau_typedesc_mat_is_implicitly_indirect_convertible   ((tau_typedesc_mat_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_U8:
  case TAU_TYPEDESC_U16:
  case TAU_TYPEDESC_U32:
  case TAU_TYPEDESC_U64:
  case TAU_TYPEDESC_USIZE:
  case TAU_TYPEDESC_F32:
  case TAU_TYPEDESC_F64:
  case TAU_TYPEDESC_C64:
  case TAU_TYPEDESC_C128:
  case TAU_TYPEDESC_CHAR:
  case TAU_TYPEDESC_BOOL:
  case TAU_TYPEDESC_UNIT:   return tau_typedesc_prim_is_implicitly_indirect_convertible  ((tau_typedesc_prim_t*  )src_desc, dst_desc);
  case TAU_TYPEDESC_FUN:    return tau_typedesc_fun_is_implicitly_indirect_convertible   ((tau_typedesc_fun_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_STRUCT: return tau_typedesc_struct_is_implicitly_indirect_convertible((tau_typedesc_struct_t*)src_desc, dst_desc);
  case TAU_TYPEDESC_UNION:  return tau_typedesc_union_is_implicitly_indirect_convertible ((tau_typedesc_union_t* )src_desc, dst_desc);
  case TAU_TYPEDESC_ENUM:   return tau_typedesc_enum_is_implicitly_indirect_convertible  ((tau_typedesc_enum_t*  )src_desc, dst_desc);
  case TAU_TYPEDESC_POISON: return tau_typedesc_poison_is_implicitly_indirect_convertible((tau_typedesc_poison_t*)src_desc, dst_desc);
  default: TAU_UNREACHABLE();
  }

  return false;
}

bool tau_typedesc_is_explicitly_convertible(tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  switch (src_desc->kind)
  {
  case TAU_TYPEDESC_MUT:    return tau_typedesc_mut_is_explicitly_convertible   ((tau_typedesc_mut_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_PTR:    return tau_typedesc_ptr_is_explicitly_convertible   ((tau_typedesc_ptr_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_ARRAY:  return tau_typedesc_array_is_explicitly_convertible ((tau_typedesc_array_t* )src_desc, dst_desc);
  case TAU_TYPEDESC_REF:    return tau_typedesc_ref_is_explicitly_convertible   ((tau_typedesc_ref_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_OPT:    return tau_typedesc_opt_is_explicitly_convertible   ((tau_typedesc_opt_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_VEC:    return tau_typedesc_vec_is_explicitly_convertible   ((tau_typedesc_vec_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_MAT:    return tau_typedesc_mat_is_explicitly_convertible   ((tau_typedesc_mat_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_U8:
  case TAU_TYPEDESC_U16:
  case TAU_TYPEDESC_U32:
  case TAU_TYPEDESC_U64:
  case TAU_TYPEDESC_USIZE:
  case TAU_TYPEDESC_F32:
  case TAU_TYPEDESC_F64:
  case TAU_TYPEDESC_C64:
  case TAU_TYPEDESC_C128:
  case TAU_TYPEDESC_CHAR:
  case TAU_TYPEDESC_BOOL:
  case TAU_TYPEDESC_UNIT:   return tau_typedesc_prim_is_explicitly_convertible  ((tau_typedesc_prim_t*  )src_desc, dst_desc);
  case TAU_TYPEDESC_FUN:    return tau_typedesc_fun_is_explicitly_convertible   ((tau_typedesc_fun_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_STRUCT: return tau_typedesc_struct_is_explicitly_convertible((tau_typedesc_struct_t*)src_desc, dst_desc);
  case TAU_TYPEDESC_UNION:  return tau_typedesc_union_is_explicitly_convertible ((tau_typedesc_union_t* )src_desc, dst_desc);
  case TAU_TYPEDESC_ENUM:   return tau_typedesc_enum_is_explicitly_convertible  ((tau_typedesc_enum_t*  )src_desc, dst_desc);
  case TAU_TYPEDESC_POISON: return tau_typedesc_poison_is_explicitly_convertible((tau_typedesc_poison_t*)src_desc, dst_desc);
  default: TAU_UNREACHABLE();
  }

  return false;
}

size_t tau_typedesc_integer_bits(tau_typedesc_t* desc)
{
  TAU_ASSERT(tau_typedesc_is_integer(desc));

  switch (desc->kind)
  {
  case TAU_TYPEDESC_I8:
  case TAU_TYPEDESC_U8:
    return 8;
  case TAU_TYPEDESC_I16:
  case TAU_TYPEDESC_U16:
    return 16;
  case TAU_TYPEDESC_I32:
  case TAU_TYPEDESC_U32:
    return 32;
  case TAU_TYPEDESC_I64:
  case TAU_TYPEDESC_U64:
    return 64;
  case TAU_TYPEDESC_ISIZE:
  case TAU_TYPEDESC_USIZE:
    return sizeof(void*) * 8;
  default:
    TAU_UNREACHABLE();
  }

  return 0;
}

bool tau_typedesc_is_callable(tau_typedesc_t* desc)
{
  desc = tau_typedesc_remove_mut(desc);

  if (tau_typedesc_is_ref(desc))
  {
    desc = tau_typedesc_remove_ptr(tau_typedesc_remove_mut(tau_typedesc_remove_ref(desc)));

    if (desc->kind == TAU_TYPEDESC_FUN)
      return true;
  }
  else if (tau_typedesc_is_ptr(desc))
  {
    desc = tau_typedesc_remove_ptr(desc);

    if (desc->kind == TAU_TYPEDESC_FUN)
      return true;
  }

  return false;
}

tau_typedesc_t* tau_typedesc_underlying_callable(tau_typedesc_t* desc)
{
  desc = tau_typedesc_remove_mut(desc);

  if (tau_typedesc_is_ref(desc))
  {
    desc = tau_typedesc_remove_ptr(tau_typedesc_remove_mut(tau_typedesc_remove_ref(desc)));

    if (desc->kind == TAU_TYPEDESC_FUN)
      return desc;
  }
  else if (tau_typedesc_is_ptr(desc))
  {
    desc = tau_typedesc_remove_ptr(desc);

    if (desc->kind == TAU_TYPEDESC_FUN)
      return desc;
  }

  TAU_UNREACHABLE();

  return NULL;
}
