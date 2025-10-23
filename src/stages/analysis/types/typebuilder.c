/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/analysis/types/typebuilder.h"

#include "ast/ast.h"
#include "utils/hash.h"
#include "utils/collections/set.h"

struct tau_typebuilder_t
{
  LLVMContextRef llvm_context;
  LLVMTargetDataRef llvm_layout;

  tau_typedesc_t* desc_i8;
  tau_typedesc_t* desc_i16;
  tau_typedesc_t* desc_i32;
  tau_typedesc_t* desc_i64;
  tau_typedesc_t* desc_isize;
  tau_typedesc_t* desc_u8;
  tau_typedesc_t* desc_u16;
  tau_typedesc_t* desc_u32;
  tau_typedesc_t* desc_u64;
  tau_typedesc_t* desc_usize;
  tau_typedesc_t* desc_f32;
  tau_typedesc_t* desc_f64;
  tau_typedesc_t* desc_c64;
  tau_typedesc_t* desc_c128;
  tau_typedesc_t* desc_char;
  tau_typedesc_t* desc_bool;
  tau_typedesc_t* desc_unit;
  tau_typedesc_t* desc_poison;

  tau_set_t* tau_set_mut;
  tau_set_t* tau_set_ptr;
  tau_set_t* tau_set_array;
  tau_set_t* tau_set_ref;
  tau_set_t* tau_set_opt;
  tau_set_t* tau_set_fun;
  tau_set_t* tau_set_vec;
  tau_set_t* tau_set_mat;
  tau_set_t* tau_set_struct;
  tau_set_t* tau_set_union;
  tau_set_t* tau_set_enum;
  tau_set_t* tau_set_var;
};

static int tau_typebuilder_cmp_mut(const void* lhs, const void* rhs)
{
  return (int)(((tau_typedesc_mut_t*)lhs)->base_type - ((tau_typedesc_mut_t*)rhs)->base_type);
}

static int tau_typebuilder_cmp_ptr(const void* lhs, const void* rhs)
{
  return (int)(((tau_typedesc_ptr_t*)lhs)->base_type - ((tau_typedesc_ptr_t*)rhs)->base_type);
}

static int tau_typebuilder_cmp_array(const void* lhs, const void* rhs)
{
  tau_typedesc_array_t* lhs_desc = (tau_typedesc_array_t*)lhs;
  tau_typedesc_array_t* rhs_desc = (tau_typedesc_array_t*)rhs;

  // TODO: fix integer overflow
  int cmp1 = (int)(lhs_desc->base_type - rhs_desc->base_type);
  int cmp2 = (int)lhs_desc->length - (int)rhs_desc->length;

  if (cmp1 != 0)
    return cmp1;

  return cmp2;
}

static int tau_typebuilder_cmp_ref(const void* lhs, const void* rhs)
{
  return (int)(((tau_typedesc_ref_t*)lhs)->base_type - ((tau_typedesc_ref_t*)rhs)->base_type);
}

static int tau_typebuilder_cmp_opt(const void* lhs, const void* rhs)
{
  return (int)(((tau_typedesc_opt_t*)lhs)->base_type - ((tau_typedesc_opt_t*)rhs)->base_type);
}

