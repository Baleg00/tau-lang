/**
 * \file str.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/str.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_expr_lit_str_t* ast_expr_lit_str_init(void)
{
  ast_expr_lit_str_t* node = (ast_expr_lit_str_t*)malloc(sizeof(ast_expr_lit_str_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_LIT_STR;

  return node;
}

void ast_expr_lit_str_free(ast_expr_lit_str_t* node)
{
  free(node->value);
  free(node);
}

void ast_expr_lit_str_nameres(nameres_ctx_t* ctx, ast_expr_lit_str_t* node)
{
}

void ast_expr_lit_str_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_str_t* node)
{
  typedesc_t* desc = typebuilder_build_ptr(ctx->typebuilder, typebuilder_build_u8(ctx->typebuilder));

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_lit_str_codegen(codegen_ctx_t* ctx, ast_expr_lit_str_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMBuildGlobalStringPtr(ctx->llvm_builder, node->value, "global_str");
}

void ast_expr_lit_str_dump_json(FILE* stream, ast_expr_lit_str_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));

  string_t* str = string_init_with_cstr(node->value);
  string_t* escaped = string_escape(str);
  
  fprintf(stream, ",\"value\":\"%s\"", string_begin(escaped));

  string_free(escaped);
  string_free(str);

  fputc('}', stream);
}
