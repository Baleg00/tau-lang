/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/type/fun.h"

#include "ast/registry.h"

tau_ast_type_fun_t* tau_ast_type_fun_init(void)
{
  tau_ast_type_fun_t* node = (tau_ast_type_fun_t*)malloc(sizeof(tau_ast_type_fun_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_TYPE_FUN;
  node->params = tau_vector_init();

  return node;
}

void tau_ast_type_fun_free(tau_ast_type_fun_t* node)
{
  tau_vector_free(node->params);
  free(node);
}

void tau_ast_type_fun_nameres(tau_nameres_ctx_t* ctx, tau_ast_type_fun_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_nameres(ctx, (tau_ast_node_t*)tau_vector_get(node->params, i));
  }

  tau_ast_node_nameres(ctx, node->return_type);
}

void tau_ast_type_fun_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_type_fun_t* node)
{
  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_typecheck(ctx, (tau_ast_node_t*)tau_vector_get(node->params, i));
  }

  tau_ast_node_typecheck(ctx, node->return_type);

  tau_typedesc_t* return_desc = tau_typetable_lookup(ctx->typetable, node->return_type);
  TAU_ASSERT(return_desc != NULL);

  tau_typedesc_t** param_types = (tau_typedesc_t**)malloc(sizeof(tau_typedesc_t*) * tau_vector_size(node->params));

  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_t* param = (tau_ast_node_t*)tau_vector_get(node->params, i);

    tau_typedesc_t* param_desc = tau_typetable_lookup(ctx->typetable, param);
    TAU_ASSERT(param_desc != NULL);

    param_types[i] = param_desc;
  }

  tau_typedesc_t* desc = tau_typebuilder_build_fun(ctx->typebuilder, return_desc, param_types, tau_vector_size(node->params), node->is_vararg, node->callconv);

  free(param_types);

  tau_typetable_insert(ctx->typetable, (tau_ast_node_t*)node, desc);
}

void tau_ast_type_fun_codegen(tau_codegen_ctx_t* ctx, tau_ast_type_fun_t* node)
{
  tau_ast_node_codegen(ctx, node->return_type);

  TAU_VECTOR_FOR_LOOP(i, node->params)
  {
    tau_ast_node_codegen(ctx, (tau_ast_node_t*)tau_vector_get(node->params, i));
  }

  tau_typedesc_t* desc = tau_typetable_lookup(ctx->typetable, (tau_ast_node_t*)node);
  TAU_ASSERT(desc != NULL);

  node->llvm_type = desc->llvm_type;
}

void tau_ast_type_fun_dump_json(FILE* stream, tau_ast_type_fun_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"params\":");
  tau_ast_node_dump_json_vector(stream, node->params);
  fprintf(stream, ",\"return_type\":");
  tau_ast_node_dump_json(stream, node->return_type);
  fprintf(stream, ",\"is_vararg\":%s", node->is_vararg ? "true" : "false");
  fprintf(stream, ",\"callconv\":\"%s\"", tau_callconv_kind_to_cstr(node->callconv));
  fputc('}', stream);
}
