/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/flt.h"

#include "ast/registry.h"

tau_ast_expr_lit_flt_t* tau_ast_expr_lit_flt_init(void)
{
  tau_ast_expr_lit_flt_t* node = (tau_ast_expr_lit_flt_t*)malloc(sizeof(tau_ast_expr_lit_flt_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_LIT_FLT;

  return node;
}

void tau_ast_expr_lit_flt_free(tau_ast_expr_lit_flt_t* node)
{
  free(node);
}

void tau_ast_expr_lit_flt_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_expr_lit_flt_t* TAU_UNUSED(node))
{
}

void tau_ast_expr_lit_flt_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_lit_flt_t* node)
{
  tau_typedesc_t* desc = tau_typebuilder_build_f32(ctx->typebuilder);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_expr_lit_flt_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_lit_flt_t* node)
{
  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMConstReal(node->llvm_type, (double)node->value);
}

void tau_ast_expr_lit_flt_dump_json(FILE* stream, tau_ast_expr_lit_flt_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"value\":%Lf", node->value);
  fputc('}', stream);
}
