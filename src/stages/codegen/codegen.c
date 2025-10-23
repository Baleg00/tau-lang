/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/codegen/codegen.h"

#include "ast/ast.h"

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_integer_to_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_integer_bits(src_desc) < tau_typedesc_integer_bits(dst_desc))
  {
    if (tau_typedesc_is_signed(dst_desc))
      return LLVMBuildSExtOrBitCast(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

    return LLVMBuildZExtOrBitCast(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
  }

  if (tau_typedesc_integer_bits(src_desc) > tau_typedesc_integer_bits(dst_desc))
    return LLVMBuildTrunc(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildBitCast(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}


static LLVMValueRef tau_codegen_build_arithmetic_cast_from_integer_to_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_signed(src_desc))
    return LLVMBuildSIToFP(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildUIToFP(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_integer_to_complex(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  LLVMTypeRef llvm_float_type = dst_desc->kind == TAU_TYPEDESC_C64 ?
    LLVMFloatTypeInContext(ctx->llvm_ctx) :
    LLVMDoubleTypeInContext(ctx->llvm_ctx);

  LLVMValueRef llvm_value_real = tau_typedesc_is_signed(src_desc) ?
    LLVMBuildSIToFP(ctx->llvm_builder, llvm_value, llvm_float_type, "") :
    LLVMBuildUIToFP(ctx->llvm_builder, llvm_value, llvm_float_type, "");

  LLVMValueRef llvm_value_imag = LLVMConstReal(llvm_float_type, 0.0);

  return tau_codegen_build_complex(ctx, llvm_value_real, llvm_value_imag);
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_integer(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_integer_to_integer(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_float(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_integer_to_float(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_complex(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_integer_to_complex(ctx, llvm_value, src_desc, dst_desc);

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_float_to_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (src_desc->kind == TAU_TYPEDESC_F32 && dst_desc->kind == TAU_TYPEDESC_F64)
    return LLVMBuildFPExt(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  if (src_desc->kind == TAU_TYPEDESC_F64 && dst_desc->kind == TAU_TYPEDESC_F32)
    return LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return llvm_value;
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_float_to_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* TAU_UNUSED(src_desc), tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_signed(dst_desc))
    return LLVMBuildFPToSI(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildFPToUI(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_float_to_complex(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  LLVMValueRef llvm_value_imag = LLVMConstReal(src_desc->llvm_type, 0.0);

  if (src_desc->kind == TAU_TYPEDESC_F32 && dst_desc->kind == TAU_TYPEDESC_C64)
    return tau_codegen_build_complex(ctx, llvm_value, llvm_value_imag);

  if (src_desc->kind == TAU_TYPEDESC_F32 && dst_desc->kind == TAU_TYPEDESC_C128)
  {
    LLVMTypeRef llvm_float_type = LLVMDoubleTypeInContext(ctx->llvm_ctx);
    LLVMValueRef llvm_value_real = LLVMBuildFPExt(ctx->llvm_builder, llvm_value, llvm_float_type, "");

    return tau_codegen_build_complex(ctx, llvm_value_real, llvm_value_imag);
  }

  if (src_desc->kind == TAU_TYPEDESC_F64 && dst_desc->kind == TAU_TYPEDESC_C64)
  {
    LLVMTypeRef llvm_float_type = LLVMFloatTypeInContext(ctx->llvm_ctx);
    LLVMValueRef llvm_value_real = LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value, llvm_float_type, "");

    return tau_codegen_build_complex(ctx, llvm_value_real, llvm_value_imag);
  }

  if (src_desc->kind == TAU_TYPEDESC_F64 && dst_desc->kind == TAU_TYPEDESC_C128)
    return tau_codegen_build_complex(ctx, llvm_value, llvm_value_imag);

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_float(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_float_to_float(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_integer(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_float_to_integer(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_complex(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_float_to_complex(ctx, llvm_value, src_desc, dst_desc);

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_complex_to_complex(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  LLVMValueRef llvm_value_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 0, "");
  LLVMValueRef llvm_value_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 1, "");

  if (src_desc->kind == TAU_TYPEDESC_C64 && dst_desc->kind == TAU_TYPEDESC_C128)
  {
    LLVMTypeRef llvm_float_type = LLVMDoubleTypeInContext(ctx->llvm_ctx);

    LLVMValueRef llvm_result_real = LLVMBuildFPExt(ctx->llvm_builder, llvm_value_real, llvm_float_type, "");
    LLVMValueRef llvm_result_imag = LLVMBuildFPExt(ctx->llvm_builder, llvm_value_imag, llvm_float_type, "");

    return tau_codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
  }

  if (src_desc->kind == TAU_TYPEDESC_C128 && dst_desc->kind == TAU_TYPEDESC_C64)
  {
    LLVMTypeRef llvm_float_type = LLVMFloatTypeInContext(ctx->llvm_ctx);

    LLVMValueRef llvm_result_real = LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value_real, llvm_float_type, "");
    LLVMValueRef llvm_result_imag = LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value_imag, llvm_float_type, "");

    return tau_codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
  }

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_complex_to_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  LLVMValueRef llvm_value_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 0, "");

  if (src_desc->kind == TAU_TYPEDESC_C64 && dst_desc->kind == TAU_TYPEDESC_F64)
    return LLVMBuildFPExt(ctx->llvm_builder, llvm_value_real, dst_desc->llvm_type, "");

  if (src_desc->kind == TAU_TYPEDESC_C128 && dst_desc->kind == TAU_TYPEDESC_F32)
    return LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value_real, dst_desc->llvm_type, "");

  return llvm_value_real;
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_complex_to_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* TAU_UNUSED(src_desc), tau_typedesc_t* dst_desc)
{
  LLVMValueRef llvm_value_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 0, "");

  if (tau_typedesc_is_signed(dst_desc))
    return LLVMBuildFPToSI(ctx->llvm_builder, llvm_value_real, dst_desc->llvm_type, "");

  return LLVMBuildFPToUI(ctx->llvm_builder, llvm_value_real, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_arithmetic_cast_from_complex(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_integer(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_complex_to_integer(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_float(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_complex_to_float(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_complex(dst_desc))
    return tau_codegen_build_arithmetic_cast_from_complex_to_complex(ctx, llvm_value, src_desc, dst_desc);

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_vector_cast_from_integer_to_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_vec_t* src_desc, tau_typedesc_vec_t* dst_desc)
{
  if (tau_typedesc_integer_bits(src_desc->base_type) < tau_typedesc_integer_bits(dst_desc->base_type))
  {
    if (tau_typedesc_is_signed(dst_desc->base_type))
      return LLVMBuildSExt(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

    return LLVMBuildZExt(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
  }

  if (tau_typedesc_integer_bits(src_desc->base_type) > tau_typedesc_integer_bits(dst_desc->base_type))
    return LLVMBuildTrunc(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildBitCast(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_vector_cast_from_integer_to_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_vec_t* src_desc, tau_typedesc_vec_t* dst_desc)
{
  if (tau_typedesc_is_signed(src_desc->base_type))
    return LLVMBuildSIToFP(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildUIToFP(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_vector_cast_from_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_vec_t* src_desc, tau_typedesc_vec_t* dst_desc)
{
  if (tau_typedesc_is_integer(dst_desc->base_type))
    return tau_codegen_build_vector_cast_from_integer_to_integer(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_float(dst_desc->base_type))
    return tau_codegen_build_vector_cast_from_integer_to_float(ctx, llvm_value, src_desc, dst_desc);

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_vector_cast_from_float_to_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_vec_t* TAU_UNUSED(src_desc), tau_typedesc_vec_t* dst_desc)
{
  if (tau_typedesc_is_signed(dst_desc->base_type))
    return LLVMBuildFPToSI(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildFPToUI(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_vector_cast_from_float_to_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_vec_t* src_desc, tau_typedesc_vec_t* dst_desc)
{
  if (src_desc->base_type->kind == TAU_TYPEDESC_F32 && dst_desc->base_type->kind == TAU_TYPEDESC_F64)
    return LLVMBuildFPExt(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  if (src_desc->base_type->kind == TAU_TYPEDESC_F64 && dst_desc->base_type->kind == TAU_TYPEDESC_F32)
    return LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return llvm_value;
}

static LLVMValueRef tau_codegen_build_vector_cast_from_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_vec_t* src_desc, tau_typedesc_vec_t* dst_desc)
{
  if (tau_typedesc_is_integer(dst_desc->base_type))
    return tau_codegen_build_vector_cast_from_float_to_integer(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_float(dst_desc->base_type))
    return tau_codegen_build_vector_cast_from_float_to_float(ctx, llvm_value, src_desc, dst_desc);

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_matrix_cast_from_integer_to_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_mat_t* src_desc, tau_typedesc_mat_t* dst_desc)
{
  if (tau_typedesc_integer_bits(src_desc->base_type) < tau_typedesc_integer_bits(dst_desc->base_type))
  {
    if (tau_typedesc_is_signed(dst_desc->base_type))
      return LLVMBuildSExt(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

    return LLVMBuildZExt(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
  }

  if (tau_typedesc_integer_bits(src_desc->base_type) > tau_typedesc_integer_bits(dst_desc->base_type))
    return LLVMBuildTrunc(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildBitCast(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_matrix_cast_from_integer_to_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_mat_t* src_desc, tau_typedesc_mat_t* dst_desc)
{
  if (tau_typedesc_is_signed(src_desc->base_type))
    return LLVMBuildSIToFP(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildUIToFP(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_matrix_cast_from_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_mat_t* src_desc, tau_typedesc_mat_t* dst_desc)
{
  if (tau_typedesc_is_integer(dst_desc->base_type))
    return tau_codegen_build_matrix_cast_from_integer_to_integer(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_float(dst_desc->base_type))
    return tau_codegen_build_matrix_cast_from_integer_to_float(ctx, llvm_value, src_desc, dst_desc);

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_matrix_cast_from_float_to_integer(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_mat_t* TAU_UNUSED(src_desc), tau_typedesc_mat_t* dst_desc)
{
  if (tau_typedesc_is_signed(dst_desc->base_type))
    return LLVMBuildFPToSI(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return LLVMBuildFPToUI(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");
}

static LLVMValueRef tau_codegen_build_matrix_cast_from_float_to_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_mat_t* src_desc, tau_typedesc_mat_t* dst_desc)
{
  if (src_desc->base_type->kind == TAU_TYPEDESC_F32 && dst_desc->base_type->kind == TAU_TYPEDESC_F64)
    return LLVMBuildFPExt(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  if (src_desc->base_type->kind == TAU_TYPEDESC_F64 && dst_desc->base_type->kind == TAU_TYPEDESC_F32)
    return LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value, dst_desc->llvm_type, "");

  return llvm_value;
}

static LLVMValueRef tau_codegen_build_matrix_cast_from_float(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_mat_t* src_desc, tau_typedesc_mat_t* dst_desc)
{
  if (tau_typedesc_is_integer(dst_desc->base_type))
    return tau_codegen_build_matrix_cast_from_float_to_integer(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_float(dst_desc->base_type))
    return tau_codegen_build_matrix_cast_from_float_to_float(ctx, llvm_value, src_desc, dst_desc);

  TAU_UNREACHABLE();

  return NULL;
}

static LLVMValueRef tau_codegen_build_implicit_cast_mut(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_mut_t* src_desc, tau_typedesc_t* dst_desc)
{
  return tau_codegen_build_implicit_cast(ctx, llvm_value, src_desc->base_type, tau_typedesc_remove_mut(dst_desc));
}

static LLVMValueRef tau_codegen_build_implicit_cast_ptr(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_ptr_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_codegen_build_opt_wrap(ctx, llvm_value, (tau_typedesc_t*)src_desc, dst_desc);

  return llvm_value;
}

static LLVMValueRef tau_codegen_build_implicit_cast_array(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_array_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_codegen_build_opt_wrap(ctx, llvm_value, (tau_typedesc_t*)src_desc, dst_desc);

  // TODO: Cast array elements.
  return llvm_value;
}

static LLVMValueRef tau_codegen_build_implicit_cast_ref(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_ref_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (!tau_typedesc_is_ref(dst_desc))
  {
    LLVMValueRef llvm_load_value = LLVMBuildLoad2(ctx->llvm_builder, src_desc->base_type->llvm_type, llvm_value, "");

    return tau_codegen_build_implicit_cast(ctx, llvm_load_value, src_desc->base_type, dst_desc);
  }

  return llvm_value;
}

static LLVMValueRef tau_codegen_build_implicit_cast_opt(tau_codegen_ctx_t* TAU_UNUSED(ctx), LLVMValueRef llvm_value, tau_typedesc_opt_t* TAU_UNUSED(src_desc), tau_typedesc_t* TAU_UNUSED(dst_desc))
{
  return llvm_value;
}

static LLVMValueRef tau_codegen_build_implicit_cast_vec(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_vec_t* src_desc, tau_typedesc_t* dst_desc)
{
  return tau_codegen_build_vector_cast(ctx, llvm_value, (tau_typedesc_t*)src_desc, dst_desc);
}

static LLVMValueRef tau_codegen_build_implicit_cast_mat(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_mat_t* src_desc, tau_typedesc_t* dst_desc)
{
  return tau_codegen_build_matrix_cast(ctx, llvm_value, (tau_typedesc_t*)src_desc, dst_desc);
}

static LLVMValueRef tau_codegen_build_implicit_cast_prim(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_prim_t* src_desc, tau_typedesc_t* dst_desc)
{
  if (tau_typedesc_is_opt(dst_desc))
    return tau_codegen_build_opt_wrap(ctx, llvm_value, (tau_typedesc_t*)src_desc, dst_desc);

  if (tau_typedesc_is_arithmetic((tau_typedesc_t*)src_desc))
    return tau_codegen_build_arithmetic_cast(ctx, llvm_value, (tau_typedesc_t*)src_desc, tau_typedesc_remove_mut(dst_desc));

  if (src_desc->kind == TAU_TYPEDESC_BOOL && tau_typedesc_remove_mut(dst_desc)->kind == TAU_TYPEDESC_BOOL)
    return llvm_value;

  TAU_UNREACHABLE();

  return NULL;
}

tau_codegen_ctx_t* tau_codegen_ctx_init(tau_typebuilder_t* typebuilder, tau_typetable_t* typetable, LLVMContextRef llvm_ctx, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_mod, LLVMBuilderRef llvm_builder)
{
  tau_codegen_ctx_t* ctx = (tau_codegen_ctx_t*)malloc(sizeof(tau_codegen_ctx_t));
  TAU_CLEAROBJ(ctx);

  ctx->typebuilder = typebuilder;
  ctx->typetable = typetable;
  ctx->llvm_ctx = llvm_ctx;
  ctx->llvm_layout = llvm_layout;
  ctx->llvm_mod = llvm_mod;
  ctx->llvm_builder = llvm_builder;

  return ctx;
}

void tau_codegen_ctx_free(tau_codegen_ctx_t* ctx)
{
  free(ctx);
}

LLVMValueRef tau_codegen_build_load_if_ref(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* desc)
{
  if (tau_typedesc_is_ref(desc))
    return LLVMBuildLoad2(ctx->llvm_builder, ((tau_typedesc_ref_t*)desc)->base_type->llvm_type, llvm_value, "");

  return llvm_value;
}

LLVMValueRef tau_codegen_build_arithmetic_cast(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  TAU_ASSERT(tau_typedesc_is_arithmetic(src_desc) && tau_typedesc_is_arithmetic(dst_desc));

  if (src_desc == dst_desc)
    return llvm_value;

  if (tau_typedesc_is_integer(src_desc))
    return tau_codegen_build_arithmetic_cast_from_integer(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_float(src_desc))
    return tau_codegen_build_arithmetic_cast_from_float(ctx, llvm_value, src_desc, dst_desc);

  if (tau_typedesc_is_complex(src_desc))
    return tau_codegen_build_arithmetic_cast_from_complex(ctx, llvm_value, src_desc, dst_desc);

  TAU_UNREACHABLE();

  return NULL;
}

LLVMValueRef tau_codegen_build_vector_cast(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  TAU_ASSERT(tau_typedesc_is_vector(src_desc) && tau_typedesc_is_vector(dst_desc));

  tau_typedesc_vec_t* src_vec_desc = (tau_typedesc_vec_t*)src_desc;
  tau_typedesc_vec_t* dst_vec_desc = (tau_typedesc_vec_t*)dst_desc;

  TAU_ASSERT(src_vec_desc->size == dst_vec_desc->size);

  if (tau_typedesc_is_integer(src_vec_desc->base_type))
    return tau_codegen_build_vector_cast_from_integer(ctx, llvm_value, src_vec_desc, dst_vec_desc);

  if (tau_typedesc_is_float(src_vec_desc->base_type))
    return tau_codegen_build_vector_cast_from_float(ctx, llvm_value, src_vec_desc, dst_vec_desc);

  TAU_UNREACHABLE();

  return NULL;
}

LLVMValueRef tau_codegen_build_matrix_cast(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  TAU_ASSERT(tau_typedesc_is_matrix(src_desc) && tau_typedesc_is_matrix(dst_desc));

  tau_typedesc_mat_t* src_mat_desc = (tau_typedesc_mat_t*)src_desc;
  tau_typedesc_mat_t* dst_mat_desc = (tau_typedesc_mat_t*)dst_desc;

  TAU_ASSERT(src_mat_desc->rows == dst_mat_desc->rows);
  TAU_ASSERT(src_mat_desc->cols == dst_mat_desc->cols);

  if (tau_typedesc_is_integer(src_mat_desc->base_type))
    return tau_codegen_build_matrix_cast_from_integer(ctx, llvm_value, src_mat_desc, dst_mat_desc);

  if (tau_typedesc_is_float(src_mat_desc->base_type))
    return tau_codegen_build_matrix_cast_from_float(ctx, llvm_value, src_mat_desc, dst_mat_desc);

  TAU_UNREACHABLE();

  return NULL;
}

LLVMValueRef tau_codegen_build_opt_wrap(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  LLVMTypeRef llvm_bool_type = LLVMInt1TypeInContext(ctx->llvm_ctx);

  LLVMValueRef llvm_value_flag = LLVMConstInt(llvm_bool_type, 1, false);
  LLVMValueRef llvm_value_wrapped = tau_codegen_build_implicit_cast(ctx, llvm_value, src_desc, ((tau_typedesc_opt_t*)dst_desc)->base_type);

  LLVMValueRef llvm_tmp1 = LLVMGetUndef(dst_desc->llvm_type);

  LLVMValueRef llvm_tmp2 = LLVMBuildInsertValue(ctx->llvm_builder, llvm_tmp1, llvm_value_flag, 0, "");
  LLVMValueRef llvm_tmp3 = LLVMBuildInsertValue(ctx->llvm_builder, llvm_tmp2, llvm_value_wrapped, 1, "");

  return llvm_tmp3;
}

LLVMValueRef tau_codegen_build_opt_unwrap_unchecked(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* desc)
{
  return LLVMBuildStructGEP2(ctx->llvm_builder, desc->llvm_type, llvm_value, 1, "");
}

LLVMValueRef tau_codegen_build_implicit_cast(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value, tau_typedesc_t* src_desc, tau_typedesc_t* dst_desc)
{
  TAU_ASSERT(tau_typedesc_is_implicitly_direct_convertible(src_desc, dst_desc));

  switch (src_desc->kind)
  {
  case TAU_TYPEDESC_MUT:    return tau_codegen_build_implicit_cast_mut   (ctx, llvm_value, (tau_typedesc_mut_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_PTR:    return tau_codegen_build_implicit_cast_ptr   (ctx, llvm_value, (tau_typedesc_ptr_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_ARRAY:  return tau_codegen_build_implicit_cast_array (ctx, llvm_value, (tau_typedesc_array_t* )src_desc, dst_desc);
  case TAU_TYPEDESC_REF:    return tau_codegen_build_implicit_cast_ref   (ctx, llvm_value, (tau_typedesc_ref_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_OPT:    return tau_codegen_build_implicit_cast_opt   (ctx, llvm_value, (tau_typedesc_opt_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_VEC:    return tau_codegen_build_implicit_cast_vec   (ctx, llvm_value, (tau_typedesc_vec_t*   )src_desc, dst_desc);
  case TAU_TYPEDESC_MAT:    return tau_codegen_build_implicit_cast_mat   (ctx, llvm_value, (tau_typedesc_mat_t*   )src_desc, dst_desc);
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
  case TAU_TYPEDESC_UNIT:   return tau_codegen_build_implicit_cast_prim  (ctx, llvm_value, (tau_typedesc_prim_t*  )src_desc, dst_desc);
  case TAU_TYPEDESC_FUN:    return llvm_value; // TODO: add implicit cast
  case TAU_TYPEDESC_STRUCT: return llvm_value; // TODO: add implicit cast
  case TAU_TYPEDESC_UNION:  return llvm_value; // TODO: add implicit cast
  case TAU_TYPEDESC_ENUM:   return llvm_value; // TODO: add implicit cast
  default: TAU_UNREACHABLE();
  }

  return NULL;
}

LLVMValueRef tau_codegen_build_complex(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value_real, LLVMValueRef llvm_value_imag)
{
  LLVMTypeRef llvm_value_real_type = LLVMTypeOf(llvm_value_real);
  LLVMTypeRef llvm_value_imag_type = LLVMTypeOf(llvm_value_imag);

  TAU_ASSERT(llvm_value_real_type == llvm_value_imag_type);

  LLVMTypeRef llvm_float_type = LLVMFloatTypeInContext(ctx->llvm_ctx);
  LLVMTypeRef llvm_double_type = LLVMDoubleTypeInContext(ctx->llvm_ctx);

  tau_typedesc_t* c_desc = NULL;

  if (llvm_value_real_type == llvm_float_type)
    c_desc = tau_typebuilder_build_c64(ctx->typebuilder);
  else if (llvm_value_real_type == llvm_double_type)
    c_desc = tau_typebuilder_build_c128(ctx->typebuilder);
  else
    TAU_UNREACHABLE();

  LLVMValueRef llvm_tmp1 = LLVMGetUndef(c_desc->llvm_type);

  LLVMValueRef llvm_tmp2 = LLVMBuildInsertValue(ctx->llvm_builder, llvm_tmp1, llvm_value_real, 0, "");
  LLVMValueRef llvm_tmp3 = LLVMBuildInsertValue(ctx->llvm_builder, llvm_tmp2, llvm_value_imag, 1, "");

  return llvm_tmp3;
}

LLVMValueRef tau_codegen_build_complex_neg(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_value)
{
  // llvm_value => a + bi
  // -llvm_value => (-a) + (-b)i

  LLVMValueRef llvm_value_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 0, ""); // a
  LLVMValueRef llvm_value_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 1, ""); // b

  LLVMValueRef llvm_result_real = LLVMBuildFNeg(ctx->llvm_builder, llvm_value_real, ""); // -a
  LLVMValueRef llvm_result_imag = LLVMBuildFNeg(ctx->llvm_builder, llvm_value_imag, ""); // -b

  return tau_codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef tau_codegen_build_complex_add(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  // llvm_lhs => a + bi
  // llvm_rhs => c + di
  // llvm_lhs + llvm_rhs => (a + c) + (b + d)i

  LLVMValueRef llvm_lhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 0, ""); // a
  LLVMValueRef llvm_lhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 1, ""); // b

  LLVMValueRef llvm_rhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 0, ""); // c
  LLVMValueRef llvm_rhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 1, ""); // d

  LLVMValueRef llvm_result_real = LLVMBuildFAdd(ctx->llvm_builder, llvm_lhs_real, llvm_rhs_real, ""); // a + c
  LLVMValueRef llvm_result_imag = LLVMBuildFAdd(ctx->llvm_builder, llvm_lhs_imag, llvm_rhs_imag, ""); // b + d

  return tau_codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef tau_codegen_build_complex_sub(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  // llvm_lhs => a + bi
  // llvm_rhs => c + di
  // llvm_lhs - llvm_rhs => (a - c) + (b - d)i

  LLVMValueRef llvm_lhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 0, ""); // a
  LLVMValueRef llvm_lhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 1, ""); // b

  LLVMValueRef llvm_rhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 0, ""); // c
  LLVMValueRef llvm_rhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 1, ""); // d

  LLVMValueRef llvm_result_real = LLVMBuildFSub(ctx->llvm_builder, llvm_lhs_real, llvm_rhs_real, ""); // a - c
  LLVMValueRef llvm_result_imag = LLVMBuildFSub(ctx->llvm_builder, llvm_lhs_imag, llvm_rhs_imag, ""); // b - d

  return tau_codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef tau_codegen_build_complex_mul(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  // llvm_lhs => a + bi
  // llvm_rhs => c + di
  // llvm_lhs * llvm_rhs => (a * c - b * d) + (a * d + b * c)i

  LLVMValueRef llvm_lhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 0, ""); // a
  LLVMValueRef llvm_lhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 1, ""); // b

  LLVMValueRef llvm_rhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 0, ""); // c
  LLVMValueRef llvm_rhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 1, ""); // d

  LLVMValueRef llvm_tmp1 = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_real, llvm_rhs_real, ""); // a * c
  LLVMValueRef llvm_tmp2 = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_imag, llvm_rhs_imag, ""); // b * d
  LLVMValueRef llvm_result_real = LLVMBuildFSub(ctx->llvm_builder, llvm_tmp1, llvm_tmp2, ""); // a * c - b * d

  LLVMValueRef llvm_tmp3 = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_real, llvm_rhs_imag, ""); // a * d
  LLVMValueRef llvm_tmp4 = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_imag, llvm_rhs_real, ""); // b * c
  LLVMValueRef llvm_result_imag = LLVMBuildFAdd(ctx->llvm_builder, llvm_tmp3, llvm_tmp4, ""); // a * d + b * d

  return tau_codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef tau_codegen_build_complex_div(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  // llvm_lhs => a + bi
  // llvm_rhs => c + di
  // llvm_lhs / llvm_rhs => (a * c + b * d) / (c^2 + d^2) + ((b * c - a * d) / (c^2 + d^2))i

  LLVMValueRef llvm_lhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 0, ""); // a
  LLVMValueRef llvm_lhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 1, ""); // b

  LLVMValueRef llvm_rhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 0, ""); // c
  LLVMValueRef llvm_rhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 1, ""); // d

  LLVMValueRef llvm_tmp1 = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_real, llvm_rhs_real, ""); // a * c
  LLVMValueRef llvm_tmp2 = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_imag, llvm_rhs_imag, ""); // b * d
  LLVMValueRef llvm_tmp3 = LLVMBuildFAdd(ctx->llvm_builder, llvm_tmp1, llvm_tmp2, ""); // a * c + b * d

  LLVMValueRef llvm_tmp4 = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_imag, llvm_rhs_real, ""); // b * c
  LLVMValueRef llvm_tmp5 = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_real, llvm_rhs_imag, ""); // a * d
  LLVMValueRef llvm_tmp6 = LLVMBuildFSub(ctx->llvm_builder, llvm_tmp4, llvm_tmp5, ""); // b * c - a * d

  LLVMValueRef llvm_tmp7 = LLVMBuildFMul(ctx->llvm_builder, llvm_rhs_real, llvm_rhs_real, ""); // c^2
  LLVMValueRef llvm_tmp8 = LLVMBuildFMul(ctx->llvm_builder, llvm_rhs_imag, llvm_rhs_imag, ""); // d^2
  LLVMValueRef llvm_tmp9 = LLVMBuildFAdd(ctx->llvm_builder, llvm_tmp7, llvm_tmp8, ""); // c^2 + d^2

  LLVMValueRef llvm_result_real = LLVMBuildFDiv(ctx->llvm_builder, llvm_tmp3, llvm_tmp9, ""); // (a * c + b * d) / (c^2 + d^2)
  LLVMValueRef llvm_result_imag = LLVMBuildFDiv(ctx->llvm_builder, llvm_tmp6, llvm_tmp9, ""); // (b * c - a * d) / (c^2 + d^2)

  return tau_codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef tau_codegen_build_complex_eq(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  // llvm_lhs => a + bi
  // llvm_rhs => c + di
  // llvm_lhs == llvm_rhs => (a == c) && (b == d)

  LLVMValueRef llvm_lhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 0, ""); // a
  LLVMValueRef llvm_lhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 1, ""); // b

  LLVMValueRef llvm_rhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 0, ""); // c
  LLVMValueRef llvm_rhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 1, ""); // d

  LLVMValueRef llvm_tmp1 = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOEQ, llvm_lhs_real, llvm_rhs_real, ""); // a == c
  LLVMValueRef llvm_tmp2 = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOEQ, llvm_lhs_imag, llvm_rhs_imag, ""); // b == d

  return LLVMBuildAnd(ctx->llvm_builder, llvm_tmp1, llvm_tmp2, ""); // (a == c) && (b == d)
}

LLVMValueRef tau_codegen_build_complex_ne(tau_codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  // llvm_lhs => a + bi
  // llvm_rhs => c + di
  // llvm_lhs != llvm_rhs => (a != c) || (b != d)

  LLVMValueRef llvm_lhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 0, ""); // a
  LLVMValueRef llvm_lhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_lhs, 1, ""); // b

  LLVMValueRef llvm_rhs_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 0, ""); // c
  LLVMValueRef llvm_rhs_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_rhs, 1, ""); // d

  LLVMValueRef llvm_tmp1 = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealONE, llvm_lhs_real, llvm_rhs_real, ""); // a != c
  LLVMValueRef llvm_tmp2 = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealONE, llvm_lhs_imag, llvm_rhs_imag, ""); // b != d

  return LLVMBuildOr(ctx->llvm_builder, llvm_tmp1, llvm_tmp2, ""); // (a != c) || (b != d)
}

LLVMValueRef tau_codegen_build_vector_add(tau_codegen_ctx_t* ctx, tau_typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  TAU_ASSERT(desc->kind == TAU_TYPEDESC_VEC);

  tau_typedesc_vec_t* vec_desc = (tau_typedesc_vec_t*)desc;

  if (tau_typedesc_is_integer(vec_desc->base_type))
    return LLVMBuildAdd(ctx->llvm_builder, llvm_lhs, llvm_rhs, "");

  if (tau_typedesc_is_float(vec_desc->base_type))
    return LLVMBuildFAdd(ctx->llvm_builder, llvm_lhs, llvm_rhs, "");

  TAU_UNREACHABLE();

  return NULL;
}

LLVMValueRef tau_codegen_build_vector_sub(tau_codegen_ctx_t* ctx, tau_typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  TAU_ASSERT(desc->kind == TAU_TYPEDESC_VEC);

  tau_typedesc_vec_t* vec_desc = (tau_typedesc_vec_t*)desc;

  if (tau_typedesc_is_integer(vec_desc->base_type))
    return LLVMBuildSub(ctx->llvm_builder, llvm_lhs, llvm_rhs, "");

  if (tau_typedesc_is_float(vec_desc->base_type))
    return LLVMBuildFSub(ctx->llvm_builder, llvm_lhs, llvm_rhs, "");

  TAU_UNREACHABLE();

  return NULL;
}

LLVMValueRef tau_codegen_build_vector_mul_scalar(tau_codegen_ctx_t* ctx, tau_typedesc_t* desc, LLVMValueRef llvm_vec, LLVMValueRef llvm_scalar)
{
  TAU_ASSERT(desc->kind == TAU_TYPEDESC_VEC);

  tau_typedesc_vec_t* vec_desc = (tau_typedesc_vec_t*)desc;

  if (tau_typedesc_is_integer(vec_desc->base_type))
    for (size_t i = 0; i < vec_desc->size; i++)
    {
      LLVMValueRef llvm_index = LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), i, false);
      LLVMValueRef llvm_value = LLVMBuildExtractElement(ctx->llvm_builder, llvm_vec, llvm_index, "");
      llvm_value = LLVMBuildMul(ctx->llvm_builder, llvm_value, llvm_scalar, "");
      llvm_vec = LLVMBuildInsertElement(ctx->llvm_builder, llvm_vec, llvm_value, llvm_index, "");
    }
  else if (tau_typedesc_is_float(vec_desc->base_type))
    for (size_t i = 0; i < vec_desc->size; i++)
    {
      LLVMValueRef llvm_index = LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), i, false);
      LLVMValueRef llvm_value = LLVMBuildExtractElement(ctx->llvm_builder, llvm_vec, llvm_index, "");
      llvm_value = LLVMBuildFMul(ctx->llvm_builder, llvm_value, llvm_scalar, "");
      llvm_vec = LLVMBuildInsertElement(ctx->llvm_builder, llvm_vec, llvm_value, llvm_index, "");
    }
  else
    TAU_UNREACHABLE();

  return llvm_vec;
}

LLVMValueRef tau_codegen_build_vector_eq(tau_codegen_ctx_t* ctx, tau_typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  TAU_ASSERT(desc->kind == TAU_TYPEDESC_VEC);

  tau_typedesc_vec_t* vec_desc = (tau_typedesc_vec_t*)desc;

  LLVMValueRef llvm_vec_value = NULL;

  if (tau_typedesc_is_integer(vec_desc->base_type))
    llvm_vec_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntEQ, llvm_lhs, llvm_rhs, "");
  else if (tau_typedesc_is_float(vec_desc->base_type))
    llvm_vec_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOEQ, llvm_lhs, llvm_rhs, "");
  else
    TAU_UNREACHABLE();

  LLVMValueRef llvm_result_value = LLVMConstInt(LLVMInt1TypeInContext(ctx->llvm_ctx), 1, false);

  for (size_t i = 0; i < vec_desc->size; i++)
  {
    LLVMValueRef llvm_index = LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), i, false);
    LLVMValueRef llvm_vec_element = LLVMBuildExtractElement(ctx->llvm_builder, llvm_vec_value, llvm_index, "");

    llvm_result_value = LLVMBuildAnd(ctx->llvm_builder, llvm_result_value, llvm_vec_element, "");
  }

  return llvm_result_value;
}

