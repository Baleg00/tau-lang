/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/addr.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_un_addr_t* ast_expr_op_un_addr_init(void)
{
  ast_expr_op_un_addr_t* node = (ast_expr_op_un_addr_t*)malloc(sizeof(ast_expr_op_un_addr_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_IND;

  return node;
}

void ast_expr_op_un_addr_nameres(nameres_ctx_t* ctx, ast_expr_op_un_addr_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_addr_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_addr_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
  ASSERT(expr_desc != NULL);

  if (expr_desc->kind != TYPEDESC_REF)
  {
    location_t loc = token_location(node->expr->tok);

    report_error_expected_reference_type(loc);
  }

  typedesc_t* desc = typebuilder_build_ptr(ctx->typebuilder, typedesc_remove_ref(expr_desc));

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_addr_codegen(codegen_ctx_t* ctx, ast_expr_op_un_addr_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  node->llvm_value = expr->llvm_value;
}
