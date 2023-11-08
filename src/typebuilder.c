#include "typebuilder.h"

#include <string.h>

#include "memtrace.h"
#include "typeset.h"

struct typebuilder_t
{
  typeset_t* typeset;
};

typebuilder_t* typebuilder_init(void)
{
  typebuilder_t* builder = (typebuilder_t*)malloc(sizeof(typebuilder_t));
  memset(builder, 0, sizeof(typebuilder_t));

  builder->typeset = typeset_init();

  return builder;
}

void typebuilder_free(typebuilder_t* builder)
{
  typeset_free(builder->typeset);
  free(builder);
}

typedesc_t* typebuilder_build_mut(typebuilder_t* builder, typedesc_t* base_type)
{
  return typeset_lookup_mut(builder->typeset, base_type);
}

typedesc_t* typebuilder_build_const(typebuilder_t* builder, typedesc_t* base_type)
{
  return typeset_lookup_const(builder->typeset, base_type);
}

typedesc_t* typebuilder_build_ptr(typebuilder_t* builder, typedesc_t* base_type)
{
  return typeset_lookup_ptr(builder->typeset, base_type);
}

typedesc_t* typebuilder_build_array(typebuilder_t* builder, size_t length, typedesc_t* base_type)
{
  return typeset_lookup_array(builder->typeset, length, base_type);
}

typedesc_t* typebuilder_build_ref(typebuilder_t* builder, typedesc_t* base_type)
{
  return typeset_lookup_ref(builder->typeset, base_type);
}

typedesc_t* typebuilder_build_opt(typebuilder_t* builder, typedesc_t* base_type)
{
  return typeset_lookup_opt(builder->typeset, base_type);
}

typedesc_t* typebuilder_build_i8(typebuilder_t* builder)
{
  return typeset_lookup_i8(builder->typeset);
}

typedesc_t* typebuilder_build_i16(typebuilder_t* builder)
{
  return typeset_lookup_i16(builder->typeset);
}

typedesc_t* typebuilder_build_i32(typebuilder_t* builder)
{
  return typeset_lookup_i32(builder->typeset);
}

typedesc_t* typebuilder_build_i64(typebuilder_t* builder)
{
  return typeset_lookup_i64(builder->typeset);
}

typedesc_t* typebuilder_build_isize(typebuilder_t* builder)
{
  return typeset_lookup_isize(builder->typeset);
}

typedesc_t* typebuilder_build_u8(typebuilder_t* builder)
{
  return typeset_lookup_u8(builder->typeset);
}

typedesc_t* typebuilder_build_u16(typebuilder_t* builder)
{
  return typeset_lookup_u16(builder->typeset);
}

typedesc_t* typebuilder_build_u32(typebuilder_t* builder)
{
  return typeset_lookup_u32(builder->typeset);
}

typedesc_t* typebuilder_build_u64(typebuilder_t* builder)
{
  return typeset_lookup_u64(builder->typeset);
}

typedesc_t* typebuilder_build_usize(typebuilder_t* builder)
{
  return typeset_lookup_usize(builder->typeset);
}

typedesc_t* typebuilder_build_f32(typebuilder_t* builder)
{
  return typeset_lookup_f32(builder->typeset);
}

typedesc_t* typebuilder_build_f64(typebuilder_t* builder)
{
  return typeset_lookup_f64(builder->typeset);
}

typedesc_t* typebuilder_build_bool(typebuilder_t* builder)
{
  return typeset_lookup_bool(builder->typeset);
}

typedesc_t* typebuilder_build_unit(typebuilder_t* builder)
{
  return typeset_lookup_unit(builder->typeset);
}

typedesc_t* typebuilder_build_fun(typebuilder_t* builder, typedesc_t* return_type, typedesc_t* param_types[], size_t param_count, bool is_vararg, abi_kind_t abi)
{
  return typeset_lookup_fun(builder->typeset, return_type, param_types, param_count, is_vararg, abi);
}

typedesc_t* typebuilder_build_gen(typebuilder_t* builder, typedesc_t* yield_type, typedesc_t* param_types[], size_t param_count)
{
  return typeset_lookup_gen(builder->typeset, yield_type, param_types, param_count);
}

typedesc_t* typebuilder_build_struct(typebuilder_t* builder, ast_node_t* node, typedesc_t* field_types[], size_t field_count)
{
  return typeset_lookup_struct(builder->typeset, node, field_types, field_count);
}

typedesc_t* typebuilder_build_union(typebuilder_t* builder, ast_node_t* node, typedesc_t* field_types[], size_t field_count)
{
  return typeset_lookup_union(builder->typeset, node, field_types, field_count);
}

typedesc_t* typebuilder_build_enum(typebuilder_t* builder, ast_node_t* node)
{
  return typeset_lookup_enum(builder->typeset, node);
}
