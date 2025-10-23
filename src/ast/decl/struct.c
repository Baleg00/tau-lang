/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/struct.h"

#include "ast/registry.h"

tau_ast_decl_struct_t* tau_ast_decl_struct_init(void)
{
  tau_ast_decl_struct_t* node = (tau_ast_decl_struct_t*)malloc(sizeof(tau_ast_decl_struct_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_STRUCT;
  node->members = tau_vector_init();

  return node;
}

void tau_ast_decl_struct_free(tau_ast_decl_struct_t* node)
{
  tau_vector_free(node->members);
  free(node);
}

void tau_ast_decl_struct_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_struct_t* node)
{
  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->id->tok);
  tau_symbol_t* sym = tau_symbol_init_with_str_view(id_view, (tau_ast_node_t*)node);

  tau_symbol_t* collision = tau_symtable_insert(scope, sym);

  if (collision != NULL)
  {
    tau_error_bag_put_nameres_symbol_collision(ctx->errors, tau_token_location(node->tok), tau_token_location(collision->node->tok));
    return;
  }

  node->scope = tau_nameres_ctx_scope_begin(ctx);

  TAU_VECTOR_FOR_LOOP(i, node->members)
  {
    tau_ast_node_nameres(ctx, (tau_ast_node_t*)tau_vector_get(node->members, i));
  }

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_decl_struct_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_struct_t* node)
{
  tau_typedesc_t* desc = tau_typebuilder_build_struct_opaque(ctx->typebuilder, (tau_ast_node_t*)node);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);

  TAU_VECTOR_FOR_LOOP(i, node->members)
  {
    tau_ast_node_typecheck(ctx, (tau_ast_node_t*)tau_vector_get(node->members, i));
  }

  size_t field_count = tau_vector_size(node->members);
  tau_typedesc_t** field_types = NULL;

  if (field_count > 0)
  {
    field_types = (tau_typedesc_t**)malloc(sizeof(tau_typedesc_t*) * field_count);
  }

  TAU_VECTOR_FOR_LOOP(i, node->members)
  {
    TAU_ASSERT(field_types != NULL);

    tau_typedesc_t* field_desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)tau_vector_get(node->members, i));
    TAU_ASSERT(field_desc != NULL);

    field_types[i] = field_desc;
  }

  tau_typebuilder_struct_set_body(ctx->typebuilder, desc, field_types, field_count);

  if (field_types != NULL)
  {
    free(field_types);
  }
}

void tau_ast_decl_struct_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_struct_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
}

void tau_ast_decl_struct_dump_json(FILE* stream, tau_ast_decl_struct_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"members\":");
  tau_ast_node_dump_json_vector(stream, node->members);
  fputc('}', stream);
}
