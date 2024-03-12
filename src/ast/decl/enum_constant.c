/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/enum_constant.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_decl_enum_constant_t* ast_decl_enum_constant_init(void)
{
  ast_decl_enum_constant_t* node = (ast_decl_enum_constant_t*)malloc(sizeof(ast_decl_enum_constant_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_ENUM_CONSTANT;

  return node;
}

void ast_decl_enum_constant_free(ast_decl_enum_constant_t* node)
{
  free(node);
}

void ast_decl_enum_constant_nameres(nameres_ctx_t* ctx, ast_decl_enum_constant_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL)
    report_error_enum_constant_redefinition((ast_decl_enum_constant_t*)collision->node, node);
}

void ast_decl_enum_constant_typecheck(typecheck_ctx_t* ctx, ast_decl_enum_constant_t* node)
{
  typetable_insert(ctx->typetable, (ast_node_t*)node, (typedesc_t*)ctx->enum_desc);
}

void ast_decl_enum_constant_codegen(codegen_ctx_t* ctx, ast_decl_enum_constant_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMConstInt(node->llvm_type, ctx->enum_idx, false);
}

size_t ast_decl_enum_constant_mangle(ast_decl_enum_constant_t* node, char* buf, size_t len)
{
  size_t written = snprintf(buf, len, "_T");
  written += ast_node_mangle_nested_name((ast_node_t*)node, buf + written, len <= written ? 0 : len - written);

  return written;
}

void ast_decl_enum_constant_dump_json(FILE* stream, ast_decl_enum_constant_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fputc('}', stream);
}
