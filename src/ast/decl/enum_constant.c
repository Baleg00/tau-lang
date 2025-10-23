/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/enum_constant.h"

#include "ast/registry.h"

tau_ast_decl_enum_constant_t* tau_ast_decl_enum_constant_init(void)
{
  tau_ast_decl_enum_constant_t* node = (tau_ast_decl_enum_constant_t*)malloc(sizeof(tau_ast_decl_enum_constant_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_ENUM_CONSTANT;

  return node;
}

void tau_ast_decl_enum_constant_free(tau_ast_decl_enum_constant_t* node)
{
  free(node);
}

void tau_ast_decl_enum_constant_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_enum_constant_t* node)
{
  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->id->tok);
  tau_symbol_t* sym = tau_symbol_init_with_str_view(id_view, (tau_ast_node_t*)node);
  tau_symbol_t* collision = tau_symtable_insert(scope, sym);

  if (collision != NULL)
  {
    tau_error_bag_put_nameres_symbol_collision(ctx->errors, tau_token_location(node->tok), tau_token_location(collision->node->tok));
  }
}

void tau_ast_decl_enum_constant_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_enum_constant_t* node)
{
  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, (tau_typedesc_t*)ctx->enum_desc);
}

void tau_ast_decl_enum_constant_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_enum_constant_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMConstInt(node->llvm_type, ctx->enum_idx, false);
}

void tau_ast_decl_enum_constant_dump_json(FILE* stream, tau_ast_decl_enum_constant_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fputc('}', stream);
}
