/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/return.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_stmt_return_t* ast_stmt_return_init(void)
{
  ast_stmt_return_t* node = (ast_stmt_return_t*)malloc(sizeof(ast_stmt_return_t));
  clearobj(node);

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

  if (ctx->fun_desc == NULL)
    report_error_return_outside_function(node->tok->loc);

  // TODO: check if return is inside defer statement

  typedesc_t* expr_desc = typebuilder_build_unit(ctx->typebuilder);

  if (node->expr != NULL)
  {
    expr_desc = typetable_lookup(ctx->typetable, node->expr);
    assert(expr_desc != NULL);
  }

  if (!typedesc_is_implicitly_convertible(expr_desc, ctx->fun_desc->return_type))
    report_error_incompatible_return_type(node->tok->loc);
}

void ast_stmt_return_ctrlflow(ctrlflow_ctx_t* ctx, ast_stmt_return_t* node)
{
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

    if (typedesc_remove_const(expected_return_desc)->kind != TYPEDESC_REF &&
        typedesc_remove_const(actual_return_desc)->kind == TYPEDESC_REF)
      llvm_return_value = LLVMBuildLoad2(ctx->llvm_builder, expr_node->llvm_type, expr_node->llvm_value, "load_tmp");

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
