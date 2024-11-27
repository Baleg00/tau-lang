/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/var.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "stages/codegen/utils.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_decl_var_t* ast_decl_var_init(void)
{
  ast_decl_var_t* node = (ast_decl_var_t*)malloc(sizeof(ast_decl_var_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_VAR;

  return node;
}

void ast_decl_var_free(ast_decl_var_t* node)
{
  free(node);
}

void ast_decl_var_nameres(nameres_ctx_t* ctx, ast_decl_var_t* node)
{
  ast_node_nameres(ctx, node->type);
  ast_node_nameres(ctx, node->expr);

  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);

  symbol_t* lookup = symtable_lookup_with_str_view(scope, id_view);
  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL && collision->node->kind == AST_DECL_VAR)
    report_error_variable_redefinition((ast_decl_var_t*)collision->node, node);

  if (lookup != NULL && (lookup->node->kind == AST_DECL_VAR || lookup->node->kind == AST_DECL_PARAM))
    report_warning_shadowed_variable((ast_decl_var_t*)lookup->node, node);
}

void ast_decl_var_typecheck(typecheck_ctx_t* ctx, ast_decl_var_t* node)
{
  ast_node_typecheck(ctx, node->type);
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, node->type);
  ASSERT(desc != NULL);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);

  if (node->expr != NULL)
  {
    typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
    ASSERT(expr_desc != NULL);

    if (!typedesc_is_implicitly_convertible(expr_desc, desc, false))
    {
      location_t loc = token_location(node->tok);

      report_error_type_mismatch(loc, desc, expr_desc);
    }
  }
}

void ast_decl_var_codegen(codegen_ctx_t* ctx, ast_decl_var_t* node)
{
  ast_node_codegen(ctx, node->type);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMBuildAlloca(ctx->llvm_builder, node->llvm_type, "");

  if (node->expr != NULL)
  {
    ast_node_codegen(ctx, node->expr);

    typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);

    LLVMValueRef llvm_value = ((ast_expr_t*)node->expr)->llvm_value;
    llvm_value = codegen_build_implicit_cast(ctx, llvm_value, expr_desc, desc);

    LLVMBuildStore(ctx->llvm_builder, llvm_value, node->llvm_value);
  }
}

size_t ast_decl_var_mangle(ast_decl_var_t* node, char* buf, size_t len)
{
  size_t written = snprintf(buf, len, "_T");
  written += ast_node_mangle_nested_name((ast_node_t*)node, buf + written, len <= written ? 0 : len - written);
  written += ast_node_mangle(node->type, buf + written, len <= written ? 0 : len - written);

  return written;
}

void ast_decl_var_dump_json(FILE* stream, ast_decl_var_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"type\":");
  ast_node_dump_json(stream, node->type);
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
