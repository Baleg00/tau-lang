/**
 * \file fun.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/fun.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

ast_type_fun_t* ast_type_fun_init(void)
{
  ast_type_fun_t* node = (ast_type_fun_t*)malloc(sizeof(ast_type_fun_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_TYPE_FUN;

  return node;
}

void ast_type_fun_free(ast_type_fun_t* node)
{
  vector_free(node->params);
  free(node);
}

void ast_type_fun_nameres(nameres_ctx_t* ctx, ast_type_fun_t* node)
{
  VECTOR_FOR_LOOP(i, node->params)
    ast_node_nameres(ctx, (ast_node_t*)vector_get(node->params, i));

  ast_node_nameres(ctx, node->return_type);
}

void ast_type_fun_typecheck(typecheck_ctx_t* ctx, ast_type_fun_t* node)
{
  VECTOR_FOR_LOOP(i, node->params)
    ast_node_typecheck(ctx, (ast_node_t*)vector_get(node->params, i));

  ast_node_typecheck(ctx, node->return_type);

  typedesc_t* return_desc = typetable_lookup(ctx->typetable, node->return_type);
  assert(return_desc != NULL);

  typedesc_t** param_types = (typedesc_t**)malloc(sizeof(typedesc_t*) * vector_size(node->params));

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_node_t* param = (ast_node_t*)vector_get(node->params, i);

    typedesc_t* param_desc = typetable_lookup(ctx->typetable, param);
    assert(param_desc != NULL);

    param_types[i] = param_desc;
  }

  typedesc_t* desc = typebuilder_build_fun(ctx->typebuilder, return_desc, param_types, vector_size(node->params), node->is_vararg, node->callconv);

  free(param_types);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_type_fun_codegen(codegen_ctx_t* ctx, ast_type_fun_t* node)
{
  ast_node_codegen(ctx, node->return_type);

  VECTOR_FOR_LOOP(i, node->params)
    ast_node_codegen(ctx, (ast_node_t*)vector_get(node->params, i));

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  assert(desc != NULL);
  
  node->llvm_type = desc->llvm_type;
}

size_t ast_type_fun_mangle(ast_type_fun_t* node, char* buf, size_t len)
{
  size_t written = snprintf(buf, len, "F");
  written += callconv_mangle(node->callconv, buf + written, len <= written ? 0 : len - written);
  written += ast_node_mangle(node->return_type, buf + written, len <= written ? 0 : len - written);

  VECTOR_FOR_LOOP(i, node->params)
    written += ast_node_mangle((ast_node_t*)vector_get(node->params, i), buf + written, len <= written ? 0 : len - written);
  
  if (node->is_vararg)
    written += snprintf(buf + written, len <= written ? 0 : len - written, "V");

  written += snprintf(buf + written, len <= written ? 0 : len - written, "@");

  return written;
}

void ast_type_fun_dump_json(FILE* stream, ast_type_fun_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"params\":");
  ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, ",\"return_type\":");
  ast_node_dump_json(stream, node->return_type);
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fprintf(stream, ",\"callconv\":\"%s\"", callconv_kind_to_cstr(node->callconv));
  fputc('}', stream);
}
