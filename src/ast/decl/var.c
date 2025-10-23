/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/var.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "stages/codegen/codegen.h"

tau_ast_decl_var_t* tau_ast_decl_var_init(void)
{
  tau_ast_decl_var_t* node = (tau_ast_decl_var_t*)malloc(sizeof(tau_ast_decl_var_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_VAR;

  return node;
}

void tau_ast_decl_var_free(tau_ast_decl_var_t* node)
{
  free(node);
}

void tau_ast_decl_var_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_var_t* node)
{
  tau_ast_node_nameres(ctx, node->type);

  if (node->expr != NULL)
  {
    tau_ast_node_nameres(ctx, node->expr);
  }

  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->id->tok);
  tau_symbol_t* sym = tau_symbol_init_with_str_view(id_view, (tau_ast_node_t*)node);

  tau_symbol_t* lookup = tau_symtable_lookup_with_str_view(scope, id_view);
  tau_symbol_t* collision = tau_symtable_insert(scope, sym);

  if (collision != NULL && collision->node->kind == TAU_AST_DECL_VAR)
  {
    tau_error_bag_put_nameres_symbol_collision(ctx->errors, tau_token_location(node->tok), tau_token_location(collision->node->tok));
    return;
  }

  if (lookup != NULL && (lookup->node->kind == TAU_AST_DECL_VAR || lookup->node->kind == TAU_AST_DECL_PARAM))
  {
    tau_error_bag_put_nameres_shadowed_symbol(ctx->errors, tau_token_location(lookup->node->tok), tau_token_location(node->tok));
  }
}

void tau_ast_decl_var_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_var_t* node)
{
  tau_ast_node_typecheck(ctx, node->type);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, node->type);
  TAU_ASSERT(desc != NULL);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);

  if (node->expr != NULL)
  {
    tau_ast_node_typecheck(ctx, node->expr);

    tau_typedesc_t* expr_desc = tau_typetable_lookup(ctx->typetable, node->expr);
    TAU_ASSERT(expr_desc != NULL);

    if (!tau_typedesc_is_implicitly_direct_convertible(expr_desc, desc))
      tau_error_bag_put_typecheck_illegal_conversion(ctx->errors, tau_token_location(node->tok));
  }
}

void tau_ast_decl_var_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_var_t* node)
{
  tau_ast_node_codegen(ctx, node->type);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMBuildAlloca(ctx->llvm_builder, node->llvm_type, "");

  if (node->expr != NULL)
  {
    tau_ast_node_codegen(ctx, node->expr);

    tau_typedesc_t* expr_desc = tau_typetable_lookup(ctx->typetable, node->expr);

    LLVMValueRef llvm_value = ((tau_ast_expr_t*)node->expr)->llvm_value;
    llvm_value = tau_codegen_build_implicit_cast(ctx, llvm_value, expr_desc, desc);

    LLVMBuildStore(ctx->llvm_builder, llvm_value, node->llvm_value);
  }
}

void tau_ast_decl_var_dump_json(FILE* stream, tau_ast_decl_var_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"type\":");
  tau_ast_node_dump_json(stream, node->type);
  fprintf(stream, ",\"expr\":");
  tau_ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
