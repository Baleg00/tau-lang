/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/str.h"

#include "ast/registry.h"

tau_ast_expr_lit_str_t* tau_ast_expr_lit_str_init(void)
{
  tau_ast_expr_lit_str_t* node = (tau_ast_expr_lit_str_t*)malloc(sizeof(tau_ast_expr_lit_str_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_LIT_STR;

  return node;
}

void tau_ast_expr_lit_str_free(tau_ast_expr_lit_str_t* node)
{
  free(node->value);
  free(node);
}

void tau_ast_expr_lit_str_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_lit_str_t* TAU_UNUSED(node))
{
}

void tau_ast_expr_lit_str_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_lit_str_t* node)
{
  tau_typedesc_t* desc = tau_typebuilder_build_ptr(ctx->typebuilder, tau_typebuilder_build_u8(ctx->typebuilder));

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_lit_str_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_lit_str_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMBuildGlobalStringPtr(ctx->llvm_builder, node->value, "global_str");
}

void tau_ast_expr_lit_str_dump_json(FILE* stream, tau_ast_expr_lit_str_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));

  tau_string_t* str = tau_string_init_with_cstr(node->value);
  tau_string_t* escaped = tau_string_escape(str);

  fprintf(stream, ",\"value\":\"%s\"", tau_string_begin(escaped));

  tau_string_free(escaped);
  tau_string_free(str);

  fputc('}', stream);
}
