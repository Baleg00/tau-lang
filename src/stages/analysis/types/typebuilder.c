/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typebuilder.h"

#include <stdint.h>
#include <string.h>

#include "ast/ast.h"
#include "utils/common.h"
#include "utils/hash.h"
#include "utils/collections/set.h"
#include "utils/memory/memtrace.h"

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
  typedesc_t* desc_char;
  typedesc_t* desc_bool;
  typedesc_t* desc_unit;

  set_t* set_mut;
  set_t* set_ptr;
  set_t* set_array;
  set_t* set_ref;
  set_t* set_opt;
  set_t* set_fun;
  set_t* set_struct;
  set_t* set_union;
  set_t* set_enum;
  set_t* set_var;
};

static int typebuilder_cmp_mut(void* lhs, void* rhs)
{
  return (int)(((typedesc_mut_t*)lhs)->base_type - ((typedesc_mut_t*)rhs)->base_type);
}

static int typebuilder_cmp_ptr(void* lhs, void* rhs)
{
  return (int)(((typedesc_ptr_t*)lhs)->base_type - ((typedesc_ptr_t*)rhs)->base_type);
}

static int typebuilder_cmp_array(void* lhs, void* rhs)
{
  typedesc_array_t* lhs_desc = (typedesc_array_t*)lhs;
  typedesc_array_t* rhs_desc = (typedesc_array_t*)rhs;

  // TODO: fix integer overflow
  int cmp1 = (int)(lhs_desc->base_type - rhs_desc->base_type);
  int cmp2 = (int)lhs_desc->length - (int)rhs_desc->length;

  if (cmp1 != 0)
    return cmp1;

  return cmp2;
}

static int typebuilder_cmp_ref(void* lhs, void* rhs)
{
  return (int)(((typedesc_ref_t*)lhs)->base_type - ((typedesc_ref_t*)rhs)->base_type);
}

static int typebuilder_cmp_opt(void* lhs, void* rhs)
{
  return (int)(((typedesc_opt_t*)lhs)->base_type - ((typedesc_opt_t*)rhs)->base_type);
}

