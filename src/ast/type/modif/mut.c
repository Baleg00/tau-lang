/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/modif/mut.h"

#include "ast/registry.h"

ast_type_mut_t* ast_type_mut_init(void)
{
  ast_type_mut_t* node = (ast_type_mut_t*)malloc(sizeof(ast_type_mut_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_MUT;

  return node;
}

void ast_type_mut_free(ast_type_mut_t* node)
{
  free(node);
}

void ast_type_mut_nameres(nameres_ctx_t* ctx, ast_type_mut_t* node)
{
  ast_node_nameres(ctx, node->base_type);
}

void ast_type_mut_typecheck(typecheck_ctx_t* ctx, ast_type_mut_t* node)
{
  ast_node_typecheck(ctx, node->base_type);

  typedesc_t* base_desc = typetable_lookup(ctx->typetable, node->base_type);
  ASSERT(base_desc != NULL);
  ASSERT(typedesc_can_add_mut(base_desc));

  typedesc_t* desc = typebuilder_build_mut(ctx->typebuilder, base_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_mut_codegen(codegen_ctx_t* ctx, ast_type_mut_t* node)
{
  ast_node_codegen(ctx, node->base_type);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void ast_type_mut_dump_json(FILE* stream, ast_type_mut_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"base_type\":");
  ast_node_dump_json(stream, node->base_type);
  fputc('}', stream);
}
