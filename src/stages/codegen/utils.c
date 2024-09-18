/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/codegen/utils.h"

#include "utils/common.h"

LLVMValueRef codegen_build_load_if_ref(codegen_ctx_t* ctx, ast_expr_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);

  if (desc->kind == TYPEDESC_REF)
    return LLVMBuildLoad2(ctx->llvm_builder, ((typedesc_ref_t*)desc)->base_type->llvm_type, node->llvm_value, "");

  return node->llvm_value;
}

LLVMValueRef codegen_build_arithmetic_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  ASSERT(typedesc_is_arithmetic(from_desc) && typedesc_is_arithmetic(to_desc));

  if (from_desc == to_desc)
    return llvm_value;

  if (typedesc_is_integer(from_desc) && typedesc_is_integer(to_desc))
  {
    if (typedesc_integer_bits(from_desc) < typedesc_integer_bits(to_desc))
      return typedesc_is_signed(to_desc) ?
        LLVMBuildSExtOrBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "") :
        LLVMBuildZExtOrBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

    return typedesc_integer_bits(from_desc) > typedesc_integer_bits(to_desc) ?
      LLVMBuildTrunc(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "") :
      LLVMBuildBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");
  }

  if (typedesc_is_integer(from_desc) && typedesc_is_float(to_desc))
    return typedesc_is_signed(from_desc) ?
      LLVMBuildSIToFP(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "") :
      LLVMBuildUIToFP(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

  if (typedesc_is_float(from_desc) && typedesc_is_integer(to_desc))
    return typedesc_is_signed(to_desc) ?
      LLVMBuildFPToSI(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "") :
      LLVMBuildFPToUI(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

  if (typedesc_is_float(from_desc) && typedesc_is_float(to_desc))
  {
    if (from_desc->kind == TYPEDESC_F32 && to_desc->kind == TYPEDESC_F64)
      return LLVMBuildFPExt(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");

    if (from_desc->kind == TYPEDESC_F64 && to_desc->kind == TYPEDESC_F32)
      return LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "");
  }

  UNREACHABLE();

  return NULL;
}

LLVMValueRef codegen_build_opt_wrap(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_opt_t* to_desc)
{
  LLVMValueRef llvm_fields[] = {
    LLVMConstInt(LLVMInt1TypeInContext(ctx->llvm_ctx), 1, false),
    codegen_build_implicit_cast(ctx, llvm_value, from_desc, to_desc->base_type)
  };

  return LLVMConstStructInContext(ctx->llvm_ctx, llvm_fields, COUNTOF(llvm_fields), false);
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
