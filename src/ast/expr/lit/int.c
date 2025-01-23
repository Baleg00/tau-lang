/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/int.h"

#include "ast/registry.h"

ast_expr_lit_int_t* ast_expr_lit_int_init(void)
{
  ast_expr_lit_int_t* node = (ast_expr_lit_int_t*)malloc(sizeof(ast_expr_lit_int_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_LIT_INT;

  return node;
}

void ast_expr_lit_int_free(ast_expr_lit_int_t* node)
{
  free(node);
}

void ast_expr_lit_int_nameres(nameres_ctx_t* UNUSED(ctx), ast_expr_lit_int_t* UNUSED(node))
{
}

void ast_expr_lit_int_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_int_t* node)
{
  typedesc_t* desc = NULL;

  string_view_t view = token_to_string_view(node->tok);

       if (string_view_ends_with_cstr(view, "i8" ))   desc = typebuilder_build_i8   (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "i16"))   desc = typebuilder_build_i16  (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "i32"))   desc = typebuilder_build_i32  (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "i64"))   desc = typebuilder_build_i64  (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "iz" ))   desc = typebuilder_build_isize(ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "u8" ))   desc = typebuilder_build_u8   (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "u16"))   desc = typebuilder_build_u16  (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "u32"))   desc = typebuilder_build_u32  (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "u64"))   desc = typebuilder_build_u64  (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "uz" ))   desc = typebuilder_build_usize(ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "c64" ))  desc = typebuilder_build_c64  (ctx->typebuilder);
  else if (string_view_ends_with_cstr(view, "c128" )) desc = typebuilder_build_c128 (ctx->typebuilder);
  else if (node->value <= INT8_MAX ) desc = typebuilder_build_i8 (ctx->typebuilder);
  else if (node->value <= INT16_MAX) desc = typebuilder_build_i16(ctx->typebuilder);
  else if (node->value <= INT32_MAX) desc = typebuilder_build_i32(ctx->typebuilder);
  else if (node->value <= INT64_MAX) desc = typebuilder_build_i64(ctx->typebuilder);
  else
  {
    error_bag_put_typecheck_integer_literal_too_large(ctx->errors, token_location(node->tok));
    return;
  }

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_lit_int_codegen(codegen_ctx_t* ctx, ast_expr_lit_int_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  if (typedesc_is_complex(desc))
  {
    LLVMTypeRef llvm_float_type = desc->kind == TYPEDESC_C64 ? LLVMFloatTypeInContext(ctx->llvm_ctx) : LLVMDoubleTypeInContext(ctx->llvm_ctx);

    LLVMValueRef llvm_value_real = LLVMConstReal(llvm_float_type, 0.0);
    LLVMValueRef llvm_value_imag = LLVMConstReal(llvm_float_type, (double)node->value);

    node->llvm_value = codegen_build_complex(ctx, llvm_value_real, llvm_value_imag);
  }
  else
  {
    node->llvm_value = LLVMConstInt(node->llvm_type, node->value, false);
  }
}

void ast_expr_lit_int_dump_json(FILE* stream, ast_expr_lit_int_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"value\":%llu", node->value);
  fputc('}', stream);
}
