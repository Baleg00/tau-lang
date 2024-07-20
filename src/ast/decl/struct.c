/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/struct.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_decl_struct_t* ast_decl_struct_init(void)
{
  ast_decl_struct_t* node = (ast_decl_struct_t*)malloc(sizeof(ast_decl_struct_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_STRUCT;

  return node;
}

void ast_decl_struct_free(ast_decl_struct_t* node)
{
  vector_free(node->members);
  free(node);
}

void ast_decl_struct_nameres(nameres_ctx_t* ctx, ast_decl_struct_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);

  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL)
    report_error_type_redefinition((ast_decl_t*)collision->node, (ast_decl_t*)node);

  node->scope = nameres_ctx_scope_begin(ctx);

  VECTOR_FOR_LOOP(i, node->members)
    ast_node_nameres(ctx, (ast_node_t*)vector_get(node->members, i));

  nameres_ctx_scope_end(ctx);
}

void ast_decl_struct_typecheck(typecheck_ctx_t* ctx, ast_decl_struct_t* node)
{
  typedesc_t* desc = typebuilder_build_struct_opaque(ctx->typebuilder, (ast_node_t*)node);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);

  VECTOR_FOR_LOOP(i, node->members)
    ast_node_typecheck(ctx, (ast_node_t*)vector_get(node->members, i));

  size_t field_count = vector_size(node->members);
  typedesc_t** field_types = NULL;

  if (field_count > 0)
    field_types = (typedesc_t**)malloc(sizeof(typedesc_t*) * field_count);

  VECTOR_FOR_LOOP(i, node->members)
  {
    typedesc_t* field_desc = typetable_lookup(ctx->typetable, (ast_node_t*)vector_get(node->members, i));
    ASSERT(field_desc != NULL);

    field_types[i] = field_desc;
  }

  typebuilder_struct_set_body(ctx->typebuilder, desc, field_types, field_count);

  if (field_types != NULL)
    free(field_types);
}

void ast_decl_struct_codegen(codegen_ctx_t* ctx, ast_decl_struct_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
}

size_t ast_decl_struct_mangle(ast_decl_struct_t* node, char* buf, size_t len)
{
  size_t written = snprintf(buf, len, "_T");
  written += ast_node_mangle_nested_name((ast_node_t*)node, buf + written, len <= written ? 0 : len - written);

  return written;
}

void ast_decl_struct_dump_json(FILE* stream, ast_decl_struct_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  ast_node_dump_json_vector(stream, node->members);
  fputc('}', stream); 
}
