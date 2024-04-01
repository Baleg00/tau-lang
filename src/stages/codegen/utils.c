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

  if (typedesc_remove_const(desc)->kind == TYPEDESC_REF)
    return LLVMBuildLoad2(ctx->llvm_builder, ((typedesc_ref_t*)desc)->base_type->llvm_type, node->llvm_value, "load_tmp");

  return node->llvm_value;
}

LLVMValueRef codegen_build_arithmetic_cast(codegen_ctx_t* ctx, LLVMValueRef llvm_value, typedesc_t* from_desc, typedesc_t* to_desc)
{
  ASSERT(typedesc_is_arithmetic(from_desc) && typedesc_is_arithmetic(to_desc));

  if (typedesc_is_integer(from_desc) && typedesc_is_integer(to_desc))
  {
    if (typedesc_integer_bits(from_desc) < typedesc_integer_bits(to_desc))
    {
      return typedesc_is_signed(to_desc) ?
        LLVMBuildSExtOrBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "sextorbitcast_tmp") :
        LLVMBuildZExtOrBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "zextorbitcast_tmp");
    }

    return typedesc_integer_bits(from_desc) > typedesc_integer_bits(to_desc) ?
      LLVMBuildTrunc(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "trunc_tmp") :
      LLVMBuildBitCast(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "bitcast_tmp");
  }
  else if (typedesc_is_integer(from_desc) && typedesc_is_float(to_desc))
  {
    return typedesc_is_signed(from_desc) ?
      LLVMBuildSIToFP(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "sitofp_tmp") :
      LLVMBuildUIToFP(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "uitofp_tmp");
  }
  else if (typedesc_is_float(from_desc) && typedesc_is_integer(to_desc))
  {
    return typedesc_is_signed(to_desc) ?
      LLVMBuildFPToSI(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "fptosi_tmp") :
      LLVMBuildFPToUI(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "fptoui_tmp");
  }
  else if (typedesc_is_float(from_desc) && typedesc_is_float(to_desc))
  {
    if (from_desc->kind == TYPEDESC_F32 && to_desc->kind == TYPEDESC_F64)
      return LLVMBuildFPExt(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "fpext_tmp");
    else if (from_desc->kind == TYPEDESC_F64 && to_desc->kind == TYPEDESC_F32)
      return LLVMBuildFPTrunc(ctx->llvm_builder, llvm_value, to_desc->llvm_type, "fptrunc_tmp");
  }

  return llvm_value;
}
