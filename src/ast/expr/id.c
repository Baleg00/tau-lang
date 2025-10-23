/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/id.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_id_t* tau_ast_expr_id_init(void)
{
  tau_ast_expr_id_t* node = (tau_ast_expr_id_t*)malloc(sizeof(tau_ast_expr_id_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_ID;

  return node;
}

void tau_ast_expr_id_free(tau_ast_expr_id_t* node)
{
  free(node);
}

void tau_ast_expr_id_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_id_t* node)
{
  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->tok);
  tau_symbol_t* sym = tau_symtable_lookup_with_str_view(scope, id_view);

  if (sym == NULL)
  {
    tau_error_bag_put_nameres_undefined_symbol(ctx->errors, tau_token_location(node->tok));
    return;
  }

  switch (sym->node->kind)
  {
  case TAU_AST_DECL_VAR:
  case TAU_AST_DECL_PARAM:
  case TAU_AST_DECL_FUN:
  case TAU_AST_DECL_ENUM: break;
  default:
  {
    tau_error_bag_put_nameres_expected_expression_symbol(ctx->errors, tau_token_location(node->tok));
    return;
  }
  }

  node->decl = sym->node;
}

void tau_ast_expr_id_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_id_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, node->decl);
  TAU_ASSERT(desc != NULL);

  if (node->decl->kind != TAU_AST_DECL_ENUM && !tau_typedesc_is_ref(desc))
    desc = tau_typebuilder_build_ref(ctx->typebuilder, desc);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_id_codegen(tau_codegen_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_id_t* node)
{
  switch (node->decl->kind)
  {
  case TAU_AST_DECL_VAR:
  {
    node->llvm_type = ((tau_ast_decl_var_t*)node->decl)->llvm_type;
    node->llvm_value = ((tau_ast_decl_var_t*)node->decl)->llvm_value;
    break;
  }
  case TAU_AST_DECL_PARAM:
  {
    node->llvm_type = ((tau_ast_decl_param_t*)node->decl)->llvm_type;
    node->llvm_value = ((tau_ast_decl_param_t*)node->decl)->llvm_value;
    break;
  }
  case TAU_AST_DECL_FUN:
  {
    node->llvm_type = ((tau_ast_decl_fun_t*)node->decl)->llvm_type;
    node->llvm_value = ((tau_ast_decl_fun_t*)node->decl)->llvm_value;
    break;
  }
  case TAU_AST_DECL_ENUM:
  {
    node->llvm_type = ((tau_ast_decl_enum_t*)node->decl)->llvm_type;
    node->llvm_value = NULL;
    break;
  }
  default:
    TAU_UNREACHABLE();
  }
}

void tau_ast_expr_id_dump_json(FILE* stream, tau_ast_expr_id_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", tau_ast_kind_to_cstr(node->kind));
}