static int tau_typebuilder_cmp_fun(const void* lhs, const void* rhs)
{
  tau_typedesc_fun_t* lhs_desc = (tau_typedesc_fun_t*)lhs;
  tau_typedesc_fun_t* rhs_desc = (tau_typedesc_fun_t*)rhs;

  uint64_t lhs_hash = tau_hash_digest(&lhs_desc->return_type, sizeof(tau_typedesc_t*));

  TAU_VECTOR_FOR_LOOP(i, lhs_desc->param_types)
  {
    tau_typedesc_t* param_desc = (tau_typedesc_t*)tau_vector_get(lhs_desc->param_types, i);
    lhs_hash = tau_hash_combine_with_data(lhs_hash, &param_desc, sizeof(tau_typedesc_t*));
  }

  lhs_hash = tau_hash_combine_with_data(lhs_hash, &lhs_desc->is_vararg, sizeof(bool));
  lhs_hash = tau_hash_combine_with_data(lhs_hash, &lhs_desc->callconv, sizeof(tau_callconv_kind_t));

  uint64_t rhs_hash = tau_hash_digest(&rhs_desc->return_type, sizeof(tau_typedesc_t*));

  TAU_VECTOR_FOR_LOOP(i, rhs_desc->param_types)
  {
    tau_typedesc_t* param_desc = (tau_typedesc_t*)tau_vector_get(rhs_desc->param_types, i);
    rhs_hash = tau_hash_combine_with_data(rhs_hash, &param_desc, sizeof(tau_typedesc_t*));
  }

  rhs_hash = tau_hash_combine_with_data(rhs_hash, &rhs_desc->is_vararg, sizeof(bool));
  rhs_hash = tau_hash_combine_with_data(rhs_hash, &rhs_desc->callconv, sizeof(tau_callconv_kind_t));

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int tau_typebuilder_cmp_vec(const void* lhs, const void* rhs)
{
  tau_typedesc_vec_t* lhs_desc = (tau_typedesc_vec_t*)lhs;
  tau_typedesc_vec_t* rhs_desc = (tau_typedesc_vec_t*)rhs;

  uint64_t lhs_hash = tau_hash_digest(&lhs_desc->size, sizeof(size_t));
  uint64_t rhs_hash = tau_hash_digest(&rhs_desc->size, sizeof(size_t));

  lhs_hash = tau_hash_combine_with_data(lhs_hash, &lhs_desc->base_type, sizeof(tau_typedesc_t*));
  rhs_hash = tau_hash_combine_with_data(rhs_hash, &rhs_desc->base_type, sizeof(tau_typedesc_t*));

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int tau_typebuilder_cmp_mat(const void* lhs, const void* rhs)
{
  tau_typedesc_mat_t* lhs_desc = (tau_typedesc_mat_t*)lhs;
  tau_typedesc_mat_t* rhs_desc = (tau_typedesc_mat_t*)rhs;

  uint64_t lhs_hash = tau_hash_digest(&lhs_desc->rows, sizeof(size_t));
  uint64_t rhs_hash = tau_hash_digest(&rhs_desc->rows, sizeof(size_t));

  lhs_hash = tau_hash_combine_with_data(lhs_hash, &lhs_desc->cols, sizeof(size_t));
  rhs_hash = tau_hash_combine_with_data(rhs_hash, &rhs_desc->cols, sizeof(size_t));

  lhs_hash = tau_hash_combine_with_data(lhs_hash, &lhs_desc->base_type, sizeof(tau_typedesc_t*));
  rhs_hash = tau_hash_combine_with_data(rhs_hash, &rhs_desc->base_type, sizeof(tau_typedesc_t*));

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int tau_typebuilder_cmp_struct(const void* lhs, const void* rhs)
{
  tau_typedesc_struct_t* lhs_desc = (tau_typedesc_struct_t*)lhs;
  tau_typedesc_struct_t* rhs_desc = (tau_typedesc_struct_t*)rhs;

  tau_ast_decl_struct_t* lhs_node = (tau_ast_decl_struct_t*)lhs_desc->node;
  tau_ast_decl_struct_t* rhs_node = (tau_ast_decl_struct_t*)rhs_desc->node;

  tau_string_view_t lhs_id_view = tau_token_to_string_view(lhs_node->id->tok);
  tau_string_view_t rhs_id_view = tau_token_to_string_view(rhs_node->id->tok);

  const void* lhs_id_view_begin = tau_string_view_begin(lhs_id_view);
  const void* rhs_id_view_begin = tau_string_view_begin(rhs_id_view);

  size_t lhs_id_view_len = tau_string_view_length(lhs_id_view);
  size_t rhs_id_view_len = tau_string_view_length(rhs_id_view);

  uint64_t lhs_hash = tau_hash_digest(lhs_id_view_begin, lhs_id_view_len);
  uint64_t rhs_hash = tau_hash_digest(rhs_id_view_begin, rhs_id_view_len);

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int tau_typebuilder_cmp_union(const void* lhs, const void* rhs)
{
  tau_typedesc_union_t* lhs_desc = (tau_typedesc_union_t*)lhs;
  tau_typedesc_union_t* rhs_desc = (tau_typedesc_union_t*)rhs;

  uint64_t lhs_hash = tau_hash_digest(&lhs_desc->node, sizeof(tau_ast_node_t*));

  TAU_VECTOR_FOR_LOOP(i, lhs_desc->field_types)
  {
    tau_typedesc_t* field_desc = (tau_typedesc_t*)tau_vector_get(lhs_desc->field_types, i);
    lhs_hash = tau_hash_combine_with_data(lhs_hash, &field_desc, sizeof(tau_typedesc_t*));
  }

  uint64_t rhs_hash = tau_hash_digest(&rhs_desc->node, sizeof(tau_ast_node_t*));

  TAU_VECTOR_FOR_LOOP(i, rhs_desc->field_types)
  {
    tau_typedesc_t* field_desc = (tau_typedesc_t*)tau_vector_get(rhs_desc->field_types, i);
    rhs_hash = tau_hash_combine_with_data(rhs_hash, &field_desc, sizeof(tau_typedesc_t*));
  }

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int tau_typebuilder_cmp_enum(const void* lhs, const void* rhs)
{
  tau_typedesc_enum_t* lhs_desc = (tau_typedesc_enum_t*)lhs;
  tau_typedesc_enum_t* rhs_desc = (tau_typedesc_enum_t*)rhs;

  uint64_t lhs_hash = tau_hash_digest(&lhs_desc->node, sizeof(tau_ast_node_t*));
  uint64_t rhs_hash = tau_hash_digest(&rhs_desc->node, sizeof(tau_ast_node_t*));

  if (lhs_hash < rhs_hash)
    return -1;

  if (lhs_hash > rhs_hash)
    return 1;

  return 0;
}

static int tau_typebuilder_cmp_var(const void* lhs, const void* rhs)
{
  tau_typedesc_var_t* lhs_desc = (tau_typedesc_var_t*)lhs;
  tau_typedesc_var_t* rhs_desc = (tau_typedesc_var_t*)rhs;

  if (lhs_desc->id < rhs_desc->id)
    return -1;

  if (lhs_desc->id > rhs_desc->id)
    return 1;

  return 0;
}

tau_typebuilder_t* tau_typebuilder_init(LLVMContextRef llvm_context, LLVMTargetDataRef llvm_layout)
{
  tau_typebuilder_t* builder = (tau_typebuilder_t*)malloc(sizeof(tau_typebuilder_t));

  builder->llvm_context = llvm_context;
  builder->llvm_layout = llvm_layout;

  builder->desc_i8     = (tau_typedesc_t*)tau_typedesc_prim_i8_init   ();
  builder->desc_i16    = (tau_typedesc_t*)tau_typedesc_prim_i16_init  ();
  builder->desc_i32    = (tau_typedesc_t*)tau_typedesc_prim_i32_init  ();
  builder->desc_i64    = (tau_typedesc_t*)tau_typedesc_prim_i64_init  ();
  builder->desc_isize  = (tau_typedesc_t*)tau_typedesc_prim_isize_init();
  builder->desc_u8     = (tau_typedesc_t*)tau_typedesc_prim_u8_init   ();
  builder->desc_u16    = (tau_typedesc_t*)tau_typedesc_prim_u16_init  ();
  builder->desc_u32    = (tau_typedesc_t*)tau_typedesc_prim_u32_init  ();
  builder->desc_u64    = (tau_typedesc_t*)tau_typedesc_prim_u64_init  ();
  builder->desc_usize  = (tau_typedesc_t*)tau_typedesc_prim_usize_init();
  builder->desc_f32    = (tau_typedesc_t*)tau_typedesc_prim_f32_init  ();
  builder->desc_f64    = (tau_typedesc_t*)tau_typedesc_prim_f64_init  ();
  builder->desc_c64    = (tau_typedesc_t*)tau_typedesc_prim_c64_init  ();
  builder->desc_c128   = (tau_typedesc_t*)tau_typedesc_prim_c128_init ();
  builder->desc_char   = (tau_typedesc_t*)tau_typedesc_prim_char_init ();
  builder->desc_bool   = (tau_typedesc_t*)tau_typedesc_prim_bool_init ();
  builder->desc_unit   = (tau_typedesc_t*)tau_typedesc_prim_unit_init ();
  builder->desc_poison = (tau_typedesc_t*)tau_typedesc_poison_init    ();

  builder->desc_i8->llvm_type     = LLVMInt8TypeInContext  (builder->llvm_context                      );
  builder->desc_i16->llvm_type    = LLVMInt16TypeInContext (builder->llvm_context                      );
  builder->desc_i32->llvm_type    = LLVMInt32TypeInContext (builder->llvm_context                      );
  builder->desc_i64->llvm_type    = LLVMInt64TypeInContext (builder->llvm_context                      );
  builder->desc_isize->llvm_type  = LLVMIntPtrTypeInContext(builder->llvm_context, builder->llvm_layout);
  builder->desc_u8->llvm_type     = LLVMInt8TypeInContext  (builder->llvm_context                      );
  builder->desc_u16->llvm_type    = LLVMInt16TypeInContext (builder->llvm_context                      );
  builder->desc_u32->llvm_type    = LLVMInt32TypeInContext (builder->llvm_context                      );
  builder->desc_u64->llvm_type    = LLVMInt64TypeInContext (builder->llvm_context                      );
  builder->desc_usize->llvm_type  = LLVMIntPtrTypeInContext(builder->llvm_context, builder->llvm_layout);
  builder->desc_f32->llvm_type    = LLVMFloatTypeInContext (builder->llvm_context                      );
  builder->desc_f64->llvm_type    = LLVMDoubleTypeInContext(builder->llvm_context                      );
  builder->desc_char->llvm_type   = LLVMInt32TypeInContext (builder->llvm_context                      );
  builder->desc_bool->llvm_type   = LLVMInt1TypeInContext  (builder->llvm_context                      );
  builder->desc_unit->llvm_type   = LLVMVoidTypeInContext  (builder->llvm_context                      );
  builder->desc_poison->llvm_type = LLVMVoidTypeInContext  (builder->llvm_context                      );

  LLVMTypeRef llvm_c64_type = LLVMStructCreateNamed(builder->llvm_context, "complex64");
  LLVMStructSetBody(llvm_c64_type, (LLVMTypeRef[]){ builder->desc_f32->llvm_type, builder->desc_f32->llvm_type }, 2, false);
  builder->desc_c64->llvm_type = llvm_c64_type;

  LLVMTypeRef llvm_c128_type = LLVMStructCreateNamed(builder->llvm_context, "complex128");
  LLVMStructSetBody(llvm_c128_type, (LLVMTypeRef[]){ builder->desc_f64->llvm_type, builder->desc_f64->llvm_type }, 2, false);
  builder->desc_c128->llvm_type = llvm_c128_type;

  builder->tau_set_mut    = tau_set_init(tau_typebuilder_cmp_mut   );
  builder->tau_set_ptr    = tau_set_init(tau_typebuilder_cmp_ptr   );
  builder->tau_set_array  = tau_set_init(tau_typebuilder_cmp_array );
  builder->tau_set_ref    = tau_set_init(tau_typebuilder_cmp_ref   );
  builder->tau_set_opt    = tau_set_init(tau_typebuilder_cmp_opt   );
  builder->tau_set_fun    = tau_set_init(tau_typebuilder_cmp_fun   );
  builder->tau_set_vec    = tau_set_init(tau_typebuilder_cmp_vec   );
  builder->tau_set_mat    = tau_set_init(tau_typebuilder_cmp_mat   );
  builder->tau_set_struct = tau_set_init(tau_typebuilder_cmp_struct);
  builder->tau_set_union  = tau_set_init(tau_typebuilder_cmp_union );
  builder->tau_set_enum   = tau_set_init(tau_typebuilder_cmp_enum  );
  builder->tau_set_var    = tau_set_init(tau_typebuilder_cmp_var   );

  return builder;
}

void tau_typebuilder_free(tau_typebuilder_t* builder)
{
  tau_typedesc_free(builder->desc_i8);
  tau_typedesc_free(builder->desc_i16);
  tau_typedesc_free(builder->desc_i32);
  tau_typedesc_free(builder->desc_i64);
  tau_typedesc_free(builder->desc_isize);
  tau_typedesc_free(builder->desc_u8);
  tau_typedesc_free(builder->desc_u16);
  tau_typedesc_free(builder->desc_u32);
  tau_typedesc_free(builder->desc_u64);
  tau_typedesc_free(builder->desc_usize);
  tau_typedesc_free(builder->desc_f32);
  tau_typedesc_free(builder->desc_f64);
  tau_typedesc_free(builder->desc_c64);
  tau_typedesc_free(builder->desc_c128);
  tau_typedesc_free(builder->desc_char);
  tau_typedesc_free(builder->desc_bool);
  tau_typedesc_free(builder->desc_unit);
  tau_typedesc_free(builder->desc_poison);

  tau_set_for_each(builder->tau_set_mut,    (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_ptr,    (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_array,  (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_ref,    (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_opt,    (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_fun,    (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_vec,    (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_mat,    (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_struct, (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_union,  (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_enum,   (tau_set_for_each_func_t)tau_typedesc_free);
  tau_set_for_each(builder->tau_set_var,    (tau_set_for_each_func_t)tau_typedesc_free);

  tau_set_free(builder->tau_set_mut);
  tau_set_free(builder->tau_set_ptr);
  tau_set_free(builder->tau_set_array);
  tau_set_free(builder->tau_set_ref);
  tau_set_free(builder->tau_set_opt);
  tau_set_free(builder->tau_set_fun);
  tau_set_free(builder->tau_set_vec);
  tau_set_free(builder->tau_set_mat);
  tau_set_free(builder->tau_set_struct);
  tau_set_free(builder->tau_set_union);
  tau_set_free(builder->tau_set_enum);
  tau_set_free(builder->tau_set_var);

  free(builder);
}

tau_typedesc_t* tau_typebuilder_build_mut(tau_typebuilder_t* builder, tau_typedesc_t* base_type)
{
  TAU_ASSERT(tau_typedesc_can_add_mut(base_type));

  tau_typedesc_mut_t* desc = tau_typedesc_mut_init();
  desc->base_type = base_type;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_mut, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = base_type->llvm_type;

  tau_set_add(builder->tau_set_mut, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_ptr(tau_typebuilder_t* builder, tau_typedesc_t* base_type)
{
  TAU_ASSERT(tau_typedesc_can_add_ptr(base_type));

  tau_typedesc_ptr_t* desc = tau_typedesc_ptr_init();
  desc->base_type = base_type;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_ptr, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMPointerType(base_type->llvm_type, 0);

  tau_set_add(builder->tau_set_ptr, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_array(tau_typebuilder_t* builder, size_t length, tau_typedesc_t* base_type)
{
  TAU_ASSERT(tau_typedesc_can_add_array(base_type));

  tau_typedesc_array_t* desc = tau_typedesc_array_init();
  desc->base_type = base_type;
  desc->length = length;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_array, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMArrayType2(base_type->llvm_type, length);

  tau_set_add(builder->tau_set_array, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_ref(tau_typebuilder_t* builder, tau_typedesc_t* base_type)
{
  TAU_ASSERT(tau_typedesc_can_add_ref(base_type));

  tau_typedesc_ref_t* desc = tau_typedesc_ref_init();
  desc->base_type = base_type;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_ref, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMPointerType(base_type->llvm_type, 0);

  tau_set_add(builder->tau_set_ref, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_opt(tau_typebuilder_t* builder, tau_typedesc_t* base_type)
{
  TAU_ASSERT(tau_typedesc_can_add_opt(base_type));

  tau_typedesc_opt_t* desc = tau_typedesc_opt_init();
  desc->base_type = base_type;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_opt, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMStructTypeInContext(builder->llvm_context, (LLVMTypeRef[]){ builder->desc_bool->llvm_type, base_type->llvm_type }, 2, false);

  tau_set_add(builder->tau_set_opt, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_vec(tau_typebuilder_t* builder, size_t size, tau_typedesc_t* base_type)
{
  TAU_ASSERT(tau_typedesc_is_integer(base_type) || tau_typedesc_is_float(base_type));

  tau_typedesc_vec_t* desc = tau_typedesc_vec_init();
  desc->size = size;
  desc->base_type = base_type;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_vec, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMVectorType(base_type->llvm_type, (uint32_t)size);

  tau_set_add(builder->tau_set_vec, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_mat(tau_typebuilder_t* builder, size_t rows, size_t cols, tau_typedesc_t* base_type)
{
  TAU_ASSERT(tau_typedesc_is_integer(base_type) || tau_typedesc_is_float(base_type));

  tau_typedesc_mat_t* desc = tau_typedesc_mat_init();
  desc->rows = rows;
  desc->cols = cols;
  desc->base_type = base_type;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_mat, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  desc->llvm_type = LLVMVectorType(base_type->llvm_type, (uint32_t)(rows * cols));

  tau_set_add(builder->tau_set_mat, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_i8(tau_typebuilder_t* builder)
{
  return builder->desc_i8;
}

tau_typedesc_t* tau_typebuilder_build_i16(tau_typebuilder_t* builder)
{
  return builder->desc_i16;
}

tau_typedesc_t* tau_typebuilder_build_i32(tau_typebuilder_t* builder)
{
  return builder->desc_i32;
}

tau_typedesc_t* tau_typebuilder_build_i64(tau_typebuilder_t* builder)
{
  return builder->desc_i64;
}

tau_typedesc_t* tau_typebuilder_build_isize(tau_typebuilder_t* builder)
{
  return builder->desc_isize;
}

tau_typedesc_t* tau_typebuilder_build_u8(tau_typebuilder_t* builder)
{
  return builder->desc_u8;
}

tau_typedesc_t* tau_typebuilder_build_u16(tau_typebuilder_t* builder)
{
  return builder->desc_u16;
}

tau_typedesc_t* tau_typebuilder_build_u32(tau_typebuilder_t* builder)
{
  return builder->desc_u32;
}

tau_typedesc_t* tau_typebuilder_build_u64(tau_typebuilder_t* builder)
{
  return builder->desc_u64;
}

tau_typedesc_t* tau_typebuilder_build_usize(tau_typebuilder_t* builder)
{
  return builder->desc_usize;
}

tau_typedesc_t* tau_typebuilder_build_integer(tau_typebuilder_t* builder, size_t bits, bool is_signed)
{
  if (bits <= 8)
    return is_signed ? builder->desc_i8 : builder->desc_u8;

  if (bits <= 16)
    return is_signed ? builder->desc_i16 : builder->desc_u16;

  if (bits <= 32)
    return is_signed ? builder->desc_i32 : builder->desc_u32;

  if (bits <= 64)
    return is_signed ? builder->desc_i64 : builder->desc_u64;

  TAU_UNREACHABLE();
  return NULL;
}

tau_typedesc_t* tau_typebuilder_build_f32(tau_typebuilder_t* builder)
{
  return builder->desc_f32;
}

tau_typedesc_t* tau_typebuilder_build_f64(tau_typebuilder_t* builder)
{
  return builder->desc_f64;
}

tau_typedesc_t* tau_typebuilder_build_c64(tau_typebuilder_t* builder)
{
  return builder->desc_c64;
}

tau_typedesc_t* tau_typebuilder_build_c128(tau_typebuilder_t* builder)
{
  return builder->desc_c128;
}

tau_typedesc_t* tau_typebuilder_build_char(tau_typebuilder_t* builder)
{
  return builder->desc_char;
}

tau_typedesc_t* tau_typebuilder_build_bool(tau_typebuilder_t* builder)
{
  return builder->desc_bool;
}

tau_typedesc_t* tau_typebuilder_build_unit(tau_typebuilder_t* builder)
{
  return builder->desc_unit;
}

tau_typedesc_t* tau_typebuilder_build_poison(tau_typebuilder_t* builder)
{
  return builder->desc_poison;
}

tau_typedesc_t* tau_typebuilder_build_fun(tau_typebuilder_t* builder, tau_typedesc_t* return_type, tau_typedesc_t* param_types[], size_t param_count, bool is_vararg, tau_callconv_kind_t callconv)
{
  tau_typedesc_fun_t* desc = tau_typedesc_fun_init();
  desc->return_type = return_type;
  desc->param_types = param_count == 0 ? NULL : tau_vector_init_from_buffer(param_types, param_count);
  desc->is_vararg = is_vararg;
  desc->callconv = callconv;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_fun, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
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

  tau_set_add(builder->tau_set_fun, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_struct(tau_typebuilder_t* builder, tau_ast_node_t* node, tau_typedesc_t* field_types[], size_t field_count)
{
  tau_typedesc_struct_t* desc = tau_typedesc_struct_init();
  desc->node = node;
  desc->field_types = field_count == 0 ? NULL : tau_vector_init_from_buffer(field_types, field_count);

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_struct, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
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

  tau_set_add(builder->tau_set_struct, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_struct_opaque(tau_typebuilder_t* builder, tau_ast_node_t* node)
{
  tau_typedesc_struct_t* desc = tau_typedesc_struct_init();
  desc->node = node;
  desc->field_types = NULL;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_struct, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  tau_ast_decl_struct_t* struct_node = (tau_ast_decl_struct_t*)node;
  tau_string_t* id_str = tau_token_to_string(struct_node->id->tok);

  desc->llvm_type = LLVMStructCreateNamed(builder->llvm_context, tau_string_begin(id_str));

  tau_string_free(id_str);

  tau_set_add(builder->tau_set_struct, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_union(tau_typebuilder_t* builder, tau_ast_node_t* node, tau_typedesc_t* field_types[], size_t field_count)
{
  tau_typedesc_union_t* desc = tau_typedesc_union_init();
  desc->node = node;
  desc->field_types = field_count == 0 ? NULL : tau_vector_init_from_buffer(field_types, field_count);

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_union, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
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

  tau_set_add(builder->tau_set_union, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_enum(tau_typebuilder_t* builder, tau_ast_node_t* node)
{
  tau_typedesc_enum_t* desc = tau_typedesc_enum_init();
  desc->node = node;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_enum, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  size_t field_count = tau_vector_size(((tau_ast_decl_enum_t*)node)->members);

       if (field_count <=  UINT8_MAX) desc->llvm_type = builder->desc_u8->llvm_type;
  else if (field_count <= UINT16_MAX) desc->llvm_type = builder->desc_u16->llvm_type;
  else if (field_count <= UINT32_MAX) desc->llvm_type = builder->desc_u32->llvm_type;
  else if (field_count <= UINT64_MAX) desc->llvm_type = builder->desc_u64->llvm_type;
  else TAU_UNREACHABLE();

  tau_set_add(builder->tau_set_enum, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_build_var(tau_typebuilder_t* builder, uint64_t id)
{
  tau_typedesc_var_t* desc = tau_typedesc_var_init();
  desc->id = id;

  tau_typedesc_t* result = (tau_typedesc_t*)tau_set_get(builder->tau_set_var, desc);

  if (result != NULL)
  {
    tau_typedesc_free((tau_typedesc_t*)desc);
    return result;
  }

  tau_set_add(builder->tau_set_var, desc);

  return (tau_typedesc_t*)desc;
}

tau_typedesc_t* tau_typebuilder_struct_set_body(tau_typebuilder_t* builder, tau_typedesc_t* desc, tau_typedesc_t* field_types[], size_t field_count)
{
  TAU_ASSERT(tau_set_contains(builder->tau_set_struct, desc));

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

  return (tau_typedesc_t*)desc;
}

static tau_typedesc_t* tau_typebuilder_build_promoted_arithmetic_from_integer_and_integer(tau_typebuilder_t* builder, tau_typedesc_t* int_desc1, tau_typedesc_t* int_desc2)
{
  if (tau_typedesc_is_signed(int_desc1) == tau_typedesc_is_signed(int_desc2))
    return tau_typedesc_integer_bits(int_desc1) >= tau_typedesc_integer_bits(int_desc2) ? int_desc1 : int_desc2;

  size_t max_bits = TAU_MAX(tau_typedesc_integer_bits(int_desc1), tau_typedesc_integer_bits(int_desc2));

  if (max_bits < 64)
    max_bits += 1;

  return tau_typebuilder_build_integer(builder, max_bits, true);
}

static tau_typedesc_t* tau_typebuilder_build_promoted_arithmetic_from_integer_and_float(tau_typebuilder_t* builder, tau_typedesc_t* int_desc, tau_typedesc_t* float_desc)
{
  return tau_typedesc_integer_bits(int_desc) <= 16 ? float_desc : tau_typebuilder_build_f64(builder);
}

static tau_typedesc_t* tau_typebuilder_build_promoted_arithmetic_from_integer_and_complex(tau_typebuilder_t* builder, tau_typedesc_t* int_desc, tau_typedesc_t* complex_desc)
{
  return tau_typedesc_integer_bits(int_desc) <= 16 ? complex_desc : tau_typebuilder_build_c128(builder);
}

static tau_typedesc_t* tau_typebuilder_build_promoted_arithmetic_from_float_and_float(tau_typebuilder_t* TAU_UNUSED(builder), tau_typedesc_t* float_desc1, tau_typedesc_t* float_desc2)
{
  return float_desc1->kind == TAU_TYPEDESC_F64 ? float_desc1 : float_desc2;
}

static tau_typedesc_t* tau_typebuilder_build_promoted_arithmetic_from_float_and_complex(tau_typebuilder_t* builder, tau_typedesc_t* float_desc, tau_typedesc_t* complex_desc)
{
  return float_desc->kind == TAU_TYPEDESC_F32 ? complex_desc : tau_typebuilder_build_c128(builder);
}

static tau_typedesc_t* tau_typebuilder_build_promoted_arithmetic_from_complex_and_complex(tau_typebuilder_t* TAU_UNUSED(builder), tau_typedesc_t* complex_desc1, tau_typedesc_t* complex_desc2)
{
  return complex_desc1->kind == TAU_TYPEDESC_C128 ? complex_desc1 : complex_desc2;
}

tau_typedesc_t* tau_typebuilder_build_promoted_arithmetic(tau_typebuilder_t* builder, tau_typedesc_t* lhs_desc, tau_typedesc_t* rhs_desc)
{
  TAU_ASSERT(tau_typedesc_is_arithmetic(lhs_desc));
  TAU_ASSERT(tau_typedesc_is_arithmetic(rhs_desc));

  if (lhs_desc == rhs_desc)
    return lhs_desc;

  if (tau_typedesc_is_integer(lhs_desc) && tau_typedesc_is_integer(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_integer_and_integer(builder, lhs_desc, rhs_desc);

  if (tau_typedesc_is_integer(lhs_desc) && tau_typedesc_is_float(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_integer_and_float(builder, lhs_desc, rhs_desc);

  if (tau_typedesc_is_float(lhs_desc) && tau_typedesc_is_integer(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_integer_and_float(builder, rhs_desc, lhs_desc);

  if (tau_typedesc_is_integer(lhs_desc) && tau_typedesc_is_complex(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_integer_and_complex(builder, lhs_desc, rhs_desc);

  if (tau_typedesc_is_complex(lhs_desc) && tau_typedesc_is_integer(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_integer_and_complex(builder, rhs_desc, lhs_desc);

  if (tau_typedesc_is_float(lhs_desc) && tau_typedesc_is_float(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_float_and_float(builder, lhs_desc, rhs_desc);

  if (tau_typedesc_is_float(lhs_desc) && tau_typedesc_is_complex(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_float_and_complex(builder, lhs_desc, rhs_desc);

  if (tau_typedesc_is_complex(lhs_desc) && tau_typedesc_is_float(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_float_and_complex(builder, rhs_desc, lhs_desc);

  if (tau_typedesc_is_complex(lhs_desc) && tau_typedesc_is_complex(rhs_desc))
    return tau_typebuilder_build_promoted_arithmetic_from_complex_and_complex(builder, lhs_desc, rhs_desc);

  TAU_UNREACHABLE();
  return NULL;
}
