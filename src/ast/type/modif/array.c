/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/modif/array.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_type_array_t* tau_ast_type_array_init(void)
{
  tau_ast_type_array_t* node = (tau_ast_type_array_t*)malloc(sizeof(tau_ast_type_array_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_TYPE_ARRAY;

  return node;
}

void tau_ast_type_array_free(tau_ast_type_array_t* node)
{
  free(node);
}

void tau_ast_type_array_nameres(tau_nameres_ctx_t* ctx, tau_ast_type_array_t* node)
{
  tau_ast_node_nameres(ctx, node->base_type);
  tau_ast_node_nameres(ctx, node->size);
}

void tau_ast_type_array_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_array_t* node)
{
  tau_ast_node_typecheck(ctx, node->base_type);
  tau_ast_node_typecheck(ctx, node->size);

  tau_typedesc_t* base_desc = tau_typetable_lookup(ctx->typetable, node->base_type);
  TAU_ASSERT(base_desc != NULL);
  TAU_ASSERT(tau_typedesc_can_add_array(base_desc));

  TAU_ASSERT(node->size->kind == TAU_AST_EXPR_LIT_INT);

  tau_typedesc_t* desc = tau_typebuilder_build_array(ctx->typebuilder, (size_t)((tau_ast_expr_lit_int_t*)node->size)->value, base_desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_type_array_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_array_t* node)
{
  tau_ast_node_codegen(ctx, node->base_type);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  TAU_ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void tau_ast_type_array_dump_json(FILE* stream, tau_ast_type_array_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"base_type\":");
  tau_ast_node_dump_json(stream, node->base_type);
  fprintf(stream, ",\"size\":");
  tau_ast_node_dump_json(stream, node->size);
  fputc('}', stream);
}
