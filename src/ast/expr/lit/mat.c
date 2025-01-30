/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/lit/mat.h"

#include "ast/registry.h"

ast_expr_lit_mat_t* ast_expr_lit_mat_init(void)
{
  ast_expr_lit_mat_t* node = (ast_expr_lit_mat_t*)malloc(sizeof(ast_expr_lit_mat_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_LIT_MAT;
  node->values = vector_init();

  return node;
}

void ast_expr_lit_mat_free(ast_expr_lit_mat_t* node)
{
  vector_free(node->values);
  free(node);
}

void ast_expr_lit_mat_nameres(nameres_ctx_t* ctx, ast_expr_lit_mat_t* node)
{
  VECTOR_FOR_LOOP(i, node->values)
    ast_node_nameres(ctx, (ast_node_t*)vector_get(node->values, i));
}

void ast_expr_lit_mat_typecheck(typecheck_ctx_t* ctx, ast_expr_lit_mat_t* node)
{
  VECTOR_FOR_LOOP(i, node->values)
    ast_node_typecheck(ctx, (ast_node_t*)vector_get(node->values, i));

  typedesc_t* base_desc = NULL;

  VECTOR_FOR_LOOP(i, node->values)
  {
    ast_node_t* value_node = (ast_node_t*)vector_get(node->values, i);

    typedesc_t* value_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, value_node));

    if (!typedesc_is_integer(value_desc) && !typedesc_is_float(value_desc))
    {
      error_bag_put_typecheck_expected_integer_or_float(ctx->errors, token_location(value_node->tok));
      typecheck_poison(ctx, (ast_node_t*)node);
      return;
    }

    if (base_desc == NULL)
      base_desc = value_desc;
    else
      base_desc = typebuilder_build_promoted_arithmetic(ctx->typebuilder, base_desc, value_desc);
  }

  typedesc_t* desc = typebuilder_build_mat(ctx->typebuilder, node->rows, node->cols, base_desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_lit_mat_codegen(codegen_ctx_t* ctx, ast_expr_lit_mat_t* node)
{
  VECTOR_FOR_LOOP(i, node->values)
    ast_node_codegen(ctx, (ast_node_t*)vector_get(node->values, i));

  typedesc_mat_t* desc = (typedesc_mat_t*)typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  LLVMValueRef* llvm_values = (LLVMValueRef*)malloc(vector_size(node->values) * sizeof(LLVMValueRef));

  if (typedesc_is_integer(desc->base_type))
    VECTOR_FOR_LOOP(i, node->values)
      llvm_values[i] = LLVMConstInt(desc->base_type->llvm_type, 0, false);
  else if (typedesc_is_float(desc->base_type))
    VECTOR_FOR_LOOP(i, node->values)
      llvm_values[i] = LLVMConstReal(desc->base_type->llvm_type, 0.0);
  else
    UNREACHABLE();

  node->llvm_value = LLVMConstVector(llvm_values, (uint32_t)vector_size(node->values));

  free(llvm_values);

  VECTOR_FOR_LOOP(i, node->values)
  {
    ast_expr_t* value_node = (ast_expr_t*)vector_get(node->values, i);
    typedesc_t* value_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, (ast_node_t*)value_node));

    LLVMValueRef llvm_value = codegen_build_load_if_ref(ctx, value_node);
    llvm_value = codegen_build_arithmetic_cast(ctx, llvm_value, value_desc, desc->base_type);

    LLVMValueRef llvm_index = LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), i, false);

    node->llvm_value = LLVMBuildInsertElement(ctx->llvm_builder, node->llvm_value, llvm_value, llvm_index, "");
  }
}

void ast_expr_lit_mat_dump_json(FILE* stream, ast_expr_lit_mat_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\",\"rows\":%zu,\"cols\":%zu,\"values\":", ast_kind_to_cstr(node->kind), node->rows, node->cols);
  ast_node_dump_json_vector(stream, node->values);
  fputc('}', stream);
}
