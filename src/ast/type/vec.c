/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/vec.h"

#include "ast/registry.h"

ast_type_vec_t* ast_type_vec_init(void)
{
  ast_type_vec_t* node = (ast_type_vec_t*)malloc(sizeof(ast_type_vec_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_VEC;

  return node;
}

void ast_type_vec_free(ast_type_vec_t* node)
{
  free(node);
}

void ast_type_vec_nameres(nameres_ctx_t* UNUSED(ctx), ast_type_vec_t* UNUSED(node))
{
}

void ast_type_vec_typecheck(typecheck_ctx_t* ctx, ast_type_vec_t* node)
{
  ast_node_typecheck(ctx, node->base_type);

  typedesc_t* base_desc = typetable_lookup(ctx->typetable, node->base_type);

  typedesc_t* desc = typebuilder_build_vec(ctx->typebuilder, node->is_row, node->size, base_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_vec_codegen(codegen_ctx_t* ctx, ast_type_vec_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void ast_type_vec_dump_json(FILE* stream, ast_type_vec_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"is_row\":%s,\"size\":%zu,\"base_type\":", ast_kind_to_cstr(node->kind), node->is_row ? "true" : "false", node->size);
  ast_node_dump_json(stream, node->base_type);
  fputc('}', stream);
}
