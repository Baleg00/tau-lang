/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/stmt/continue.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_stmt_continue_t* tau_ast_stmt_continue_init(void)
{
  tau_ast_stmt_continue_t* node = (tau_ast_stmt_continue_t*)malloc(sizeof(tau_ast_stmt_continue_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_STMT_CONTINUE;

  return node;
}

void tau_ast_stmt_continue_free(tau_ast_stmt_continue_t* node)
{
  free(node);
}

void tau_ast_stmt_continue_nameres(tau_nameres_ctx_t* TAU_UNUSED(ctx), tau_ast_stmt_continue_t* TAU_UNUSED(node))
{
}

void tau_ast_stmt_continue_typecheck(tau_typecheck_ctx_t* TAU_UNUSED(ctx), tau_ast_stmt_continue_t* TAU_UNUSED(node))
{
}

void tau_ast_stmt_continue_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_stmt_continue_t* node)
{
  if (tau_vector_empty(ctx->stmts))
  {
    tau_error_bag_put_ctrlflow_continue_outside_loop(ctx->errors, tau_token_location(node->tok));
    return;
  }

  for (int i = (int)tau_vector_size(ctx->stmts) - 1; i >= 0; i--)
  {
    tau_ast_node_t* stmt_node = (tau_ast_node_t*)tau_vector_get(ctx->stmts, (size_t)i);

    switch (stmt_node->kind)
    {
    case TAU_AST_STMT_WHILE:
    case TAU_AST_STMT_DO_WHILE:
    case TAU_AST_STMT_LOOP: node->loop = stmt_node; return;
    default: TAU_NOOP();
    }
  }

  tau_error_bag_put_ctrlflow_continue_outside_loop(ctx->errors, tau_token_location(node->tok));
}

void tau_ast_stmt_continue_codegen(tau_codegen_ctx_t* ctx, tau_ast_stmt_continue_t* node)
{
  switch (node->loop->kind)
  {
  case TAU_AST_STMT_WHILE:    LLVMBuildBr(ctx->llvm_builder, ((tau_ast_stmt_while_t*   )node->loop)->llvm_cond ); break;
  case TAU_AST_STMT_DO_WHILE: LLVMBuildBr(ctx->llvm_builder, ((tau_ast_stmt_do_while_t*)node->loop)->llvm_cond ); break;
  case TAU_AST_STMT_LOOP:     LLVMBuildBr(ctx->llvm_builder, ((tau_ast_stmt_loop_t*    )node->loop)->llvm_begin); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_stmt_continue_dump_json(FILE* stream, tau_ast_stmt_continue_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"}", tau_ast_kind_to_cstr(node->kind));
}
