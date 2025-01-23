/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/return.h"

#include "ast/ast.h"
#include "ast/registry.h"

ast_stmt_return_t* ast_stmt_return_init(void)
{
  ast_stmt_return_t* node = (ast_stmt_return_t*)malloc(sizeof(ast_stmt_return_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_STMT_RETURN;

  return node;
}

void ast_stmt_return_free(ast_stmt_return_t* node)
{
  free(node);
}

void ast_stmt_return_nameres(nameres_ctx_t* ctx, ast_stmt_return_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_stmt_return_typecheck(typecheck_ctx_t* ctx, ast_stmt_return_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typebuilder_build_unit(ctx->typebuilder);

  if (node->expr != NULL)
  {
    expr_desc = typetable_lookup(ctx->typetable, node->expr);
    ASSERT(expr_desc != NULL);

    if (!typedesc_is_implicitly_direct_convertible(expr_desc, ctx->fun_desc->return_type))
    {
      error_bag_put_typecheck_illegal_conversion(ctx->errors, token_location(node->expr->tok));
      return;
    }
  }

  if (!typedesc_is_implicitly_direct_convertible(expr_desc, ctx->fun_desc->return_type))
  {
    error_bag_put_typecheck_illegal_conversion(ctx->errors, token_location(node->tok));
  }
}

void ast_stmt_return_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_return_t* node)
{
  for (int i = (int)vector_size(ctx->stmts) - 1; i >= 0; i--)
  {
    ast_node_t* stmt_node = (ast_node_t*)vector_get(ctx->stmts, (size_t)i);

    if (stmt_node->kind == AST_STMT_DEFER)
    {
      error_bag_put_ctrlflow_return_inside_defer(ctx->errors, token_location(node->tok));
      return;
    }
  }
}

void ast_stmt_return_codegen(codegen_ctx_t* ctx, ast_stmt_return_t* node)
{
  if (node->expr == NULL)
    LLVMBuildRetVoid(ctx->llvm_builder);
  else
  {
    ast_node_codegen(ctx, node->expr);

    ast_expr_t* expr_node = (ast_expr_t*)node->expr;

    LLVMValueRef llvm_return_value = expr_node->llvm_value;

    typedesc_t* expected_return_desc = typetable_lookup(ctx->typetable, ctx->fun_node->return_type);
    typedesc_t* actual_return_desc = typetable_lookup(ctx->typetable, node->expr);

    if (!typedesc_is_ref(expected_return_desc) && typedesc_is_ref(actual_return_desc))
      llvm_return_value = LLVMBuildLoad2(ctx->llvm_builder, expr_node->llvm_type, expr_node->llvm_value, "");

    if (typedesc_is_arithmetic(expected_return_desc) && typedesc_is_arithmetic(actual_return_desc))
      llvm_return_value = codegen_build_arithmetic_cast(ctx, llvm_return_value, actual_return_desc, expected_return_desc);

    LLVMBuildRet(ctx->llvm_builder, llvm_return_value);
  }
}

void ast_stmt_return_dump_json(FILE* stream, ast_stmt_return_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
