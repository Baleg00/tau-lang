/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/codegen/codegen.h"

#include "ast/ast.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

codegen_ctx_t* codegen_ctx_init(typebuilder_t* typebuilder, typetable_t* typetable, LLVMContextRef llvm_ctx, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_mod, LLVMBuilderRef llvm_builder)
{
  codegen_ctx_t* ctx = (codegen_ctx_t*)malloc(sizeof(codegen_ctx_t));
  CLEAROBJ(ctx);

  ctx->typebuilder = typebuilder;
  ctx->typetable = typetable;
  ctx->llvm_ctx = llvm_ctx;
  ctx->llvm_layout = llvm_layout;
  ctx->llvm_mod = llvm_mod;
  ctx->llvm_builder = llvm_builder;

  return ctx;
}

void codegen_ctx_free(codegen_ctx_t* ctx)
{
  free(ctx);
}

LLVMValueRef codegen_build_load_if_ref(codegen_ctx_t* ctx, ast_expr_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);

  if (desc->kind == TYPEDESC_REF)
    return LLVMBuildLoad2(ctx->llvm_builder, ((typedesc_ref_t*)desc)->base_type->llvm_type, node->llvm_value, "");

  return node->llvm_value;
}

static LLVMValueRef codegen_build_arithmetic_cast_from_integer_to_integer(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  if (typedesc_integer_bits(from_desc) < typedesc_integer_bits(to_desc))
  {
    if (typedesc_is_signed(to_desc))
      return LLVMBuildSExtOrBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

    return LLVMBuildZExtOrBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");
  }

  if (typedesc_integer_bits(from_desc) > typedesc_integer_bits(to_desc))
    return LLVMBuildTrunc(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

  return LLVMBuildBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");
}


static LLVMValueRef codegen_build_arithmetic_cast_from_integer_to_float(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  if (typedesc_is_signed(from_desc))
    return LLVMBuildSIToFP(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

  return LLVMBuildUIToFP(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");
}

static LLVMValueRef codegen_build_arithmetic_cast_from_integer_to_complex(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  LLVMTypeRef llvm_float_type = to_desc->kind == TYPEDESC_C64 ?
    LLVMFloatTypeInContext(ctx->llvm_ctx) :
    LLVMDoubleTypeInContext(ctx->llvm_ctx);

  LLVMValueRef llvm_value_real = typedesc_is_signed(from_desc) ?
    LLVMBuildSIToFP(ctx->llvm_builder, llvm_value, llvm_float_type, "") :
    LLVMBuildUIToFP(ctx->llvm_builder, llvm_value, llvm_float_type, "");

  LLVMValueRef llvm_value_imag = LLVMConstReal(llvm_float_type, 0.0);

  return codegen_build_complex(ctx, llvm_value_real, llvm_value_imag);
}

static LLVMValueRef codegen_build_arithmetic_cast_from_integer(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  if (typedesc_is_integer(to_desc))
    return codegen_build_arithmetic_cast_from_integer_to_integer(ctx, llvm_value, from_desc, to_desc);

  if (typedesc_is_float(to_desc))
    return codegen_build_arithmetic_cast_from_integer_to_float(ctx, llvm_value, from_desc, to_desc);

  if (typedesc_is_complex(to_desc))
    return codegen_build_arithmetic_cast_from_integer_to_complex(ctx, llvm_value, from_desc, to_desc);

  UNREACHABLE();

  return NULL;
}

static LLVMValueRef codegen_build_arithmetic_cast_from_float_to_float(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  if (from_desc->kind == TYPEDESC_F32 && to_desc->kind == TYPEDESC_F64)
    return LLVMBuildFPExt(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

  if (from_desc->kind == TYPEDESC_F64 && to_desc->kind == TYPEDESC_F32)
    return LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

  return llvm_value;
}

static LLVMValueRef codegen_build_arithmetic_cast_from_float_to_integer(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* UNUSED(from_desc), typedesc_t* to_desc)
{
  if (typedesc_is_signed(to_desc))
    return LLVMBuildFPToSI(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

  return LLVMBuildFPToUI(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");
}

static LLVMValueRef codegen_build_arithmetic_cast_from_float_to_complex(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  LLVMValueRef llvm_value_imag = LLVMConstReal(from_desc->llvm_type, 0.0);

  if (from_desc->kind == TYPEDESC_F32 && to_desc->kind == TYPEDESC_C64)
    return codegen_build_complex(ctx, llvm_value, llvm_value_imag);

  if (from_desc->kind == TYPEDESC_F32 && to_desc->kind == TYPEDESC_C128)
  {
    LLVMTypeRef llvm_float_type = LLVMDoubleTypeInContext(ctx->llvm_ctx);
    LLVMValueRef llvm_value_real = LLVMBuildFPExt(ctx->llvm_builder, llvm_value, llvm_float_type, "");

    return codegen_build_complex(ctx, llvm_value_real, llvm_value_imag);
  }

  if (from_desc->kind == TYPEDESC_F64 && to_desc->kind == TYPEDESC_C64)
  {
    LLVMTypeRef llvm_float_type = LLVMFloatTypeInContext(ctx->llvm_ctx);
    LLVMValueRef llvm_value_real = LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value, llvm_float_type, "");

    return codegen_build_complex(ctx, llvm_value_real, llvm_value_imag);
  }

  if (from_desc->kind == TYPEDESC_F64 && to_desc->kind == TYPEDESC_C128)
    return codegen_build_complex(ctx, llvm_value, llvm_value_imag);

  UNREACHABLE();

  return NULL;
}

static LLVMValueRef codegen_build_arithmetic_cast_from_float(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  if (typedesc_is_float(to_desc))
    return codegen_build_arithmetic_cast_from_float_to_float(ctx, llvm_value, from_desc, to_desc);

  if (typedesc_is_integer(to_desc))
    return codegen_build_arithmetic_cast_from_float_to_integer(ctx, llvm_value, from_desc, to_desc);

  if (typedesc_is_complex(to_desc))
    return codegen_build_arithmetic_cast_from_float_to_complex(ctx, llvm_value, from_desc, to_desc);

  UNREACHABLE();

  return NULL;
}

static LLVMValueRef codegen_build_arithmetic_cast_from_complex_to_complex(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  LLVMValueRef llvm_value_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 0, "");
  LLVMValueRef llvm_value_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 1, "");

  if (from_desc->kind == TYPEDESC_C64 && to_desc->kind == TYPEDESC_C128)
  {
    LLVMTypeRef llvm_float_type = LLVMDoubleTypeInContext(ctx->llvm_ctx);

    LLVMValueRef llvm_result_real = LLVMBuildFPExt(ctx->llvm_builder, llvm_value_real, llvm_float_type, "");
    LLVMValueRef llvm_result_imag = LLVMBuildFPExt(ctx->llvm_builder, llvm_value_imag, llvm_float_type, "");

    return codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
  }

  if (from_desc->kind == TYPEDESC_C128 && to_desc->kind == TYPEDESC_C64)
  {
    LLVMTypeRef llvm_float_type = LLVMFloatTypeInContext(ctx->llvm_ctx);

    LLVMValueRef llvm_result_real = LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value_real, llvm_float_type, "");
    LLVMValueRef llvm_result_imag = LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value_imag, llvm_float_type, "");

    return codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
  }

  UNREACHABLE();

  return NULL;
}

static LLVMValueRef codegen_build_arithmetic_cast_from_complex_to_float(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  LLVMValueRef llvm_value_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 0, "");

  if (from_desc->kind == TYPEDESC_C64 && to_desc->kind == TYPEDESC_F64)
    return LLVMBuildFPExt(ctx->llvm_builder, llvm_value_real, to_desc->llvm_type, "");

  if (from_desc->kind == TYPEDESC_C128 && to_desc->kind == TYPEDESC_F32)
    return LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value_real, to_desc->llvm_type, "");

  return llvm_value_real;
}

