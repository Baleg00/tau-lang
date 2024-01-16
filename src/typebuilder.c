/**
 * \file typebuilder.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "typebuilder.h"

#include <stdint.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "hash.h"
#include "memtrace.h"
#include "set.h"
#include "util.h"

struct typebuilder_t
{
  LLVMContextRef llvm_context;
  LLVMTargetDataRef llvm_layout;

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

static int typebuilder_cmp_modifier(void* lhs, void* rhs)
{
  return (int)(((typedesc_modifier_t*)lhs)->base_type - ((typedesc_modifier_t*)rhs)->base_type);
}

static int typebuilder_cmp_fun(void* lhs, void* rhs)
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

static int typebuilder_cmp_gen(void* lhs, void* rhs)
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

static int typebuilder_cmp_struct(void* lhs, void* rhs)
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

static int typebuilder_cmp_union(void* lhs, void* rhs)
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

static int typebuilder_cmp_enum(void* lhs, void* rhs)
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

static typedesc_t* typebuilder_build_modifier(typebuilder_t* builder, typedesc_kind_t kind, typedesc_t* base_type)
{
  assert(typedesc_can_add_modifier(kind, base_type));

  typedesc_modifier_t* desc = (typedesc_modifier_t*)typedesc_init(kind);
  desc->base_type = base_type;

  set_t* set_modifier = NULL;

  switch (kind)
  {
  case TYPEDESC_MUT:   set_modifier = builder->set_mut;   break;
  case TYPEDESC_CONST: set_modifier = builder->set_const; break;
  case TYPEDESC_PTR:   set_modifier = builder->set_ptr;   break;
  case TYPEDESC_ARRAY: set_modifier = builder->set_array; break;
  case TYPEDESC_REF:   set_modifier = builder->set_ref;   break;
  case TYPEDESC_OPT:   set_modifier = builder->set_opt;   break;
  default: unreachable();
  }

  typedesc_t* result = (typedesc_t*)set_get(set_modifier, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  switch (kind)
  {
  case TYPEDESC_MUT:   desc->llvm_type = base_type->llvm_type; break;
  case TYPEDESC_CONST: desc->llvm_type = base_type->llvm_type; break;
  case TYPEDESC_PTR:   desc->llvm_type = LLVMPointerType(base_type->llvm_type, 0); break;
  case TYPEDESC_ARRAY: desc->llvm_type = LLVMArrayType2(base_type->llvm_type, 0); break;
  case TYPEDESC_REF:   desc->llvm_type = LLVMPointerType(base_type->llvm_type, 0); break;
  case TYPEDESC_OPT:   desc->llvm_type = LLVMStructTypeInContext(builder->llvm_context, (LLVMTypeRef[]){ builder->desc_bool->llvm_type, base_type->llvm_type }, 2, false); break;
  default: unreachable();
  }

  set_add(set_modifier, desc);

  return (typedesc_t*)desc;
}

typebuilder_t* typebuilder_init(LLVMContextRef llvm_context, LLVMTargetDataRef llvm_layout)
{
  typebuilder_t* builder = (typebuilder_t*)malloc(sizeof(typebuilder_t));

  builder->llvm_context = llvm_context;
  builder->llvm_layout = llvm_layout;
  
  builder->desc_i8    = typedesc_init(TYPEDESC_I8);
  builder->desc_i16   = typedesc_init(TYPEDESC_I16);
  builder->desc_i32   = typedesc_init(TYPEDESC_I32);
  builder->desc_i64   = typedesc_init(TYPEDESC_I64);
  builder->desc_isize = typedesc_init(TYPEDESC_ISIZE);
  builder->desc_u8    = typedesc_init(TYPEDESC_U8);
  builder->desc_u16   = typedesc_init(TYPEDESC_U16);
  builder->desc_u32   = typedesc_init(TYPEDESC_U32);
  builder->desc_u64   = typedesc_init(TYPEDESC_U64);
  builder->desc_usize = typedesc_init(TYPEDESC_USIZE);
  builder->desc_f32   = typedesc_init(TYPEDESC_F32);
  builder->desc_f64   = typedesc_init(TYPEDESC_F64);
  builder->desc_bool  = typedesc_init(TYPEDESC_BOOL);
  builder->desc_unit  = typedesc_init(TYPEDESC_UNIT);

  builder->desc_i8->llvm_type    = LLVMInt8TypeInContext(builder->llvm_context);
  builder->desc_i16->llvm_type   = LLVMInt16TypeInContext(builder->llvm_context);
  builder->desc_i32->llvm_type   = LLVMInt32TypeInContext(builder->llvm_context);
  builder->desc_i64->llvm_type   = LLVMInt64TypeInContext(builder->llvm_context);
  builder->desc_isize->llvm_type = LLVMIntPtrTypeInContext(builder->llvm_context, builder->llvm_layout);
  builder->desc_u8->llvm_type    = LLVMInt8TypeInContext(builder->llvm_context);
  builder->desc_u16->llvm_type   = LLVMInt16TypeInContext(builder->llvm_context);
  builder->desc_u32->llvm_type   = LLVMInt32TypeInContext(builder->llvm_context);
  builder->desc_u64->llvm_type   = LLVMInt64TypeInContext(builder->llvm_context);
  builder->desc_usize->llvm_type = LLVMIntPtrTypeInContext(builder->llvm_context, builder->llvm_layout);
  builder->desc_f32->llvm_type   = LLVMFloatTypeInContext(builder->llvm_context);
  builder->desc_f64->llvm_type   = LLVMDoubleTypeInContext(builder->llvm_context);
  builder->desc_bool->llvm_type  = LLVMInt1TypeInContext(builder->llvm_context);
  builder->desc_unit->llvm_type  = LLVMVoidTypeInContext(builder->llvm_context);

  builder->set_mut    = set_init(typebuilder_cmp_modifier);
  builder->set_const  = set_init(typebuilder_cmp_modifier);
  builder->set_ptr    = set_init(typebuilder_cmp_modifier);
  builder->set_array  = set_init(typebuilder_cmp_modifier);
  builder->set_ref    = set_init(typebuilder_cmp_modifier);
  builder->set_opt    = set_init(typebuilder_cmp_modifier);
  builder->set_fun    = set_init(typebuilder_cmp_fun);
  builder->set_gen    = set_init(typebuilder_cmp_gen);
  builder->set_struct = set_init(typebuilder_cmp_struct);
  builder->set_union  = set_init(typebuilder_cmp_union);
  builder->set_enum   = set_init(typebuilder_cmp_enum);

  return builder;
}

void typebuilder_free(typebuilder_t* builder)
{
  typedesc_free(builder->desc_i8);
  typedesc_free(builder->desc_i16);
  typedesc_free(builder->desc_i32);
  typedesc_free(builder->desc_i64);
  typedesc_free(builder->desc_isize);
  typedesc_free(builder->desc_u8);
  typedesc_free(builder->desc_u16);
  typedesc_free(builder->desc_u32);
  typedesc_free(builder->desc_u64);
  typedesc_free(builder->desc_usize);
  typedesc_free(builder->desc_f32);
  typedesc_free(builder->desc_f64);
  typedesc_free(builder->desc_bool);
  typedesc_free(builder->desc_unit);

  set_for_each(builder->set_mut,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_const,  (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_ptr,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_array,  (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_ref,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_opt,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_fun,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_gen,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_struct, (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_union,  (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_enum,   (set_for_each_func_t)typedesc_free);

  set_free(builder->set_mut);
  set_free(builder->set_const);
  set_free(builder->set_ptr);
  set_free(builder->set_array);
  set_free(builder->set_ref);
  set_free(builder->set_opt);
  set_free(builder->set_fun);
  set_free(builder->set_gen);
  set_free(builder->set_struct);
  set_free(builder->set_union);
  set_free(builder->set_enum);

  free(builder);
}

typedesc_t* typebuilder_build_mut(typebuilder_t* builder, typedesc_t* base_type)
{
  return typebuilder_build_modifier(builder, TYPEDESC_MUT, base_type);
}

typedesc_t* typebuilder_build_const(typebuilder_t* builder, typedesc_t* base_type)
{
  return typebuilder_build_modifier(builder, TYPEDESC_CONST, base_type);
}

typedesc_t* typebuilder_build_ptr(typebuilder_t* builder, typedesc_t* base_type)
{
  return typebuilder_build_modifier(builder, TYPEDESC_PTR, base_type);
}

typedesc_t* typebuilder_build_array(typebuilder_t* builder, size_t length, typedesc_t* base_type)
{
  return typebuilder_build_modifier(builder, TYPEDESC_ARRAY, base_type);
}

typedesc_t* typebuilder_build_ref(typebuilder_t* builder, typedesc_t* base_type)
{
  return typebuilder_build_modifier(builder, TYPEDESC_REF, base_type);
}

typedesc_t* typebuilder_build_opt(typebuilder_t* builder, typedesc_t* base_type)
{
  return typebuilder_build_modifier(builder, TYPEDESC_OPT, base_type);
}

typedesc_t* typebuilder_build_i8(typebuilder_t* builder)
{
  return builder->desc_i8;
}

typedesc_t* typebuilder_build_i16(typebuilder_t* builder)
{
  return builder->desc_i16;
}

typedesc_t* typebuilder_build_i32(typebuilder_t* builder)
{
  return builder->desc_i32;
}

typedesc_t* typebuilder_build_i64(typebuilder_t* builder)
{
  return builder->desc_i64;
}

typedesc_t* typebuilder_build_isize(typebuilder_t* builder)
{
  return builder->desc_isize;
}

typedesc_t* typebuilder_build_u8(typebuilder_t* builder)
{
  return builder->desc_u8;
}

typedesc_t* typebuilder_build_u16(typebuilder_t* builder)
{
  return builder->desc_u16;
}

typedesc_t* typebuilder_build_u32(typebuilder_t* builder)
{
  return builder->desc_u32;
}

typedesc_t* typebuilder_build_u64(typebuilder_t* builder)
{
  return builder->desc_u64;
}

typedesc_t* typebuilder_build_usize(typebuilder_t* builder)
{
  return builder->desc_usize;
}

typedesc_t* typebuilder_build_f32(typebuilder_t* builder)
{
  return builder->desc_f32;
}

typedesc_t* typebuilder_build_f64(typebuilder_t* builder)
{
  return builder->desc_f64;
}

typedesc_t* typebuilder_build_bool(typebuilder_t* builder)
{
  return builder->desc_bool;
}

typedesc_t* typebuilder_build_unit(typebuilder_t* builder)
{
  return builder->desc_unit;
}

typedesc_t* typebuilder_build_fun(typebuilder_t* builder, typedesc_t* return_type, typedesc_t* param_types[], size_t param_count, bool is_vararg, abi_kind_t abi)
{
  typedesc_fun_t* desc = (typedesc_fun_t*)typedesc_init(TYPEDESC_FUN);
  desc->return_type = return_type;
  desc->param_types = param_count == 0 ? NULL : list_init_from_buffer(param_types, param_count);
  desc->is_vararg = is_vararg;
  desc->abi = abi;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_fun, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  LLVMTypeRef* llvm_param_types = NULL;

  if (param_count > 0)
  {
    llvm_param_types = (LLVMTypeRef*)malloc(sizeof(LLVMTypeRef) * param_count);

    for (size_t i = 0; i < param_count; i++)
      llvm_param_types[i] = param_types[i]->llvm_type;
  }
  
  desc->llvm_type = LLVMFunctionType(
    desc->return_type->llvm_type,
    llvm_param_types,
    (uint32_t)param_count,
    is_vararg
  );

  if (llvm_param_types != NULL)
    free(llvm_param_types);

  set_add(builder->set_fun, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_gen(typebuilder_t* builder, typedesc_t* yield_type, typedesc_t* param_types[], size_t param_count)
{
  typedesc_gen_t* desc = (typedesc_gen_t*)typedesc_init(TYPEDESC_GEN);
  desc->yield_type = yield_type;
  desc->param_types = param_count == 0 ? NULL : list_init_from_buffer(param_types, param_count);

  typedesc_t* result = (typedesc_t*)set_get(builder->set_gen, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = NULL;

  set_add(builder->set_gen, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_struct(typebuilder_t* builder, ast_node_t* node, typedesc_t* field_types[], size_t field_count)
{
  typedesc_struct_t* desc = (typedesc_struct_t*)typedesc_init(TYPEDESC_STRUCT);
  desc->node = node;
  desc->field_types = field_count == 0 ? NULL : list_init_from_buffer(field_types, field_count);

  typedesc_t* result = (typedesc_t*)set_get(builder->set_struct, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  LLVMTypeRef* llvm_field_types = NULL;

  if (field_count > 0)
  {
    llvm_field_types = (LLVMTypeRef*)malloc(sizeof(LLVMTypeRef) * field_count);

    for (size_t i = 0; i < field_count; i++)
      llvm_field_types[i] = field_types[i]->llvm_type;
  }

  desc->llvm_type = LLVMStructTypeInContext(
    builder->llvm_context,
    llvm_field_types,
    (uint32_t)field_count,
    false
  );

  if (llvm_field_types != NULL)
    free(llvm_field_types);

  set_add(builder->set_struct, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_union(typebuilder_t* builder, ast_node_t* node, typedesc_t* field_types[], size_t field_count)
{
  typedesc_union_t* desc = (typedesc_union_t*)typedesc_init(TYPEDESC_UNION);
  desc->node = node;
  desc->field_types = field_count == 0 ? NULL : list_init_from_buffer(field_types, field_count);

  typedesc_t* result = (typedesc_t*)set_get(builder->set_union, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  LLVMTypeRef max_field_type = NULL;
  size_t max_field_size = 0;

  for (size_t i = 0; i < field_count; i++)
  {
    size_t field_size = LLVMSizeOfTypeInBits(builder->llvm_layout, field_types[i]->llvm_type);

    if (field_size > max_field_size)
    {
      max_field_size = field_size;
      max_field_type = field_types[i]->llvm_type;
    }
  }

  desc->llvm_type = max_field_type;

  set_add(builder->set_union, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_enum(typebuilder_t* builder, ast_node_t* node)
{
  typedesc_enum_t* desc = (typedesc_enum_t*)typedesc_init(TYPEDESC_ENUM);
  desc->node = node;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_enum, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  size_t field_count = list_size(((ast_decl_enum_t*)node)->members);

       if (field_count <=  UINT8_MAX) desc->llvm_type = builder->desc_u8->llvm_type;
  else if (field_count <= UINT16_MAX) desc->llvm_type = builder->desc_u16->llvm_type;
  else if (field_count <= UINT32_MAX) desc->llvm_type = builder->desc_u32->llvm_type;
  else if (field_count <= UINT64_MAX) desc->llvm_type = builder->desc_u64->llvm_type;
  else unreachable();

  set_add(builder->set_enum, desc);

  return (typedesc_t*)desc;
}
