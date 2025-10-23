/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/vec.h"

#include "ast/registry.h"

tau_ast_type_vec_t* tau_ast_type_vec_init(void)
{
  tau_ast_type_vec_t* node = (tau_ast_type_vec_t*)malloc(sizeof(tau_ast_type_vec_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_TYPE_VEC;

  return node;
}

void tau_ast_type_vec_free(tau_ast_type_vec_t* node)
{
  free(node);
}

void tau_ast_type_vec_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_type_vec_t* TAU_UNUSED(node))
{
}

void tau_ast_type_vec_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_vec_t* node)
{
  tau_ast_node_typecheck(ctx, node->base_type);

  tau_typedesc_t* base_desc = tau_typetable_lookup(ctx->typetable, node->base_type);

  tau_typedesc_t* desc = tau_typebuilder_build_vec(ctx->typebuilder, node->size, base_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_type_vec_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_vec_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  TAU_ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void tau_ast_type_vec_dump_json(FILE* stream, tau_ast_type_vec_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"size\":%zu,\"base_type\":", tau_ast_kind_to_cstr(node->kind), node->size);
  tau_ast_node_dump_json(stream, node->base_type);
  fputc('}', stream);
}
