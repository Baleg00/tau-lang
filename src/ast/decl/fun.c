/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/decl/fun.h"

#include "ast/ast.h"
#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_decl_fun_t* ast_decl_fun_init(void)
{
  ast_decl_fun_t* node = (ast_decl_fun_t*)malloc(sizeof(ast_decl_fun_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_DECL_FUN;

  return node;
}

void ast_decl_fun_free(ast_decl_fun_t* node)
{
  vector_free(node->params);
  free(node);
}

void ast_decl_fun_nameres(nameres_ctx_t* ctx, ast_decl_fun_t* node)
{
  symtable_t* scope = nameres_ctx_scope_cur(ctx);

  string_view_t id_view = token_to_string_view(node->id->tok);
  symbol_t* sym = symbol_init_with_str_view(id_view, (ast_node_t*)node);

  symbol_t* collision = symtable_insert(scope, sym);

  if (collision != NULL)
    report_error_type_redefinition((ast_decl_t*)collision->node, (ast_decl_t*)node);

  node->scope = nameres_ctx_scope_begin(ctx);

  VECTOR_FOR_LOOP(i, node->params)
    ast_node_nameres(ctx, (ast_node_t*)vector_get(node->params, i));

  ast_node_nameres(ctx, node->return_type);
  ast_node_nameres(ctx, node->stmt);

  nameres_ctx_scope_end(ctx);
}

void ast_decl_fun_typecheck(typecheck_ctx_t* ctx, ast_decl_fun_t* node)
{
  VECTOR_FOR_LOOP(i, node->params)
    ast_node_typecheck(ctx, (ast_node_t*)vector_get(node->params, i));

  ast_node_typecheck(ctx, node->return_type);

  size_t param_count = vector_size(node->params);
  typedesc_t** param_types = NULL;

  if (param_count > 0)
    param_types = (typedesc_t**)malloc(sizeof(typedesc_t*) * param_count);

  VECTOR_FOR_LOOP(i, node->params)
  {
    typedesc_t* param_desc = typetable_lookup(ctx->typetable, (ast_node_t*)vector_get(node->params, i));
    ASSERT(param_desc != NULL);

    param_types[i] = param_desc;
  }

  typedesc_t* return_desc = typetable_lookup(ctx->typetable, node->return_type);
  ASSERT(return_desc != NULL);

  typedesc_t* desc = typebuilder_build_fun(ctx->typebuilder, return_desc, param_types, param_count, node->is_vararg, node->callconv);

  if (param_types != NULL)
    free(param_types);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);

  ctx->fun_desc = (typedesc_fun_t*)desc;

  ast_node_typecheck(ctx, node->stmt);

  ctx->fun_desc = NULL;
}

void ast_decl_fun_ctrlflow(ctrlflow_ctx_t* ctx, ast_decl_fun_t* node)
{
  ast_node_ctrlflow(ctx, node->stmt);
}

void ast_decl_fun_codegen(codegen_ctx_t* ctx, ast_decl_fun_t* node)
{
  ast_node_codegen(ctx, node->return_type);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  string_t* id_str = token_to_string(node->id->tok);

  node->llvm_value = LLVMAddFunction(ctx->llvm_mod, string_begin(id_str), node->llvm_type);

  string_free(id_str);

  LLVMCallConv llvm_callconv = LLVMCCallConv;

  switch (node->callconv)
  {
  case CALLCONV_TAU:
  case CALLCONV_CDECL:      llvm_callconv = LLVMCCallConv;             break;
  case CALLCONV_STDCALL:    llvm_callconv = LLVMX86StdcallCallConv;    break;
  case CALLCONV_WIN64:      llvm_callconv = LLVMWin64CallConv;         break;
  case CALLCONV_SYSV64:     llvm_callconv = LLVMX8664SysVCallConv;     break;
  case CALLCONV_AAPCS:      llvm_callconv = LLVMARMAAPCSCallConv;      break;
  case CALLCONV_FASTCALL:   llvm_callconv = LLVMFastCallConv;          break;
  case CALLCONV_VECTORCALL: llvm_callconv = LLVMX86VectorCallCallConv; break;
  case CALLCONV_THISCALL:   llvm_callconv = LLVMX86ThisCallCallConv;   break;
  default: UNREACHABLE();
  }

  LLVMSetFunctionCallConv(node->llvm_value, llvm_callconv);

  if (!node->is_extern)
  {
    node->llvm_entry = LLVMAppendBasicBlockInContext(ctx->llvm_ctx, node->llvm_value, "entry");

    LLVMPositionBuilderAtEnd(ctx->llvm_builder, node->llvm_entry);

    ctx->fun_node = node;

    VECTOR_FOR_LOOP(i, node->params)
    {
      ctx->param_idx = i;
      ast_node_codegen(ctx, (ast_node_t*)vector_get(node->params, i));
    }

    ast_node_codegen(ctx, node->stmt);

    ctx->fun_node = NULL;
  }
}

size_t ast_decl_fun_mangle(ast_decl_fun_t* node, char* buf, size_t len)
{
  size_t written = snprintf(buf, len, "_T");
  written += ast_node_mangle_nested_name((ast_node_t*)node, buf + written, len <= written ? 0 : len - written);
  written += snprintf(buf + written, len <= written ? 0 : len - written, "F");
  written += callconv_mangle(node->callconv, buf + written, len <= written ? 0 : len - written);
  written += ast_node_mangle(node->return_type, buf + written, len <= written ? 0 : len - written);
  written += snprintf(buf + written, len <= written ? 0 : len - written, "%zu", vector_size(node->params));

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_decl_param_t* param = (ast_decl_param_t*)vector_get(node->params, i);
    written += ast_node_mangle(param->type, buf + written, len <= written ? 0 : len - written);
  }

  if (node->is_vararg)
    written += snprintf(buf + written, len <= written ? 0 : len - written, "V");

  return written;
}

void ast_decl_fun_dump_json(FILE* stream, ast_decl_fun_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"id\":");
  ast_node_dump_json(stream, node->id);
  fprintf(stream, ",\"is_pub\":%s", node->is_pub ? "true" : "false");
  fprintf(stream, ",\"is_extern\":%s", node->is_extern ? "true" : "false");
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fprintf(stream, ",\"callconv\":\"%s\"", callconv_kind_to_cstr(node->callconv));
  fprintf(stream, ",\"params\":");
  ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, ",\"return_type\":");
  ast_node_dump_json(stream, node->return_type);
  fprintf(stream, ",\"stmt\":");
  ast_node_dump_json(stream, node->stmt);
  fputc('}', stream);
}
