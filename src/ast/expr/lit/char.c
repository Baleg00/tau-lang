/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/char.h"

#include "ast/registry.h"

tau_ast_expr_lit_char_t* tau_ast_expr_lit_char_init(void)
{
  tau_ast_expr_lit_char_t* node = (tau_ast_expr_lit_char_t*)malloc(sizeof(tau_ast_expr_lit_char_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_LIT_CHAR;

  return node;
}

void tau_ast_expr_lit_char_free(tau_ast_expr_lit_char_t* node)
{
  free(node);
}

void tau_ast_expr_lit_char_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_lit_char_t* TAU_UNUSED(node))
{
}

void tau_ast_expr_lit_char_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_lit_char_t* node)
{
  tau_typedesc_t* desc = tau_typebuilder_build_char(ctx->typebuilder);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_lit_char_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_lit_char_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMConstInt(node->llvm_type, node->value, false);
}

void tau_ast_expr_lit_char_dump_json(FILE* stream, tau_ast_expr_lit_char_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"value\":%" PRIu32, node->value);
  fputc('}', stream);
}
