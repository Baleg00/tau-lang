/**
 * \file typedesc.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "typedesc.h"

#include <string.h>

#include "allocator.h"
#include "memtrace.h"
#include "vector.h"
#include "util.h"

/**
 * \brief Vector of all allocated type descriptors.
 */
static vector_t* g_descriptors = NULL;

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

  typedesc_t* desc = (typedesc_t*)allocator_allocate(allocator_global(), desc_size);
  assert(desc != NULL);

  memset(desc, 0, desc_size);

  desc->kind = kind;

  if (g_descriptors == NULL)
    g_descriptors = vector_init();

  vector_push(g_descriptors, desc);

  return desc;
}

void typedesc_cleanup(void)
{
  if (g_descriptors == NULL)
    return;

  for (size_t i = 0; i < vector_size(g_descriptors); i++)
  {
    typedesc_t* desc = (typedesc_t*)vector_get(g_descriptors, i);

    switch (desc->kind)
    {
    case TYPEDESC_FUN:
      list_free(((typedesc_fun_t*)desc)->param_types);
      break;
    case TYPEDESC_GEN:
      list_free(((typedesc_gen_t*)desc)->param_types);
      break;
    case TYPEDESC_STRUCT:
      list_free(((typedesc_struct_t*)desc)->member_types);
      break;
    case TYPEDESC_UNION:
      list_free(((typedesc_union_t*)desc)->member_types);
      break;
    case TYPEDESC_MOD:
      list_free(((typedesc_mod_t*)desc)->member_types);
      break;
    default: fallthrough();
    }

    allocator_deallocate(allocator_global(), desc);
  }

  vector_free(g_descriptors);
  g_descriptors = NULL;
}

typedesc_t* typedesc_add_mut(typedesc_t* base_type)
{
  typedesc_mut_t* desc = (typedesc_mut_t*)typedesc_init(TYPEDESC_MUT);
  desc->base_type = base_type;
  return (typedesc_t*)desc;
}

typedesc_t* typedesc_add_const(typedesc_t* base_type)
{
  typedesc_const_t* desc = (typedesc_const_t*)typedesc_init(TYPEDESC_CONST);
  desc->base_type = base_type;
  return (typedesc_t*)desc;
}

typedesc_t* typedesc_add_ptr(typedesc_t* base_type)
{
  typedesc_ptr_t* desc = (typedesc_ptr_t*)typedesc_init(TYPEDESC_PTR);
  desc->base_type = base_type;
  return (typedesc_t*)desc;
}

typedesc_t* typedesc_add_array(typedesc_t* base_type)
{
  typedesc_array_t* desc = (typedesc_array_t*)typedesc_init(TYPEDESC_ARRAY);
  desc->base_type = base_type;
  return (typedesc_t*)desc;
}

typedesc_t* typedesc_add_ref(typedesc_t* base_type)
{
  typedesc_ref_t* desc = (typedesc_ref_t*)typedesc_init(TYPEDESC_REF);
  desc->base_type = base_type;
  return (typedesc_t*)desc;
}

typedesc_t* typedesc_add_opt(typedesc_t* base_type)
{
  typedesc_opt_t* desc = (typedesc_opt_t*)typedesc_init(TYPEDESC_OPT);
  desc->base_type = base_type;
  return (typedesc_t*)desc;
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

bool typedesc_is_same(typedesc_t* first, typedesc_t* second)
{
  if (first->kind != second->kind)
    return false;

  if (typedesc_is_modifier(first))
    return typedesc_is_same(((typedesc_modifier_t*)first)->base_type, ((typedesc_modifier_t*)second)->base_type);

  if (typedesc_is_invokable(first))
  {
    if (!typedesc_is_invokable(second))
      return false;
    
    list_t *first_params, *second_params;

    switch (first->kind)
    {
    case TYPEDESC_FUN: first_params = ((typedesc_fun_t*)first)->param_types; break;
    case TYPEDESC_GEN: first_params = ((typedesc_gen_t*)first)->param_types; break;
    default: unreachable();
    }

    switch (second->kind)
    {
    case TYPEDESC_FUN: second_params = ((typedesc_fun_t*)second)->param_types; break;
    case TYPEDESC_GEN: second_params = ((typedesc_gen_t*)second)->param_types; break;
    default: unreachable();
    }

    if (list_size(first_params) != list_size(second_params))
      return false;

    list_node_t* lhs_it = list_front_node(first_params);
    list_node_t* rhs_it = list_front_node(second_params);
    
    for (; lhs_it != NULL && rhs_it != NULL; lhs_it = list_node_next(lhs_it), rhs_it = list_node_next(rhs_it))
      if (!typedesc_is_same((typedesc_t*)list_node_get(lhs_it), (typedesc_t*)list_node_get(rhs_it)))
        return false;

    switch (first->kind)
    {
    case TYPEDESC_FUN: return typedesc_is_same(((typedesc_fun_t*)first)->return_type, ((typedesc_fun_t*)second)->return_type);
    case TYPEDESC_GEN: return typedesc_is_same(((typedesc_gen_t*)first)->yield_type, ((typedesc_gen_t*)second)->yield_type);
    default: unreachable();
    }

    return false;
  }

  if (typedesc_is_decl(first))
    if (!typedesc_is_decl(second))
      return false;
    else
      return ((typedesc_decl_t*)first)->node == ((typedesc_decl_t*)second)->node;

  return true;
}
