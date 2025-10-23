/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/return.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_stmt_return_t* tau_ast_stmt_return_init(void)
{
  tau_ast_stmt_return_t* node = (tau_ast_stmt_return_t*)malloc(sizeof(tau_ast_stmt_return_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_STMT_RETURN;

  return node;
}

void tau_ast_stmt_return_free(tau_ast_stmt_return_t* node)
{
  free(node);
}

void tau_ast_stmt_return_nameres(tau_nameres_ctx_t* ctx, tau_ast_stmt_return_t* node)
{
  tau_ast_node_nameres(ctx, node->expr);
}

void tau_ast_stmt_return_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_stmt_return_t* node)
{
  tau_ast_node_typecheck(ctx, node->expr);

  tau_typedesc_t* expr_desc = tau_typebuilder_build_unit(ctx->typebuilder);

  if (node->expr != NULL)
  {
    expr_desc = tau_typetable_lookup(ctx->typetable, node->expr);
    TAU_ASSERT(expr_desc != NULL);
  }

  if (!tau_typedesc_is_implicitly_direct_convertible(expr_desc, ctx->fun_desc->return_type))
    tau_error_bag_put_typecheck_illegal_conversion(ctx->errors, tau_token_location(node->tok));
}

void tau_ast_stmt_return_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_return_t* node)
{
  for (int i = (int)tau_vector_size(ctx->stmts) - 1; i >= 0; i--)
  {
    tau_ast_node_t* stmt_node = (tau_ast_node_t*)tau_vector_get(ctx->stmts, (size_t)i);

    if (stmt_node->kind == TAU_AST_STMT_DEFER)
    {
      tau_error_bag_put_ctrlflow_return_inside_defer(ctx->errors, tau_token_location(node->tok));
      return;
    }
  }
}

void tau_ast_stmt_return_codegen(tau_codegen_ctx_t* ctx, tau_ast_stmt_return_t* node)
{
  if (node->expr == NULL)
    LLVMBuildRetVoid(ctx->llvm_builder);
  else
  {
    tau_ast_node_codegen(ctx, node->expr);

    tau_ast_expr_t* expr_node = (tau_ast_expr_t*)node->expr;

    LLVMValueRef llvm_return_value = expr_node->llvm_value;

    tau_typedesc_t* expected_return_desc = tau_typetable_lookup(ctx->typetable, ctx->fun_node->return_type);
    tau_typedesc_t* actual_return_desc = tau_typetable_lookup(ctx->typetable, node->expr);

    if (!tau_typedesc_is_ref(expected_return_desc) && tau_typedesc_is_ref(actual_return_desc))
      llvm_return_value = LLVMBuildLoad2(ctx->llvm_builder, expr_node->llvm_type, expr_node->llvm_value, "");

    if (tau_typedesc_is_arithmetic(expected_return_desc) && tau_typedesc_is_arithmetic(actual_return_desc))
      llvm_return_value = tau_codegen_build_arithmetic_cast(ctx, llvm_return_value, actual_return_desc, expected_return_desc);

    LLVMBuildRet(ctx->llvm_builder, llvm_return_value);
  }
}

void tau_ast_stmt_return_dump_json(FILE* stream, tau_ast_stmt_return_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"expr\":");
  tau_ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