static LLVMValueRef codegen_build_arithmetic_cast_from_complex_to_integer(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* UNUSED(from_desc), typedesc_t* to_desc)
{
  LLVMValueRef llvm_value_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 0, "");

  if (typedesc_is_signed(to_desc))
    return LLVMBuildFPToSI(ctx->llvm_builder, llvm_value_real, to_desc->llvm_type, "");

  return LLVMBuildFPToUI(ctx->llvm_builder, llvm_value_real, to_desc->llvm_type, "");
}

static LLVMValueRef codegen_build_arithmetic_cast_from_complex(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  if (typedesc_is_integer(to_desc))
    return codegen_build_arithmetic_cast_from_complex_to_integer(ctx, llvm_value, from_desc, to_desc);

  if (typedesc_is_float(to_desc))
    return codegen_build_arithmetic_cast_from_complex_to_float(ctx, llvm_value, from_desc, to_desc);

  if (typedesc_is_complex(to_desc))
    return codegen_build_arithmetic_cast_from_complex_to_complex(ctx, llvm_value, from_desc, to_desc);

  UNREACHABLE();

  return NULL;
}

LLVMValueRef codegen_build_arithmetic_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  ASSERT(typedesc_is_arithmetic(from_desc) && typedesc_is_arithmetic(to_desc));

  if (from_desc == to_desc)
    return llvm_value;

  if (typedesc_is_integer(from_desc))
    return codegen_build_arithmetic_cast_from_integer(ctx, llvm_value, from_desc, to_desc);

  if (typedesc_is_float(from_desc))
    return codegen_build_arithmetic_cast_from_float(ctx, llvm_value, from_desc, to_desc);

  if (typedesc_is_complex(from_desc))
    return codegen_build_arithmetic_cast_from_complex(ctx, llvm_value, from_desc, to_desc);

  UNREACHABLE();

  return NULL;
}

