/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/param.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_decl_param_t* tau_ast_decl_param_init(void)
{
  tau_ast_decl_param_t* node = (tau_ast_decl_param_t*)malloc(sizeof(tau_ast_decl_param_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_PARAM;

  return node;
}

void tau_ast_decl_param_free(tau_ast_decl_param_t* node)
{
  free(node);
}

void tau_ast_decl_param_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_param_t* node)
{
  tau_ast_node_nameres(ctx, node->type);

  if (node->expr != NULL)
  {
    tau_ast_node_nameres(ctx, node->expr);
  }

  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->id->tok);
  tau_symbol_t* sym = tau_symbol_init_with_str_view(id_view, (tau_ast_node_t*)node);

  tau_symbol_t* collision = tau_symtable_insert(scope, sym);

  if (collision != NULL && collision->node->kind == TAU_AST_DECL_PARAM)
  {
    tau_error_bag_put_nameres_symbol_collision(ctx->errors, tau_token_location(node->tok), tau_token_location(collision->node->tok));
  }
}

void tau_ast_decl_param_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_param_t* node)
{
  tau_ast_node_typecheck(ctx, node->type);

  if (node->expr != NULL)
  {
    tau_ast_node_typecheck(ctx, node->expr);
  }

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, node->type);
  TAU_ASSERT(desc != NULL);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);

  if (node->expr != NULL)
  {
    tau_typedesc_t* expr_desc = tau_typetable_lookup(ctx->typetable, node->expr);
    TAU_ASSERT(expr_desc != NULL);

    if (!tau_typedesc_is_implicitly_direct_convertible(expr_desc, desc))
      tau_error_bag_put_typecheck_illegal_conversion(ctx->errors, tau_token_location(node->tok));
  }
}

void tau_ast_decl_param_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_param_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMBuildAlloca(ctx->llvm_builder, node->llvm_type, "");

  LLVMValueRef param_value = LLVMGetParam(ctx->fun_node->llvm_value, (uint32_t)ctx->param_idx);
  LLVMBuildStore(ctx->llvm_builder, param_value, node->llvm_value);
}

void tau_ast_decl_param_dump_json(FILE* stream, tau_ast_decl_param_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"type\":");
  tau_ast_node_dump_json(stream, node->type);
  fprintf(stream, ",\"expr\":");
  tau_ast_node_dump_json(stream, node->expr);
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fputc('}', stream);
}