LLVMValueRef tau_codegen_build_vector_ne(tau_codegen_ctx_t* ctx, tau_typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  TAU_ASSERT(desc->kind == TAU_TYPEDESC_VEC);

  tau_typedesc_vec_t* vec_desc = (tau_typedesc_vec_t*)desc;

  LLVMValueRef llvm_vec_value = NULL;

  if (tau_typedesc_is_integer(vec_desc->base_type))
    llvm_vec_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntNE, llvm_lhs, llvm_rhs, "");
  else if (tau_typedesc_is_float(vec_desc->base_type))
    llvm_vec_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealONE, llvm_lhs, llvm_rhs, "");
  else
    TAU_UNREACHABLE();

  LLVMValueRef llvm_result_value = LLVMConstInt(LLVMInt1TypeInContext(ctx->llvm_ctx), 0, false);

  for (size_t i = 0; i < vec_desc->size; i++)
  {
    LLVMValueRef llvm_index = LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), i, false);
    LLVMValueRef llvm_vec_element = LLVMBuildExtractElement(ctx->llvm_builder, llvm_vec_value, llvm_index, "");

    llvm_result_value = LLVMBuildOr(ctx->llvm_builder, llvm_result_value, llvm_vec_element, "");
  }

  return llvm_result_value;
}

