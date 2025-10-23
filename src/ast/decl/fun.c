/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/fun.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_decl_fun_t* tau_ast_decl_fun_init(void)
{
  tau_ast_decl_fun_t* node = (tau_ast_decl_fun_t*)malloc(sizeof(tau_ast_decl_fun_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_DECL_FUN;
  node->params = tau_vector_init();

  return node;
}

void tau_ast_decl_fun_free(tau_ast_decl_fun_t* node)
{
  tau_vector_free(node->params);
  free(node);
}

void tau_ast_decl_fun_nameres(tau_nameres_ctx_t* ctx, tau_ast_decl_fun_t* node)
{
  tau_symtable_t* scope = tau_nameres_ctx_scope_cur(ctx);

  tau_string_view_t id_view = tau_token_to_string_view(node->id->tok);
  tau_symbol_t* sym = tau_symbol_init_with_str_view(id_view, (tau_ast_node_t*)node);

  tau_symbol_t* collision = tau_symtable_insert(scope, sym);

  if (collision != NULL)
  {
    tau_error_bag_put_nameres_symbol_collision(ctx->errors, tau_token_location(node->tok), tau_token_location(collision->node->tok));
    return;
  }

  node->scope = tau_nameres_ctx_scope_begin(ctx);

  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_nameres(ctx, (tau_ast_node_t*)tau_vector_get(node->params, i));
  }

  tau_ast_node_nameres(ctx, node->return_type);

  if (node->stmt != NULL)
  {
    tau_ast_node_nameres(ctx, node->stmt);
  }

  tau_nameres_ctx_scope_end(ctx);
}

void tau_ast_decl_fun_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_decl_fun_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_typecheck(ctx, (tau_ast_node_t*)tau_vector_get(node->params, i));
  }

  tau_ast_node_typecheck(ctx, node->return_type);

  size_t param_count = tau_vector_size(node->params);
  tau_typedesc_t** param_types = NULL;

  if (param_count > 0)
  {
    param_types = (tau_typedesc_t**)malloc(sizeof(tau_typedesc_t*) * param_count);
  }

  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    TAU_ASSERT(param_types != NULL);

    tau_typedesc_t* param_desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)tau_vector_get(node->params, i));
    TAU_ASSERT(param_desc != NULL);

    param_types[i] = param_desc;
  }

  tau_typedesc_t* return_desc = tau_typetable_lookup(ctx->typetable, node->return_type);
  TAU_ASSERT(return_desc != NULL);

  tau_typedesc_t* desc = tau_typebuilder_build_fun(ctx->typebuilder, return_desc, param_types, param_count, node->is_vararg, node->callconv);

  if (param_types != NULL)
  {
    free(param_types);
  }

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);

  ctx->fun_desc = (tau_typedesc_fun_t*)desc;

  if (node->stmt != NULL)
  {
    tau_ast_node_typecheck(ctx, node->stmt);
  }

  ctx->fun_desc = NULL;
}

void tau_ast_decl_fun_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_decl_fun_t* node)
{
  if (node->stmt != NULL)
  {
    tau_ast_node_ctrlflow(ctx, node->stmt);
  }
}

void tau_ast_decl_fun_codegen(tau_codegen_ctx_t* ctx, tau_ast_decl_fun_t* node)
{
  tau_ast_node_codegen(ctx, node->return_type);

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  tau_string_t* id_str = tau_token_to_string(node->id->tok);

  node->llvm_value = LLVMAddFunction(ctx->llvm_mod, tau_string_begin(id_str), node->llvm_type);

  tau_string_free(id_str);

  LLVMCallConv llvm_callconv = LLVMCCallConv;

  switch (node->callconv)
  {
  case TAU_CALLCONV_TAU:
  case TAU_CALLCONV_CDECL:      llvm_callconv = LLVMCCallConv;             break;
  case TAU_CALLCONV_STDCALL:    llvm_callconv = LLVMX86StdcallCallConv;    break;
  case TAU_CALLCONV_WIN64:      llvm_callconv = LLVMWin64CallConv;         break;
  case TAU_CALLCONV_SYSV64:     llvm_callconv = LLVMX8664SysVCallConv;     break;
  case TAU_CALLCONV_AAPCS:      llvm_callconv = LLVMARMAAPCSCallConv;      break;
  case TAU_CALLCONV_FASTCALL:   llvm_callconv = LLVMFastCallConv;          break;
  case TAU_CALLCONV_VECTORCALL: llvm_callconv = LLVMX86VectorCallCallConv; break;
  case TAU_CALLCONV_THISCALL:   llvm_callconv = LLVMX86ThisCallCallConv;   break;
  default: TAU_UNREACHABLE();
  }

  LLVMSetFunctionCallConv(node->llvm_value, llvm_callconv);

  if (!node->is_extern)
  {
    node->llvm_entry = LLVMAppendBasicBlockInContext(ctx->llvm_ctx, node->llvm_value, "entry");

    LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_entry);

    ctx->fun_node = node;

    TAU_VECTOR_FOR_LOOP(i, node->params)
    {
      ctx->param_idx = i;
      tau_ast_node_codegen(ctx, (tau_ast_node_t*)tau_vector_get(node->params, i));
    }

    tau_ast_node_codegen(ctx, node->stmt);

    ctx->fun_node = NULL;
  }
}

void tau_ast_decl_fun_dump_json(FILE* stream, tau_ast_decl_fun_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  tau_ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"is_extern\":%s", node->is_extern ? "true" : "false");
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fprintf(stream, ",\"callconv\":\"%s\"", tau_callconv_kind_to_cstr(node->callconv));
  fprintf(stream, ",\"params\":");
  tau_ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, ",\"return_type\":");
  tau_ast_node_dump_json(stream, node->return_type);
  fprintf(stream, ",\"stmt\":");
  tau_ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