LLVMValueRef codegen_build_opt_wrap(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_opt_t* to_desc)
{
  LLVMTypeRef llvm_bool_type = LLVMInt1TypeInContext(ctx->llvm_ctx);

  LLVMValueRef llvm_value_flag = LLVMConstInt(llvm_bool_type, 1, false);
  LLVMValueRef llvm_value_wrapped = codegen_build_implicit_cast(ctx, llvm_value, from_desc, to_desc->base_type);

  LLVMValueRef llvm_tmp1 = LLVMGetUndef(to_desc->llvm_type);

  LLVMValueRef llvm_tmp2 = LLVMBuildInsertValue(ctx->llvm_builder, llvm_tmp1, llvm_value_flag, 0, "");
  LLVMValueRef llvm_tmp3 = LLVMBuildInsertValue(ctx->llvm_builder, llvm_tmp2, llvm_value_wrapped, 1, "");

  return llvm_tmp3;
}

LLVMValueRef codegen_build_opt_unwrap_unchecked(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_opt_t* desc)
{
  return LLVMBuildStructGEP2(ctx->llvm_builder, desc->llvm_type, llvm_value, 1, "");
}

static LLVMValueRef codegen_build_implicit_cast_mut(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_mut_t* from_desc, typedesc_t* to_desc)
{
  return codegen_build_implicit_cast(ctx, llvm_value, from_desc->base_type, typedesc_remove_mut(to_desc));
}

static LLVMValueRef codegen_build_implicit_cast_ptr(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_ptr_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_OPT)
    return codegen_build_opt_wrap(ctx, llvm_value, (typedesc_t*)from_desc, (typedesc_opt_t*)to_desc);

  return llvm_value;
}

static LLVMValueRef codegen_build_implicit_cast_array(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_array_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_OPT)
    return codegen_build_opt_wrap(ctx, llvm_value, (typedesc_t*)from_desc, (typedesc_opt_t*)to_desc);

  // TODO: Cast array elements.
  return llvm_value;
}

static LLVMValueRef codegen_build_implicit_cast_ref(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_ref_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind != TYPEDESC_REF)
  {
    LLVMValueRef llvm_load_value = LLVMBuildLoad2(ctx->llvm_builder, from_desc->base_type->llvm_type, llvm_value, "");

    return codegen_build_implicit_cast(ctx, llvm_load_value, from_desc->base_type, to_desc);
  }

  return llvm_value;
}

static LLVMValueRef codegen_build_implicit_cast_opt(codegen_ctx_t* UNUSED(ctx), LLVMValueRef llvm_value, typedesc_opt_t* UNUSED(from_desc), typedesc_t* UNUSED(to_desc))
{
  return llvm_value;
}

static LLVMValueRef codegen_build_implicit_cast_prim(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_prim_t* from_desc, typedesc_t* to_desc)
{
  if (to_desc->kind == TYPEDESC_OPT)
    return codegen_build_opt_wrap(ctx, llvm_value, (typedesc_t*)from_desc, (typedesc_opt_t*)to_desc);

  if (typedesc_is_arithmetic((typedesc_t*)from_desc))
    return codegen_build_arithmetic_cast(ctx, llvm_value, (typedesc_t*)from_desc, typedesc_remove_mut(to_desc));

  if (from_desc->kind == TYPEDESC_BOOL && typedesc_remove_mut(to_desc)->kind == TYPEDESC_BOOL)
    return llvm_value;

  UNREACHABLE();

  return NULL;
}