LLVMValueRef tau_codegen_build_matrix_add(tau_codegen_ctx_t* ctx, tau_typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  TAU_ASSERT(desc->kind == TAU_TYPEDESC_MAT);

  tau_typedesc_mat_t* mat_desc = (tau_typedesc_mat_t*)desc;

  if (tau_typedesc_is_integer(mat_desc->base_type))
    return LLVMBuildAdd(ctx->llvm_builder, llvm_lhs, llvm_rhs, "");

  if (tau_typedesc_is_float(mat_desc->base_type))
    return LLVMBuildFAdd(ctx->llvm_builder, llvm_lhs, llvm_rhs, "");

  TAU_UNREACHABLE();

  return NULL;
}

LLVMValueRef tau_codegen_build_matrix_sub(tau_codegen_ctx_t* ctx, tau_typedesc_t* desc, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
{
  TAU_ASSERT(desc->kind == TAU_TYPEDESC_MAT);

  tau_typedesc_mat_t* mat_desc = (tau_typedesc_mat_t*)desc;

  if (tau_typedesc_is_integer(mat_desc->base_type))
    return LLVMBuildSub(ctx->llvm_builder, llvm_lhs, llvm_rhs, "");

  if (tau_typedesc_is_float(mat_desc->base_type))
    return LLVMBuildFSub(ctx->llvm_builder, llvm_lhs, llvm_rhs, "");

  TAU_UNREACHABLE();

  return NULL;
}

LLVMValueRef tau_codegen_build_matrix_mul_scalar(tau_codegen_ctx_t* ctx, tau_typedesc_t* desc, LLVMValueRef llvm_mat, LLVMValueRef llvm_scalar)
{
  TAU_ASSERT(desc->kind == TAU_TYPEDESC_MAT);

  tau_typedesc_mat_t* mat_desc = (tau_typedesc_mat_t*)desc;

  if (tau_typedesc_is_integer(mat_desc->base_type))
    for (size_t i = 0; i < mat_desc->rows * mat_desc->cols; i++)
    {
      LLVMValueRef llvm_index = LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), i, false);
      LLVMValueRef llvm_value = LLVMBuildExtractElement(ctx->llvm_builder, llvm_mat, llvm_index, "");
      llvm_value = LLVMBuildMul(ctx->llvm_builder, llvm_value, llvm_scalar, "");
      llvm_mat = LLVMBuildInsertElement(ctx->llvm_builder, llvm_mat, llvm_value, llvm_index, "");
    }
  else if (tau_typedesc_is_float(mat_desc->base_type))
    for (size_t i = 0; i < mat_desc->rows * mat_desc->cols; i++)
    {
      LLVMValueRef llvm_index = LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), i, false);
      LLVMValueRef llvm_value = LLVMBuildExtractElement(ctx->llvm_builder, llvm_mat, llvm_index, "");
      llvm_value = LLVMBuildFMul(ctx->llvm_builder, llvm_value, llvm_scalar, "");
      llvm_mat = LLVMBuildInsertElement(ctx->llvm_builder, llvm_mat, llvm_value, llvm_index, "");
    }
  else
    TAU_UNREACHABLE();

  return llvm_mat;
}
