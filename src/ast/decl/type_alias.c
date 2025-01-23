/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/type_alias.h"

#include "ast/registry.h"

ast_decl_type_alias_t* ast_decl_type_alias_init(void)
{
  ast_decl_type_alias_t* node = (ast_decl_type_alias_t*)malloc(sizeof(ast_decl_type_alias_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_TYPE_ALIAS;

  return node;
}

void ast_decl_type_alias_free(ast_decl_type_alias_t* node)
{
  free(node);
}

void ast_decl_type_alias_nameres(nameres_ctx_t* ctx, ast_decl_type_alias_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);

  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL)
  {
    error_bag_put_nameres_symbol_collision(ctx->errors, token_location(node->tok), token_location(collision->node->tok));
    return;
  }

  node->scope = nameres_ctx_scope_begin(ctx);

  ast_node_nameres(ctx, node->type);

  nameres_ctx_scope_end(ctx);
}

void ast_decl_type_alias_typecheck(typecheck_ctx_t* ctx, ast_decl_type_alias_t* node)
{
  ast_node_typecheck(ctx, node->type);

  typedesc_t* desc = typetable_lookup(ctx->typetable, node->type);
  ASSERT(desc != NULL);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_decl_type_alias_codegen(codegen_ctx_t* ctx, ast_decl_type_alias_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void ast_decl_type_alias_dump_json(FILE* stream, ast_decl_type_alias_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"type\":");
  ast_node_dump_json(stream, node->type);
  fputc('}', stream);
}
