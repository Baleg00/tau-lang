/**
 * \file typedesc.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "typedesc.h"

#include "memtrace.h"
#include "util.h"

LIST_FOR_EACH_FUNC_DECL(typedesc_free, typedesc_t)

/** Utility macro which expands to fields that all types must have. */
#define TYPEDESC_HEADER\
  struct\
  {\
    typedesc_kind_t kind; /** Type kind. */\
    size_t size; /** Byte size of type. */\
    size_t align; /** Memory alignment of type. */\
  }\

/** Utility macro which expands to fields that all declared types must have. */
#define TYPEDESC_DECL_HEADER\
  struct\
  {\
    ast_node_t* node; /** Declaration node. */\
  }\

/** Base type for all types. */
struct typedesc_s
{
  TYPEDESC_HEADER;
};

/** Type for mutable types. */
struct typedesc_mut_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for compile-time types. */
struct typedesc_const_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for pointer types. */
struct typedesc_ptr_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for array types. */
struct typedesc_array_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for reference types. */
struct typedesc_ref_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for optional types. */
struct typedesc_opt_s
{
  TYPEDESC_HEADER;
  typedesc_t* base_type; // Underlying type.
};

/** Type for function types. */
struct typedesc_fun_s
{
  TYPEDESC_HEADER;
  list_t* param_types; // Parameter types.
  typedesc_t* return_type; // Return type.
};

/** Type for generator types. */
struct typedesc_gen_s
{
  TYPEDESC_HEADER;
  list_t* param_types; // List of parameter types.
  typedesc_t* yield_type; // Yield type.
};

/** Base type for all declared types. */
struct typedesc_decl_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
};

/** Type for struct types. */
struct typedesc_struct_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
};

/** Type for union types. */
struct typedesc_union_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
};

/** Type for enum types. */
struct typedesc_enum_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
};

/** Type for module types. */
struct typedesc_mod_s
{
  TYPEDESC_HEADER;
  TYPEDESC_DECL_HEADER;
  list_t* member_types; // List of member types.
};

#undef TYPEDESC_DECL_HEADER
#undef TYPEDESC_HEADER

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

  typedesc_t* desc = (typedesc_t*)calloc(1, desc_size);
  assert(desc != NULL);

  desc->kind = kind;

  return desc;
}

static void typedesc_free_type(typedesc_t* desc)
{
  free(desc);
}

static void typedesc_free_mut(typedesc_mut_t* desc)
{
  typedesc_free(desc->base_type);
  free(desc);
}

static void typedesc_free_const(typedesc_const_t* desc)
{
  typedesc_free(desc->base_type);
  free(desc);
}

static void typedesc_free_ptr(typedesc_ptr_t* desc)
{
  typedesc_free(desc->base_type);
  free(desc);
}

static void typedesc_free_array(typedesc_array_t* desc)
{
  typedesc_free(desc->base_type);
  free(desc);
}

static void typedesc_free_ref(typedesc_ref_t* desc)
{
  typedesc_free(desc->base_type);
  free(desc);
}

static void typedesc_free_opt(typedesc_opt_t* desc)
{
  typedesc_free(desc->base_type);
  free(desc);
}

static void typedesc_free_builtin(typedesc_t* desc)
{
  free(desc);
}

