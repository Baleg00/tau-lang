/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/int.h"

#include "ast/registry.h"

tau_ast_expr_lit_int_t* tau_ast_expr_lit_int_init(void)
{
  tau_ast_expr_lit_int_t* node = (tau_ast_expr_lit_int_t*)malloc(sizeof(tau_ast_expr_lit_int_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_LIT_INT;

  return node;
}

void tau_ast_expr_lit_int_free(tau_ast_expr_lit_int_t* node)
{
  free(node);
}

void tau_ast_expr_lit_int_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_lit_int_t* TAU_UNUSED(node))
{
}

void tau_ast_expr_lit_int_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_lit_int_t* node)
{
  tau_typedesc_t* desc = NULL;

  tau_string_view_t view = tau_token_to_string_view(node->tok);

       if (tau_string_view_ends_with_cstr(view, "i8" ))   desc = tau_typebuilder_build_i8   (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "i16"))   desc = tau_typebuilder_build_i16  (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "i32"))   desc = tau_typebuilder_build_i32  (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "i64"))   desc = tau_typebuilder_build_i64  (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "iz" ))   desc = tau_typebuilder_build_isize(ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "u8" ))   desc = tau_typebuilder_build_u8   (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "u16"))   desc = tau_typebuilder_build_u16  (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "u32"))   desc = tau_typebuilder_build_u32  (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "u64"))   desc = tau_typebuilder_build_u64  (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "uz" ))   desc = tau_typebuilder_build_usize(ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "c64" ))  desc = tau_typebuilder_build_c64  (ctx->typebuilder);
  else if (tau_string_view_ends_with_cstr(view, "c128" )) desc = tau_typebuilder_build_c128 (ctx->typebuilder);
  else if (node->value <= INT8_MAX ) desc = tau_typebuilder_build_i8 (ctx->typebuilder);
  else if (node->value <= INT16_MAX) desc = tau_typebuilder_build_i16(ctx->typebuilder);
  else if (node->value <= INT32_MAX) desc = tau_typebuilder_build_i32(ctx->typebuilder);
  else if (node->value <= INT64_MAX) desc = tau_typebuilder_build_i64(ctx->typebuilder);
  else
  {
    tau_error_bag_put_typecheck_integer_literal_too_large(ctx->errors, tau_token_location(node->tok));
    tau_typecheck_poison(ctx, (tau_ast_node_t*)node);
    return;
  }

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_lit_int_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_lit_int_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  if (tau_typedesc_is_complex(desc))
  {
    LLVMTypeRef llvm_float_type = desc->kind == TAU_TYPEDESC_C64 ? LLVMFloatTypeInContext(ctx->llvm_ctx) : LLVMDoubleTypeInContext(ctx->llvm_ctx);

    LLVMValueRef llvm_value_real = LLVMConstReal(llvm_float_type, 0.0);
    LLVMValueRef llvm_value_imag = LLVMConstReal(llvm_float_type, (double)node->value);

    node->llvm_value = tau_codegen_build_complex(ctx, llvm_value_real, llvm_value_imag);
  }
  else
  {
    node->llvm_value = LLVMConstInt(node->llvm_type, node->value, false);
  }
}

void tau_ast_expr_lit_int_dump_json(FILE* stream, tau_ast_expr_lit_int_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"value\":%" PRIu64, node->value);
  fputc('}', stream);
}