static int typebuilder_cmp_fun(void* lhs, void* rhs)
{
  typedesc_fun_t* lhs_desc = (typedesc_fun_t*)lhs;
  typedesc_fun_t* rhs_desc = (typedesc_fun_t*)rhs;

  size_t lhs_hash = (size_t)hash_digest(&lhs_desc->return_type, sizeof(typedesc_t*));

  VECTOR_FOR_LOOP(i, lhs_desc->param_types)
  {
    typedesc_t* param_desc = (typedesc_t*)vector_get(lhs_desc->param_types, i);
    lhs_hash = hash_combine_with_data(lhs_hash, &param_desc, sizeof(typedesc_t*));
  }

  lhs_hash = hash_combine_with_data(lhs_hash, &lhs_desc->is_vararg, sizeof(bool));
  lhs_hash = hash_combine_with_data(lhs_hash, &lhs_desc->callconv, sizeof(callconv_kind_t));

  size_t rhs_hash = (size_t)hash_digest(&rhs_desc->return_type, sizeof(typedesc_t*));

  VECTOR_FOR_LOOP(i, rhs_desc->param_types)
  {
    typedesc_t* param_desc = (typedesc_t*)vector_get(rhs_desc->param_types, i);
    rhs_hash = hash_combine_with_data(rhs_hash, &param_desc, sizeof(typedesc_t*));
  }

  rhs_hash = hash_combine_with_data(rhs_hash, &rhs_desc->is_vararg, sizeof(bool));
  rhs_hash = hash_combine_with_data(rhs_hash, &rhs_desc->callconv, sizeof(callconv_kind_t));

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

  ast_decl_struct_t* lhs_node = (ast_decl_struct_t*)lhs_desc->node;
  ast_decl_struct_t* rhs_node = (ast_decl_struct_t*)rhs_desc->node;

  string_view_t lhs_id_view = token_to_string_view(lhs_node->id->tok);
  string_view_t rhs_id_view = token_to_string_view(rhs_node->id->tok);

  const void* lhs_id_view_begin = string_view_begin(lhs_id_view);
  const void* rhs_id_view_begin = string_view_begin(rhs_id_view);

  size_t lhs_id_view_len = string_view_length(lhs_id_view);
  size_t rhs_id_view_len = string_view_length(rhs_id_view);

  size_t lhs_hash = (size_t)hash_digest(lhs_id_view_begin, lhs_id_view_len);
  size_t rhs_hash = (size_t)hash_digest(rhs_id_view_begin, rhs_id_view_len);

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

  size_t lhs_hash = (size_t)hash_digest(&lhs_desc->node, sizeof(ast_node_t*));

  VECTOR_FOR_LOOP(i, lhs_desc->field_types)
  {
    typedesc_t* field_desc = (typedesc_t*)vector_get(lhs_desc->field_types, i);
    lhs_hash = hash_combine_with_data(lhs_hash, &field_desc, sizeof(typedesc_t*));
  }

  size_t rhs_hash = (size_t)hash_digest(&rhs_desc->node, sizeof(ast_node_t*));

  VECTOR_FOR_LOOP(i, rhs_desc->field_types)
  {
    typedesc_t* field_desc = (typedesc_t*)vector_get(rhs_desc->field_types, i);
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

  size_t lhs_hash = (size_t)hash_digest(&lhs_desc->node, sizeof(ast_node_t*));
  size_t rhs_hash = (size_t)hash_digest(&rhs_desc->node, sizeof(ast_node_t*));

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int typebuilder_cmp_var(void* lhs, void* rhs)
{
  typedesc_var_t* lhs_desc = (typedesc_var_t*)lhs;
  typedesc_var_t* rhs_desc = (typedesc_var_t*)rhs;

  if (lhs_desc->id < rhs_desc->id)
    return -1;

  if (lhs_desc->id > rhs_desc->id)
    return 1;

  return 0;
}

typebuilder_t* typebuilder_init(LLVMContextRef llvm_context, LLVMTargetDataRef llvm_layout)
{
  typebuilder_t* builder = (typebuilder_t*)malloc(sizeof(typebuilder_t));

  builder->llvm_context = llvm_context;
  builder->llvm_layout = llvm_layout;

  builder->desc_i8    = (typedesc_t*)typedesc_prim_i8_init   ();
  builder->desc_i16   = (typedesc_t*)typedesc_prim_i16_init  ();
  builder->desc_i32   = (typedesc_t*)typedesc_prim_i32_init  ();
  builder->desc_i64   = (typedesc_t*)typedesc_prim_i64_init  ();
  builder->desc_isize = (typedesc_t*)typedesc_prim_isize_init();
  builder->desc_u8    = (typedesc_t*)typedesc_prim_u8_init   ();
  builder->desc_u16   = (typedesc_t*)typedesc_prim_u16_init  ();
  builder->desc_u32   = (typedesc_t*)typedesc_prim_u32_init  ();
  builder->desc_u64   = (typedesc_t*)typedesc_prim_u64_init  ();
  builder->desc_usize = (typedesc_t*)typedesc_prim_usize_init();
  builder->desc_f32   = (typedesc_t*)typedesc_prim_f32_init  ();
  builder->desc_f64   = (typedesc_t*)typedesc_prim_f64_init  ();
  builder->desc_char  = (typedesc_t*)typedesc_prim_char_init ();
  builder->desc_bool  = (typedesc_t*)typedesc_prim_bool_init ();
  builder->desc_unit  = (typedesc_t*)typedesc_prim_unit_init ();

  builder->desc_i8->llvm_type    = LLVMInt8TypeInContext  (builder->llvm_context                      );
  builder->desc_i16->llvm_type   = LLVMInt16TypeInContext (builder->llvm_context                      );
  builder->desc_i32->llvm_type   = LLVMInt32TypeInContext (builder->llvm_context                      );
  builder->desc_i64->llvm_type   = LLVMInt64TypeInContext (builder->llvm_context                      );
  builder->desc_isize->llvm_type = LLVMIntPtrTypeInContext(builder->llvm_context, builder->llvm_layout);
  builder->desc_u8->llvm_type    = LLVMInt8TypeInContext  (builder->llvm_context                      );
  builder->desc_u16->llvm_type   = LLVMInt16TypeInContext (builder->llvm_context                      );
  builder->desc_u32->llvm_type   = LLVMInt32TypeInContext (builder->llvm_context                      );
  builder->desc_u64->llvm_type   = LLVMInt64TypeInContext (builder->llvm_context                      );
  builder->desc_usize->llvm_type = LLVMIntPtrTypeInContext(builder->llvm_context, builder->llvm_layout);
  builder->desc_f32->llvm_type   = LLVMFloatTypeInContext (builder->llvm_context                      );
  builder->desc_f64->llvm_type   = LLVMDoubleTypeInContext(builder->llvm_context                      );
  builder->desc_char->llvm_type  = LLVMInt32TypeInContext (builder->llvm_context                      );
  builder->desc_bool->llvm_type  = LLVMInt1TypeInContext  (builder->llvm_context                      );
  builder->desc_unit->llvm_type  = LLVMVoidTypeInContext  (builder->llvm_context                      );

  builder->set_mut    = set_init(typebuilder_cmp_mut   );
  builder->set_ptr    = set_init(typebuilder_cmp_ptr   );
  builder->set_array  = set_init(typebuilder_cmp_array );
  builder->set_ref    = set_init(typebuilder_cmp_ref   );
  builder->set_opt    = set_init(typebuilder_cmp_opt   );
  builder->set_fun    = set_init(typebuilder_cmp_fun   );
  builder->set_struct = set_init(typebuilder_cmp_struct);
  builder->set_union  = set_init(typebuilder_cmp_union );
  builder->set_enum   = set_init(typebuilder_cmp_enum  );
  builder->set_var    = set_init(typebuilder_cmp_var   );

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
  typedesc_free(builder->desc_char);
  typedesc_free(builder->desc_bool);
  typedesc_free(builder->desc_unit);

  set_for_each(builder->set_mut,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_ptr,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_array,  (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_ref,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_opt,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_fun,    (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_struct, (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_union,  (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_enum,   (set_for_each_func_t)typedesc_free);
  set_for_each(builder->set_var,    (set_for_each_func_t)typedesc_free);

  set_free(builder->set_mut);
  set_free(builder->set_ptr);
  set_free(builder->set_array);
  set_free(builder->set_ref);
  set_free(builder->set_opt);
  set_free(builder->set_fun);
  set_free(builder->set_struct);
  set_free(builder->set_union);
  set_free(builder->set_enum);
  set_free(builder->set_var);

  free(builder);
}

typedesc_t* typebuilder_build_mut(typebuilder_t* builder, typedesc_t* base_type)
{
  ASSERT(typedesc_can_add_mut(base_type));

  typedesc_mut_t* desc = (typedesc_mut_t*)typedesc_mut_init();
  desc->base_type = base_type;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_mut, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = base_type->llvm_type;

  set_add(builder->set_mut, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_ptr(typebuilder_t* builder, typedesc_t* base_type)
{
  ASSERT(typedesc_can_add_ptr(base_type));

  typedesc_ptr_t* desc = (typedesc_ptr_t*)typedesc_ptr_init();
  desc->base_type = base_type;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_ptr, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMPointerType(base_type->llvm_type, 0);

  set_add(builder->set_ptr, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_array(typebuilder_t* builder, size_t length, typedesc_t* base_type)
{
  ASSERT(typedesc_can_add_array(base_type));

  typedesc_array_t* desc = (typedesc_array_t*)typedesc_array_init();
  desc->base_type = base_type;
  desc->length = length;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_array, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMArrayType2(base_type->llvm_type, length);

  set_add(builder->set_array, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_ref(typebuilder_t* builder, typedesc_t* base_type)
{
  ASSERT(typedesc_can_add_ref(base_type));

  typedesc_ref_t* desc = (typedesc_ref_t*)typedesc_ref_init();
  desc->base_type = base_type;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_ref, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMPointerType(base_type->llvm_type, 0);

  set_add(builder->set_ref, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_opt(typebuilder_t* builder, typedesc_t* base_type)
{
  ASSERT(typedesc_can_add_opt(base_type));

  typedesc_ref_t* desc = (typedesc_ref_t*)typedesc_opt_init();
  desc->base_type = base_type;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_opt, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMStructTypeInContext(builder->llvm_context, (LLVMTypeRef[]){ builder->desc_bool->llvm_type, base_type->llvm_type }, 2, false);

  set_add(builder->set_opt, desc);

  return (typedesc_t*)desc;
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

typedesc_t* typebuilder_build_char(typebuilder_t* builder)
{
  return builder->desc_char;
}

typedesc_t* typebuilder_build_bool(typebuilder_t* builder)
{
  return builder->desc_bool;
}

typedesc_t* typebuilder_build_unit(typebuilder_t* builder)
{
  return builder->desc_unit;
}

typedesc_t* typebuilder_build_fun(typebuilder_t* builder, typedesc_t* return_type, typedesc_t* param_types[], size_t param_count, bool is_vararg, callconv_kind_t callconv)
{
  typedesc_fun_t* desc = typedesc_fun_init();
  desc->return_type = return_type;
  desc->param_types = param_count == 0 ? NULL : vector_init_from_buffer(param_types, param_count);
  desc->is_vararg = is_vararg;
  desc->callconv = callconv;

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

typedesc_t* typebuilder_build_struct(typebuilder_t* builder, ast_node_t* node, typedesc_t* field_types[], size_t field_count)
{
  typedesc_struct_t* desc = typedesc_struct_init();
  desc->node = node;
  desc->field_types = field_count == 0 ? NULL : vector_init_from_buffer(field_types, field_count);

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

typedesc_t* typebuilder_build_struct_opaque(typebuilder_t* builder, ast_node_t* node)
{
  typedesc_struct_t* desc = typedesc_struct_init();
  desc->node = node;
  desc->field_types = NULL;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_struct, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  ast_decl_struct_t* struct_node = (ast_decl_struct_t*)node;
  string_t* id_str = token_to_string(struct_node->id->tok);

  desc->llvm_type = LLVMStructCreateNamed(builder->llvm_context, string_begin(id_str));

  string_free(id_str);

  set_add(builder->set_struct, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_union(typebuilder_t* builder, ast_node_t* node, typedesc_t* field_types[], size_t field_count)
{
  typedesc_union_t* desc = typedesc_union_init();
  desc->node = node;
  desc->field_types = field_count == 0 ? NULL : vector_init_from_buffer(field_types, field_count);

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
    size_t field_size = (size_t)LLVMSizeOfTypeInBits(builder->llvm_layout, field_types[i]->llvm_type);

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
  typedesc_enum_t* desc = typedesc_enum_init();
  desc->node = node;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_enum, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  size_t field_count = vector_size(((ast_decl_enum_t*)node)->members);

       if (field_count <=  UINT8_MAX) desc->llvm_type = builder->desc_u8->llvm_type;
  else if (field_count <= UINT16_MAX) desc->llvm_type = builder->desc_u16->llvm_type;
  else if (field_count <= UINT32_MAX) desc->llvm_type = builder->desc_u32->llvm_type;
  else if (field_count <= UINT64_MAX) desc->llvm_type = builder->desc_u64->llvm_type;
  else UNREACHABLE();

  set_add(builder->set_enum, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_build_var(typebuilder_t* builder, uint64_t id)
{
  typedesc_var_t* desc = typedesc_var_init();
  desc->id = id;

  typedesc_t* result = (typedesc_t*)set_get(builder->set_var, desc);

  if (result != NULL)
  {
    typedesc_free((typedesc_t*)desc);
    return result;
  }

  set_add(builder->set_var, desc);

  return (typedesc_t*)desc;
}

typedesc_t* typebuilder_struct_set_body(typebuilder_t* builder, typedesc_t* desc, typedesc_t* field_types[], size_t field_count)
{
  ASSERT(set_contains(builder->set_struct, desc));

  LLVMTypeRef* llvm_field_types = NULL;

  if (field_count > 0)
  {
    llvm_field_types = (LLVMTypeRef*)malloc(sizeof(LLVMTypeRef) * field_count);

    for (size_t i = 0; i < field_count; i++)
    {
      llvm_field_types[i] = field_types[i]->llvm_type;
    }
  }

  LLVMStructSetBody(
    desc->llvm_type,
    llvm_field_types,
    (uint32_t)field_count,
    false
   );

  if (llvm_field_types != NULL)
  {
    free(llvm_field_types);
  }

  return (typedesc_t*)desc;
}
