/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/id.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_id_t* ast_expr_id_init(void)
{
  ast_expr_id_t* node = (ast_expr_id_t*)malloc(sizeof(ast_expr_id_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_ID;

  return node;
}

void ast_expr_id_free(ast_expr_id_t* node)
{
  free(node);
}

void ast_expr_id_nameres(nameres_ctx_t* ctx, ast_expr_id_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->tok);
  symbol_t* sym = symtable_lookup_with_str_view(scope, id_view);

  if (sym == NULL)
  {
    location_t loc = token_location(node->tok);

    report_error_undefined_symbol(loc);
  }

  switch (sym->node->kind)
  {
  case AST_DECL_VAR:
  case AST_DECL_PARAM:
  case AST_DECL_FUN:
  case AST_DECL_ENUM: break;
  default:
  {
    location_t loc = token_location(node->tok);

    report_error_symbol_is_not_an_expression(loc);
  }
  }

  node->decl = sym->node;
}

void ast_expr_id_typecheck(typecheck_ctx_t* ctx, ast_expr_id_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, node->decl);
  ASSERT(desc != NULL);

  if (node->decl->kind != AST_DECL_ENUM && desc->kind != TYPEDESC_REF)
    desc = typebuilder_build_ref(ctx->typebuilder, desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_id_codegen(codegen_ctx_t* UNUSED(ctx), ast_expr_id_t* node)
{
  switch (node->decl->kind)
  {
  case AST_DECL_VAR:
  {
    node->llvm_type = ((ast_decl_var_t*)node->decl)->llvm_type;
    node->llvm_value = ((ast_decl_var_t*)node->decl)->llvm_value;
    break;
  }
  case AST_DECL_PARAM:
  {
    node->llvm_type = ((ast_decl_param_t*)node->decl)->llvm_type;
    node->llvm_value = ((ast_decl_param_t*)node->decl)->llvm_value;
    break;
  }
  case AST_DECL_FUN:
  {
    node->llvm_type = ((ast_decl_fun_t*)node->decl)->llvm_type;
    node->llvm_value = ((ast_decl_fun_t*)node->decl)->llvm_value;
    break;
  }
  case AST_DECL_ENUM:
  {
    node->llvm_type = ((ast_decl_enum_t*)node->decl)->llvm_type;
    node->llvm_value = NULL;
    break;
  }
  default:
    UNREACHABLE();
  }
}

void ast_expr_id_dump_json(FILE* stream, ast_expr_id_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", ast_kind_to_cstr(node->kind));
}