static void typedesc_free_fun(typedesc_fun_t* desc)
{
  typedesc_free(desc->return_type);
  list_for_each(desc->param_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
  list_free(desc->param_types);
  free(desc);
}

static void typedesc_free_gen(typedesc_gen_t* desc)
{
  typedesc_free(desc->yield_type);
  list_for_each(desc->param_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
  list_free(desc->param_types);
  free(desc);
}

static void typedesc_free_struct(typedesc_struct_t* desc)
{
  list_for_each(desc->member_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
  list_free(desc->member_types);
  free(desc);
}

static void typedesc_free_union(typedesc_union_t* desc)
{
  list_for_each(desc->member_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
  list_free(desc->member_types);
  free(desc);
}

static void typedesc_free_enum(typedesc_enum_t* desc)
{
  free(desc);
}

static void typedesc_free_mod(typedesc_mod_t* desc)
{
  list_for_each(desc->member_types, LIST_FOR_EACH_FUNC_NAME(typedesc_free));
  list_free(desc->member_types);
  free(desc);
}

void typedesc_free(typedesc_t* desc)
{
  if (desc == NULL)
    return;

  switch (desc->kind)
  {
  case TYPEDESC_TYPE:   typedesc_free_type   (                    desc); break;
  case TYPEDESC_MUT:    typedesc_free_mut    ((typedesc_mut_t*)   desc); break;
  case TYPEDESC_CONST:  typedesc_free_const  ((typedesc_const_t*) desc); break;
  case TYPEDESC_PTR:    typedesc_free_ptr    ((typedesc_ptr_t*)   desc); break;
  case TYPEDESC_ARRAY:  typedesc_free_array  ((typedesc_array_t*) desc); break;
  case TYPEDESC_REF:    typedesc_free_ref    ((typedesc_ref_t*)   desc); break;
  case TYPEDESC_OPT:    typedesc_free_opt    ((typedesc_opt_t*)   desc); break;
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
  case TYPEDESC_NULL:   typedesc_free_builtin(                    desc); break;
  case TYPEDESC_FUN:    typedesc_free_fun    ((typedesc_fun_t*)   desc); break;
  case TYPEDESC_GEN:    typedesc_free_gen    ((typedesc_gen_t*)   desc); break;
  case TYPEDESC_STRUCT: typedesc_free_struct ((typedesc_struct_t*)desc); break;
  case TYPEDESC_UNION:  typedesc_free_union  ((typedesc_union_t*) desc); break;
  case TYPEDESC_ENUM:   typedesc_free_enum   ((typedesc_enum_t*)  desc); break;
  case TYPEDESC_MOD:    typedesc_free_mod    ((typedesc_mod_t*)   desc); break;
  default: unreachable();
  }
}


typedesc_kind_t typedesc_get_kind(typedesc_t* desc)
{
  return desc->kind;
}

void typedesc_set_kind(typedesc_t* desc, typedesc_kind_t kind)
{
  desc->kind = kind;
}

size_t typedesc_get_size(typedesc_t* desc)
{
  return desc->size;
}

void typedesc_set_size(typedesc_t* desc, size_t size)
{
  desc->size = size;
}

size_t typedesc_get_align(typedesc_t* desc)
{
  return desc->align;
}

void typedesc_set_align(typedesc_t* desc, size_t align)
{
  desc->align = align;
}

ast_node_t* typedesc_get_node(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_STRUCT: return ((typedesc_struct_t*)desc)->node;
  case TYPEDESC_UNION:  return ((typedesc_union_t*) desc)->node;
  case TYPEDESC_ENUM:   return ((typedesc_enum_t*)  desc)->node;
  case TYPEDESC_MOD:    return ((typedesc_mod_t*)   desc)->node;
  default: unreachable();
  }

  return NULL;
}

void typedesc_set_node(typedesc_t* desc, ast_node_t* node)
{
  switch (desc->kind)
  {
  case TYPEDESC_STRUCT: ((typedesc_struct_t*)desc)->node = node; break;
  case TYPEDESC_UNION:  ((typedesc_union_t*) desc)->node = node; break;
  case TYPEDESC_ENUM:   ((typedesc_enum_t*)  desc)->node = node; break;
  case TYPEDESC_MOD:    ((typedesc_mod_t*)   desc)->node = node; break;
  default: unreachable();
  }
}

typedesc_t* typedesc_get_base(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_MUT:   return ((typedesc_mut_t*)  desc)->base_type;
  case TYPEDESC_CONST: return ((typedesc_const_t*)desc)->base_type;
  case TYPEDESC_PTR:   return ((typedesc_ptr_t*)  desc)->base_type;
  case TYPEDESC_ARRAY: return ((typedesc_array_t*)desc)->base_type;
  case TYPEDESC_REF:   return ((typedesc_ref_t*)  desc)->base_type;
  case TYPEDESC_OPT:   return ((typedesc_opt_t*)  desc)->base_type;
  default: unreachable();
  }

  return NULL;
}

void typedesc_set_base(typedesc_t* desc, typedesc_t* base_type)
{
  switch (desc->kind)
  {
  case TYPEDESC_MUT:   ((typedesc_mut_t*)  desc)->base_type = base_type; break;
  case TYPEDESC_CONST: ((typedesc_const_t*)desc)->base_type = base_type; break;
  case TYPEDESC_PTR:   ((typedesc_ptr_t*)  desc)->base_type = base_type; break;
  case TYPEDESC_ARRAY: ((typedesc_array_t*)desc)->base_type = base_type; break;
  case TYPEDESC_REF:   ((typedesc_ref_t*)  desc)->base_type = base_type; break;
  case TYPEDESC_OPT:   ((typedesc_opt_t*)  desc)->base_type = base_type; break;
  default: unreachable();
  }
}

typedesc_t* typedesc_get_return(typedesc_t* desc)
{
  assert(desc->kind == TYPEDESC_FUN);
  return ((typedesc_fun_t*)desc)->return_type;
}

void typedesc_set_return(typedesc_t* desc, typedesc_t* return_type)
{
  assert(desc->kind == TYPEDESC_FUN);
  ((typedesc_fun_t*)desc)->return_type = return_type;
}

typedesc_t* typedesc_get_yield(typedesc_t* desc)
{
  assert(desc->kind == TYPEDESC_GEN);
  return ((typedesc_gen_t*)desc)->yield_type;
}

void typedesc_set_yield(typedesc_t* desc, typedesc_t* yield_type)
{
  assert(desc->kind == TYPEDESC_GEN);
  ((typedesc_gen_t*)desc)->yield_type = yield_type;
}

list_t* typedesc_get_params(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_FUN: return ((typedesc_fun_t*)desc)->param_types;
  case TYPEDESC_GEN: return ((typedesc_gen_t*)desc)->param_types;
  default: unreachable();
  }

  return NULL;
}

void typedesc_set_params(typedesc_t* desc, list_t* param_types)
{
  switch (desc->kind)
  {
  case TYPEDESC_FUN: ((typedesc_fun_t*)desc)->param_types = param_types; break;
  case TYPEDESC_GEN: ((typedesc_gen_t*)desc)->param_types = param_types; break;
  default: unreachable();
  }
}

list_t* typedesc_get_members(typedesc_t* desc)
{
  switch (desc->kind)
  {
  case TYPEDESC_STRUCT: return ((typedesc_struct_t*)desc)->member_types;
  case TYPEDESC_UNION:  return ((typedesc_union_t*) desc)->member_types;
  case TYPEDESC_MOD:    return ((typedesc_mod_t*)   desc)->member_types;
  default: unreachable();
  }

  return NULL;
}

void typedesc_set_members(typedesc_t* desc, list_t* member_types)
{
  switch (desc->kind)
  {
  case TYPEDESC_STRUCT: ((typedesc_struct_t*)desc)->member_types = member_types; break;
  case TYPEDESC_UNION:  ((typedesc_union_t*) desc)->member_types = member_types; break;
  case TYPEDESC_MOD:    ((typedesc_mod_t*)   desc)->member_types = member_types; break;
  default: unreachable();
  }
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
  return desc->kind == TYPEDESC_MUT ? typedesc_get_base(desc) : desc;
}

typedesc_t* typedesc_remove_const(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_CONST ? typedesc_get_base(desc) : desc;
}

typedesc_t* typedesc_remove_ptr(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_PTR ? typedesc_get_base(desc) : desc;
}

typedesc_t* typedesc_remove_array(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_ARRAY ? typedesc_get_base(desc) : desc;
}

typedesc_t* typedesc_remove_ref(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_REF ? typedesc_get_base(desc) : desc;
}

typedesc_t* typedesc_remove_opt(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_OPT ? typedesc_get_base(desc) : desc;
}

bool typedesc_is_modifier(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_MODIFIER;
}

bool typedesc_is_builtin(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_BUILTIN;
}

bool typedesc_is_integer(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_INTEGER;
}

bool typedesc_is_float(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_FLOAT;
}

bool typedesc_is_arithmetic(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_ARITHMETIC;
}

bool typedesc_is_signed(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_SIGNED;
}

bool typedesc_is_unsigned(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_UNSIGNED;
}

bool typedesc_is_invokable(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_INVOKABLE;
}

bool typedesc_is_composite(typedesc_t* desc)
{
  return desc->kind & TYPEDESC_FLAG_COMPOSITE;
}

bool typedesc_is_type(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_TYPE;
}

bool typedesc_is_mut(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_MUT;
}

bool typedesc_is_const(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_CONST;
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

bool typedesc_is_i8(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_I8;
}

bool typedesc_is_i16(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_I16;
}

bool typedesc_is_i32(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_I32;
}

bool typedesc_is_i64(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_I64;
}

bool typedesc_is_isize(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_ISIZE;
}

bool typedesc_is_u8(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_U8;
}

bool typedesc_is_u16(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_U16;
}

bool typedesc_is_u32(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_U32;
}

bool typedesc_is_u64(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_U64;
}

bool typedesc_is_usize(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_USIZE;
}

bool typedesc_is_f32(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_F32;
}

bool typedesc_is_f64(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_F64;
}

bool typedesc_is_bool(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_BOOL;
}

bool typedesc_is_unit(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_UNIT;
}

bool typedesc_is_null(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_NULL;
}

bool typedesc_is_fun(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_FUN;
}

bool typedesc_is_gen(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_GEN;
}

bool typedesc_is_struct(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_STRUCT;
}

bool typedesc_is_union(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_UNION;
}

bool typedesc_is_enum(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_ENUM;
}

bool typedesc_is_mod(typedesc_t* desc)
{
  return desc->kind == TYPEDESC_MOD;
}

bool typedesc_is_same(typedesc_t* first, typedesc_t* second)
{
  if (first->kind != second->kind)
    return false;

  if (typedesc_is_modifier(first))
    return typedesc_is_same(typedesc_get_base(first), typedesc_get_base(second));

  if (typedesc_is_invokable(first))
  {
    if (list_size(typedesc_get_params(first)) != list_size(typedesc_get_params(second)))
      return false;

    list_node_t* lhs_it = list_front_node(typedesc_get_params(first));
    list_node_t* rhs_it = list_front_node(typedesc_get_params(second));
    
    for (; lhs_it != NULL && rhs_it != NULL; lhs_it = list_node_next(lhs_it), rhs_it = list_node_next(rhs_it))
      if (!typedesc_is_same((typedesc_t*)list_node_get(lhs_it), (typedesc_t*)list_node_get(rhs_it)))
        return false;

    switch (first->kind)
    {
    case TYPEDESC_FUN: return typedesc_is_same(typedesc_get_return(first), typedesc_get_return(second));
    case TYPEDESC_GEN: return typedesc_is_same(typedesc_get_yield(first), typedesc_get_yield(second));
    default: unreachable();
    }

    return false;
  }

  if (typedesc_is_composite(first))
    return typedesc_get_node(first) == typedesc_get_node(second);

  return true;
}
