/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/ind.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_un_ind_t* ast_expr_op_un_ind_init(void)
{
  ast_expr_op_un_ind_t* node = (ast_expr_op_un_ind_t*)malloc(sizeof(ast_expr_op_un_ind_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;
  node->op_kind = OP_IND;

  return node;
}

void ast_expr_op_un_ind_nameres(nameres_ctx_t* ctx, ast_expr_op_un_ind_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_ind_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_ind_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
  ASSERT(expr_desc != NULL);

  if (typedesc_remove_ref_mut(expr_desc)->kind != TYPEDESC_PTR)
    report_error_expected_pointer_type(node->expr->tok->loc);
  
  typedesc_t* pointed_desc = ((typedesc_ptr_t*)typedesc_remove_ref_mut(expr_desc))->base_type;
  typedesc_t* desc = typebuilder_build_ref(ctx->typebuilder, pointed_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_ind_codegen(codegen_ctx_t* ctx, ast_expr_op_un_ind_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = ((ast_expr_t*)node->expr)->llvm_value;
}
