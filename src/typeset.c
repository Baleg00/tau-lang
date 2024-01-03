/**
 * \file typeset.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "typeset.h"

#include <stdint.h>
#include <string.h>

#include "hash.h"
#include "memtrace.h"
#include "set.h"
#include "util.h"

struct typeset_t
{
  typedesc_t* desc_i8;
  typedesc_t* desc_i16;
  typedesc_t* desc_i32;
  typedesc_t* desc_i64;
  typedesc_t* desc_isize;
  typedesc_t* desc_u8;
  typedesc_t* desc_u16;
  typedesc_t* desc_u32;
  typedesc_t* desc_u64;
  typedesc_t* desc_usize;
  typedesc_t* desc_f32;
  typedesc_t* desc_f64;
  typedesc_t* desc_bool;
  typedesc_t* desc_unit;

  set_t* set_mut;
  set_t* set_const;
  set_t* set_ptr;
  set_t* set_array;
  set_t* set_ref;
  set_t* set_opt;
  set_t* set_fun;
  set_t* set_gen;
  set_t* set_struct;
  set_t* set_union;
  set_t* set_enum;
};

static int typeset_cmp_modifier(void* lhs, void* rhs)
{
  return (int)(((typedesc_modifier_t*)lhs)->base_type - ((typedesc_modifier_t*)rhs)->base_type);
}

static int typeset_cmp_fun(void* lhs, void* rhs)
{
  typedesc_fun_t* lhs_desc = (typedesc_fun_t*)lhs;
  typedesc_fun_t* rhs_desc = (typedesc_fun_t*)rhs;

  size_t lhs_hash = hash_digest(&lhs_desc->return_type, sizeof(typedesc_t*));

  LIST_FOR_LOOP(it, lhs_desc->param_types)
  {
    typedesc_t* param_desc = (typedesc_t*)list_node_get(it);
    lhs_hash = hash_combine_with_data(lhs_hash, &param_desc, sizeof(typedesc_t*));
  }

  lhs_hash = hash_combine_with_data(lhs_hash, &lhs_desc->is_vararg, sizeof(bool));
  lhs_hash = hash_combine_with_data(lhs_hash, &lhs_desc->abi, sizeof(abi_kind_t));

  size_t rhs_hash = hash_digest(&rhs_desc->return_type, sizeof(typedesc_t*));

  LIST_FOR_LOOP(it, rhs_desc->param_types)
  {
    typedesc_t* param_desc = (typedesc_t*)list_node_get(it);
    rhs_hash = hash_combine_with_data(rhs_hash, &param_desc, sizeof(typedesc_t*));
  }

  rhs_hash = hash_combine_with_data(rhs_hash, &rhs_desc->is_vararg, sizeof(bool));
  rhs_hash = hash_combine_with_data(rhs_hash, &rhs_desc->abi, sizeof(abi_kind_t));

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int typeset_cmp_gen(void* lhs, void* rhs)
{
  typedesc_gen_t* lhs_desc = (typedesc_gen_t*)lhs;
  typedesc_gen_t* rhs_desc = (typedesc_gen_t*)rhs;

  size_t lhs_hash = hash_digest(&lhs_desc->yield_type, sizeof(typedesc_t*));

  LIST_FOR_LOOP(it, lhs_desc->param_types)
  {
    typedesc_t* param_desc = (typedesc_t*)list_node_get(it);
    lhs_hash = hash_combine_with_data(lhs_hash, &param_desc, sizeof(typedesc_t*));
  }

  size_t rhs_hash = hash_digest(&rhs_desc->yield_type, sizeof(typedesc_t*));

  LIST_FOR_LOOP(it, rhs_desc->param_types)
  {
    typedesc_t* param_desc = (typedesc_t*)list_node_get(it);
    rhs_hash = hash_combine_with_data(rhs_hash, &param_desc, sizeof(typedesc_t*));
  }

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int typeset_cmp_struct(void* lhs, void* rhs)
{
  typedesc_struct_t* lhs_desc = (typedesc_struct_t*)lhs;
  typedesc_struct_t* rhs_desc = (typedesc_struct_t*)rhs;

  size_t lhs_hash = hash_digest(&lhs_desc->node, sizeof(ast_node_t*));

  LIST_FOR_LOOP(it, lhs_desc->field_types)
  {
    typedesc_t* field_desc = (typedesc_t*)list_node_get(it);
    lhs_hash = hash_combine_with_data(lhs_hash, &field_desc, sizeof(typedesc_t*));
  }

  size_t rhs_hash = hash_digest(&rhs_desc->node, sizeof(ast_node_t*));

  LIST_FOR_LOOP(it, rhs_desc->field_types)
  {
    typedesc_t* field_desc = (typedesc_t*)list_node_get(it);
    rhs_hash = hash_combine_with_data(rhs_hash, &field_desc, sizeof(typedesc_t*));
  }

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int typeset_cmp_union(void* lhs, void* rhs)
{
  typedesc_union_t* lhs_desc = (typedesc_union_t*)lhs;
  typedesc_union_t* rhs_desc = (typedesc_union_t*)rhs;

  size_t lhs_hash = hash_digest(&lhs_desc->node, sizeof(ast_node_t*));

  LIST_FOR_LOOP(it, lhs_desc->field_types)
  {
    typedesc_t* field_desc = (typedesc_t*)list_node_get(it);
    lhs_hash = hash_combine_with_data(lhs_hash, &field_desc, sizeof(typedesc_t*));
  }

  size_t rhs_hash = hash_digest(&rhs_desc->node, sizeof(ast_node_t*));

  LIST_FOR_LOOP(it, rhs_desc->field_types)
  {
    typedesc_t* field_desc = (typedesc_t*)list_node_get(it);
    rhs_hash = hash_combine_with_data(rhs_hash, &field_desc, sizeof(typedesc_t*));
  }

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int typeset_cmp_enum(void* lhs, void* rhs)
{
  typedesc_enum_t* lhs_desc = (typedesc_enum_t*)lhs;
  typedesc_enum_t* rhs_desc = (typedesc_enum_t*)rhs;

  size_t lhs_hash = hash_digest(&lhs_desc->node, sizeof(ast_node_t*));
  size_t rhs_hash = hash_digest(&rhs_desc->node, sizeof(ast_node_t*));

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

typeset_t* typeset_init(void)
{
  typeset_t* typeset = (typeset_t*)malloc(sizeof(typeset_t));

  typeset->desc_i8    = typedesc_init(TYPEDESC_I8);
  typeset->desc_i16   = typedesc_init(TYPEDESC_I16);
  typeset->desc_i32   = typedesc_init(TYPEDESC_I32);
  typeset->desc_i64   = typedesc_init(TYPEDESC_I64);
  typeset->desc_isize = typedesc_init(TYPEDESC_ISIZE);
  typeset->desc_u8    = typedesc_init(TYPEDESC_U8);
  typeset->desc_u16   = typedesc_init(TYPEDESC_U16);
  typeset->desc_u32   = typedesc_init(TYPEDESC_U32);
  typeset->desc_u64   = typedesc_init(TYPEDESC_U64);
  typeset->desc_usize = typedesc_init(TYPEDESC_USIZE);
  typeset->desc_f32   = typedesc_init(TYPEDESC_F32);
  typeset->desc_f64   = typedesc_init(TYPEDESC_F64);
  typeset->desc_bool  = typedesc_init(TYPEDESC_BOOL);
  typeset->desc_unit  = typedesc_init(TYPEDESC_UNIT);

  typeset->set_mut    = set_init(typeset_cmp_modifier);
  typeset->set_const  = set_init(typeset_cmp_modifier);
  typeset->set_ptr    = set_init(typeset_cmp_modifier);
  typeset->set_array  = set_init(typeset_cmp_modifier);
  typeset->set_ref    = set_init(typeset_cmp_modifier);
  typeset->set_opt    = set_init(typeset_cmp_modifier);
  typeset->set_fun    = set_init(typeset_cmp_fun);
  typeset->set_gen    = set_init(typeset_cmp_gen);
  typeset->set_struct = set_init(typeset_cmp_struct);
  typeset->set_union  = set_init(typeset_cmp_union);
  typeset->set_enum   = set_init(typeset_cmp_enum);

  return typeset;
}

void typeset_free(typeset_t* typeset)
{
  typedesc_free(typeset->desc_i8);
  typedesc_free(typeset->desc_i16);
  typedesc_free(typeset->desc_i32);
  typedesc_free(typeset->desc_i64);
  typedesc_free(typeset->desc_isize);
  typedesc_free(typeset->desc_u8);
  typedesc_free(typeset->desc_u16);
  typedesc_free(typeset->desc_u32);
  typedesc_free(typeset->desc_u64);
  typedesc_free(typeset->desc_usize);
  typedesc_free(typeset->desc_f32);
  typedesc_free(typeset->desc_f64);
  typedesc_free(typeset->desc_bool);
  typedesc_free(typeset->desc_unit);

  set_for_each(typeset->set_mut,    (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_const,  (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_ptr,    (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_array,  (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_ref,    (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_opt,    (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_fun,    (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_gen,    (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_struct, (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_union,  (set_for_each_func_t)typedesc_free);
  set_for_each(typeset->set_enum,   (set_for_each_func_t)typedesc_free);

  set_free(typeset->set_mut);
  set_free(typeset->set_const);
  set_free(typeset->set_ptr);
  set_free(typeset->set_array);
  set_free(typeset->set_ref);
  set_free(typeset->set_opt);
  set_free(typeset->set_fun);
  set_free(typeset->set_gen);
  set_free(typeset->set_struct);
  set_free(typeset->set_union);
  set_free(typeset->set_enum);

  free(typeset);
}

static typedesc_t* typeset_lookup_modifier(typeset_t* typeset, typedesc_kind_t kind, typedesc_t* base_type)
{
  assert(typedesc_can_add_modifier(kind, base_type));

  typedesc_modifier_t* desc = (typedesc_modifier_t*)typedesc_init(kind);
  desc->base_type = base_type;

  set_t* set_modifier = NULL;

  switch (kind)
  {
  case TYPEDESC_MUT:   set_modifier = typeset->set_mut;   break;
  case TYPEDESC_CONST: set_modifier = typeset->set_const; break;
  case TYPEDESC_PTR:   set_modifier = typeset->set_ptr;   break;
  case TYPEDESC_ARRAY: set_modifier = typeset->set_array; break;
  case TYPEDESC_REF:   set_modifier = typeset->set_ref;   break;
  case TYPEDESC_OPT:   set_modifier = typeset->set_opt;   break;
  default: unreachable();
  }

  typedesc_t* result = (typedesc_t*)set_get(set_modifier, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  set_add(set_modifier, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typeset_lookup_mut(typeset_t* typeset, typedesc_t* base_type)
{
  return typeset_lookup_modifier(typeset, TYPEDESC_MUT, base_type);
}

typedesc_t* typeset_lookup_const(typeset_t* typeset, typedesc_t* base_type)
{
  return typeset_lookup_modifier(typeset, TYPEDESC_CONST, base_type);
}

typedesc_t* typeset_lookup_ptr(typeset_t* typeset, typedesc_t* base_type)
{
  return typeset_lookup_modifier(typeset, TYPEDESC_PTR, base_type);
}

typedesc_t* typeset_lookup_array(typeset_t* typeset, size_t length, typedesc_t* base_type)
{
  return typeset_lookup_modifier(typeset, TYPEDESC_ARRAY, base_type);
}

typedesc_t* typeset_lookup_ref(typeset_t* typeset, typedesc_t* base_type)
{
  return typeset_lookup_modifier(typeset, TYPEDESC_REF, base_type);
}

typedesc_t* typeset_lookup_opt(typeset_t* typeset, typedesc_t* base_type)
{
  return typeset_lookup_modifier(typeset, TYPEDESC_OPT, base_type);
}

typedesc_t* typeset_lookup_i8(typeset_t* typeset)
{
  return typeset->desc_i8;
}

typedesc_t* typeset_lookup_i16(typeset_t* typeset)
{
  return typeset->desc_i16;
}

typedesc_t* typeset_lookup_i32(typeset_t* typeset)
{
  return typeset->desc_i32;
}

typedesc_t* typeset_lookup_i64(typeset_t* typeset)
{
  return typeset->desc_i64;
}

typedesc_t* typeset_lookup_isize(typeset_t* typeset)
{
  return typeset->desc_isize;
}

typedesc_t* typeset_lookup_u8(typeset_t* typeset)
{
  return typeset->desc_u8;
}

typedesc_t* typeset_lookup_u16(typeset_t* typeset)
{
  return typeset->desc_u16;
}

typedesc_t* typeset_lookup_u32(typeset_t* typeset)
{
  return typeset->desc_u32;
}

typedesc_t* typeset_lookup_u64(typeset_t* typeset)
{
  return typeset->desc_u64;
}

typedesc_t* typeset_lookup_usize(typeset_t* typeset)
{
  return typeset->desc_usize;
}

typedesc_t* typeset_lookup_f32(typeset_t* typeset)
{
  return typeset->desc_f32;
}

typedesc_t* typeset_lookup_f64(typeset_t* typeset)
{
  return typeset->desc_f64;
}

typedesc_t* typeset_lookup_bool(typeset_t* typeset)
{
  return typeset->desc_bool;
}

typedesc_t* typeset_lookup_unit(typeset_t* typeset)
{
  return typeset->desc_unit;
}

typedesc_t* typeset_lookup_fun(typeset_t* typeset, typedesc_t* return_type, typedesc_t* param_types[], size_t param_count, bool is_vararg, abi_kind_t abi)
{
  typedesc_fun_t* desc = (typedesc_fun_t*)typedesc_init(TYPEDESC_FUN);
  desc->return_type = return_type;
  desc->param_types = param_count == 0 ? NULL : list_init_from_buffer(param_types, param_count);
  desc->is_vararg = is_vararg;
  desc->abi = abi;

  typedesc_t* result = (typedesc_t*)set_get(typeset->set_fun, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  set_add(typeset->set_fun, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typeset_lookup_gen(typeset_t* typeset, typedesc_t* yield_type, typedesc_t* param_types[], size_t param_count)
{
  typedesc_gen_t* desc = (typedesc_gen_t*)typedesc_init(TYPEDESC_GEN);
  desc->yield_type = yield_type;
  desc->param_types = param_count == 0 ? NULL : list_init_from_buffer(param_types, param_count);

  typedesc_t* result = (typedesc_t*)set_get(typeset->set_gen, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  set_add(typeset->set_gen, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typeset_lookup_struct(typeset_t* typeset, ast_node_t* node, typedesc_t* field_types[], size_t field_count)
{
  typedesc_struct_t* desc = (typedesc_struct_t*)typedesc_init(TYPEDESC_STRUCT);
  desc->node = node;
  desc->field_types = field_count == 0 ? NULL : list_init_from_buffer(field_types, field_count);

  typedesc_t* result = (typedesc_t*)set_get(typeset->set_struct, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  set_add(typeset->set_struct, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typeset_lookup_union(typeset_t* typeset, ast_node_t* node, typedesc_t* field_types[], size_t field_count)
{
  typedesc_union_t* desc = (typedesc_union_t*)typedesc_init(TYPEDESC_UNION);
  desc->node = node;
  desc->field_types = field_count == 0 ? NULL : list_init_from_buffer(field_types, field_count);

  typedesc_t* result = (typedesc_t*)set_get(typeset->set_union, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  set_add(typeset->set_union, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typeset_lookup_enum(typeset_t* typeset, ast_node_t* node)
{
  typedesc_enum_t* desc = (typedesc_enum_t*)typedesc_init(TYPEDESC_ENUM);
  desc->node = node;

  typedesc_t* result = (typedesc_t*)set_get(typeset->set_enum, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  set_add(typeset->set_enum, desc);

  return (typedesc_t*)desc;
}
