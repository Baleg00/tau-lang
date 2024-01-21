/**
 * \file enum_constant.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/enum_constant.h"

#include "ast/registry.h"
#include "stages/analysis/symtable.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_decl_enum_constant_t* ast_decl_enum_constant_init(void)
{
  ast_decl_enum_constant_t* node = (ast_decl_enum_constant_t*)malloc(sizeof(ast_decl_enum_constant_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_ENUM_CONSTANT;

  return node;
}

void ast_decl_enum_constant_free(ast_decl_enum_constant_t* node)
{
  free(node);
}

void ast_decl_enum_constant_nameres(nameres_ctx_t* ctx, ast_decl_enum_constant_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);
  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL)
    report_error_enumerator_redeclaration(node->id->tok->loc, collision->node->tok->loc);
}

void ast_decl_enum_constant_dump_json(FILE* stream, ast_decl_enum_constant_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"type\":");
  ast_node_dump_json(stream, node->type);
  fputc('}', stream);
}
