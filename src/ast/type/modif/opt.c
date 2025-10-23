/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/modif/opt.h"

#include "ast/registry.h"

tau_ast_type_opt_t* tau_ast_type_opt_init(void)
{
  tau_ast_type_opt_t* node = (tau_ast_type_opt_t*)malloc(sizeof(tau_ast_type_opt_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_TYPE_OPT;

  return node;
}

void tau_ast_type_opt_free(tau_ast_type_opt_t* node)
{
  free(node);
}

void tau_ast_type_opt_nameres(tau_nameres_ctx_t* ctx, tau_ast_type_opt_t* node)
{
  tau_ast_node_nameres(ctx, node->base_type);
}

void tau_ast_type_opt_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_opt_t* node)
{
  tau_ast_node_typecheck(ctx, node->base_type);

  tau_typedesc_t* base_desc = tau_typetable_lookup(ctx->typetable, node->base_type);
  TAU_ASSERT(base_desc != NULL);
  TAU_ASSERT(tau_typedesc_can_add_opt(base_desc));

  tau_typedesc_t* desc = tau_typebuilder_build_opt(ctx->typebuilder, base_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_type_opt_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_opt_t* node)
{
  tau_ast_node_codegen(ctx, node->base_type);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  TAU_ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void tau_ast_type_opt_dump_json(FILE* stream, tau_ast_type_opt_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"base_type\":");
  tau_ast_node_dump_json(stream, node->base_type);
  fputc('}', stream);
}
