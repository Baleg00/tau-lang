/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/if.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_stmt_if_t* tau_ast_stmt_if_init(void)
{
  tau_ast_stmt_if_t* node = (tau_ast_stmt_if_t*)malloc(sizeof(tau_ast_stmt_if_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_STMT_IF;

  return node;
}

void tau_ast_stmt_if_free(tau_ast_stmt_if_t* node)
{
  free(node);
}

void tau_ast_stmt_if_nameres(tau_nameres_ctx_t* ctx, tau_ast_stmt_if_t* node)
{
  node->scope = tau_nameres_ctx_scope_begin(ctx);

  tau_ast_node_nameres(ctx, node->cond);
  tau_ast_node_nameres(ctx, node->stmt);
  tau_ast_node_nameres(ctx, node->stmt_else);

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_stmt_if_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_stmt_if_t* node)
{
  tau_ast_node_typecheck(ctx, node->cond);
  tau_ast_node_typecheck(ctx, node->stmt);
  tau_ast_node_typecheck(ctx, node->stmt_else);

  tau_typedesc_t* cond_desc = tau_typetable_lookup(ctx->typetable, node->cond);
  TAU_ASSERT(cond_desc != NULL);

  if (tau_typedesc_remove_ref_mut(cond_desc)->kind != TAU_TYPEDESC_BOOL)
    tau_error_bag_put_typecheck_expected_bool(ctx->errors, tau_token_location(node->cond->tok));
}

void tau_ast_stmt_if_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_if_t* node)
{
  tau_ast_node_ctrlflow(ctx, node->stmt);
  tau_ast_node_ctrlflow(ctx, node->stmt_else);
}

void tau_ast_stmt_if_codegen(tau_codegen_ctx_t* ctx, tau_ast_stmt_if_t* node)
{
  node->llvm_then = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "if_then");

  if (node->stmt_else != NULL)
    node->llvm_else = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "if_else");

  node->llvm_end = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "if_end");

  tau_ast_node_codegen(ctx, node->cond);

  tau_typedesc_t* cond_desc = tau_typetable_lookup(ctx->typetable, node->cond);

  LLVMValueRef llvm_cond_value = tau_codegen_build_load_if_ref(ctx, ((tau_ast_expr_t*)node->cond)->llvm_value, cond_desc);

  LLVMBuildCondBr(ctx->llvm_builder, llvm_cond_value, node->llvm_then, node->stmt_else != NULL ? node->llvm_else : node->llvm_end);
  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_then);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_then);

  tau_ast_node_codegen(ctx, node->stmt);

  if (LLVMGetBasicBlockTerminator(node->llvm_then) == NULL)
    LLVMBuildBr(ctx->llvm_builder, node->llvm_end);

  if (node->stmt_else != NULL)
  {
    LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_else);
    LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_else);

    tau_ast_node_codegen(ctx, node->stmt_else);

    if (LLVMGetBasicBlockTerminator(node->llvm_else) == NULL)
      LLVMBuildBr(ctx->llvm_builder, node->llvm_end);
  }

  LLVMAppendExistingBasicBlock(ctx->fun_node->llvm_value, node->llvm_end);
  LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_end);
}

void tau_ast_stmt_if_dump_json(FILE* stream, tau_ast_stmt_if_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"cond\":");
  tau_ast_node_dump_json(stream, node->cond);
  fprintf(stream, ",\"stmt\":");
  tau_ast_node_dump_json(stream, node->stmt);
  fprintf(stream, ",\"stmt_else\":");
  tau_ast_node_dump_json(stream, node->stmt_else);
  fputc('}', stream);
}
