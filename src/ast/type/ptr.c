/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/ptr.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_type_ptr_t* ast_type_ptr_init(void)
{
  ast_type_ptr_t* node = (ast_type_ptr_t*)malloc(sizeof(ast_type_ptr_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_PTR;

  return node;
}

void ast_type_ptr_free(ast_type_ptr_t* node)
{
  free(node);
}

void ast_type_ptr_nameres(nameres_ctx_t* ctx, ast_type_ptr_t* node)
{
  ast_node_nameres(ctx, node->base_type);
}

void ast_type_ptr_typecheck(typecheck_ctx_t* ctx, ast_type_ptr_t* node)
{
  ast_node_typecheck(ctx, node->base_type);

  typedesc_t* base_desc = typetable_lookup(ctx->typetable, node->base_type);
  assert(base_desc != NULL);
  assert(typedesc_can_add_ptr(base_desc));

  typedesc_t* desc = typebuilder_build_ptr(ctx->typebuilder, base_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_ptr_codegen(codegen_ctx_t* ctx, ast_type_ptr_t* node)
{
  ast_node_codegen(ctx, node->base_type);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  assert(desc != NULL);
  
  node->llvm_type = desc->llvm_type;
}

size_t ast_type_ptr_mangle(ast_type_ptr_t* node, char* buf, size_t len)
{
  size_t written = snprintf(buf, len, "p");
  written += ast_node_mangle(node->base_type, buf + written, len <= written ? 0 : len - written);
  
  return written;
}

void ast_type_ptr_dump_json(FILE* stream, ast_type_ptr_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"base_type\":");
  ast_node_dump_json(stream, node->base_type);
  fputc('}', stream);
}