LLVMValueRef codegen_build_implicit_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  ASSERT(typedesc_is_implicitly_convertible(from_desc, to_desc, false));

  switch (from_desc->kind)
  {
  case TYPEDESC_MUT:    return codegen_build_implicit_cast_mut   (ctx, llvm_value, (typedesc_mut_t*   )from_desc, to_desc);
  case TYPEDESC_PTR:    return codegen_build_implicit_cast_ptr   (ctx, llvm_value, (typedesc_ptr_t*   )from_desc, to_desc);
  case TYPEDESC_ARRAY:  return codegen_build_implicit_cast_array (ctx, llvm_value, (typedesc_array_t* )from_desc, to_desc);
  case TYPEDESC_REF:    return codegen_build_implicit_cast_ref   (ctx, llvm_value, (typedesc_ref_t*   )from_desc, to_desc);
  case TYPEDESC_OPT:    return codegen_build_implicit_cast_opt   (ctx, llvm_value, (typedesc_opt_t*   )from_desc, to_desc);
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
  case TYPEDESC_UNIT:   return codegen_build_implicit_cast_prim  (ctx, llvm_value, (typedesc_prim_t*  )from_desc, to_desc);
  case TYPEDESC_FUN:    return llvm_value; // TODO: add implicit cast
  case TYPEDESC_STRUCT: return llvm_value; // TODO: add implicit cast
  case TYPEDESC_UNION:  return llvm_value; // TODO: add implicit cast
  case TYPEDESC_ENUM:   return llvm_value; // TODO: add implicit cast
  default: UNREACHABLE();
  }

  return NULL;
}

LLVMValueRef codegen_build_complex(codegen_ctx_t* ctx, LLVMValueRef llvm_value_real, LLVMValueRef llvm_value_imag)
{
  LLVMTypeRef llvm_value_real_type = LLVMTypeOf(llvm_value_real);
  LLVMTypeRef llvm_value_imag_type = LLVMTypeOf(llvm_value_imag);

  ASSERT(llvm_value_real_type == llvm_value_imag_type);

  LLVMTypeRef llvm_float_type = LLVMFloatTypeInContext(ctx->llvm_ctx);
  LLVMTypeRef llvm_double_type = LLVMDoubleTypeInContext(ctx->llvm_ctx);

  typedesc_t* c_desc = NULL;

  if (llvm_value_real_type == llvm_float_type)
    c_desc = typebuilder_build_c64(ctx->typebuilder);
  else if (llvm_value_real_type == llvm_double_type)
    c_desc = typebuilder_build_c128(ctx->typebuilder);
  else
    UNREACHABLE();

  LLVMValueRef llvm_tmp1 = LLVMGetUndef(c_desc->llvm_type);

  LLVMValueRef llvm_tmp2 = LLVMBuildInsertValue(ctx->llvm_builder, llvm_tmp1, llvm_value_real, 0, "");
  LLVMValueRef llvm_tmp3 = LLVMBuildInsertValue(ctx->llvm_builder, llvm_tmp2, llvm_value_imag, 1, "");

  return llvm_tmp3;
}

LLVMValueRef codegen_build_complex_neg(codegen_ctx_t* ctx, LLVMValueRef llvm_value)
{
  // llvm_value => a + bi
  // -llvm_value => (-a) + (-b)i

  LLVMValueRef llvm_value_real = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 0, ""); // a
  LLVMValueRef llvm_value_imag = LLVMBuildExtractValue(ctx->llvm_builder, llvm_value, 1, ""); // b

  LLVMValueRef llvm_result_real = LLVMBuildFNeg(ctx->llvm_builder, llvm_value_real, ""); // -a
  LLVMValueRef llvm_result_imag = LLVMBuildFNeg(ctx->llvm_builder, llvm_value_imag, ""); // -b

  return codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef codegen_build_complex_add(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
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

  return codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef codegen_build_complex_sub(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
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

  return codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef codegen_build_complex_mul(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
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

  return codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef codegen_build_complex_div(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
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

  return codegen_build_complex(ctx, llvm_result_real, llvm_result_imag);
}

LLVMValueRef codegen_build_complex_eq(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
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

LLVMValueRef codegen_build_complex_ne(codegen_ctx_t* ctx, LLVMValueRef llvm_lhs, LLVMValueRef llvm_rhs)
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
