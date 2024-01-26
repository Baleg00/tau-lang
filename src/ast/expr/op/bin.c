/**
 * \file bin.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin.h"

#include <llvm-c/Core.h>

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

static LLVMValueRef codegen_build_load_if_ref(codegen_ctx_t* ctx, ast_expr_t* node)
{
  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);

  if (typedesc_remove_const(desc)->kind == TYPEDESC_REF)
    return LLVMBuildLoad2(ctx->llvm_builder, node->llvm_type, node->llvm_value, "load_tmp");

  return node->llvm_value;
}

ast_expr_op_bin_t* ast_expr_op_bin_init(void)
{
  ast_expr_op_bin_t* node = (ast_expr_op_bin_t*)malloc(sizeof(ast_expr_op_bin_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;

  return node;
}

void ast_expr_op_bin_free(ast_expr_op_bin_t* node)
{
  free(node);
}

void ast_expr_op_bin_nameres(nameres_ctx_t* ctx, ast_expr_op_bin_t* node)
{
  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node)
{
  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  assert(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  assert(rhs_desc != NULL);

  typedesc_t* desc = NULL;

  switch (node->op_kind)
  {
  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
  case OP_BIT_AND:
  case OP_BIT_OR:
  case OP_BIT_XOR:
  {
    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(lhs_desc)))
      report_error_expected_arithmetic_type(node->lhs->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(rhs_desc)))
      report_error_expected_arithmetic_type(node->rhs->tok->loc);

    if (typedesc_is_signed(typedesc_remove_const_ref_mut(lhs_desc)) != typedesc_is_signed(typedesc_remove_const_ref_mut(rhs_desc)))
      report_warning_mixed_signedness(node->tok->loc);

    desc = typedesc_arithmetic_promote(typedesc_remove_const_ref_mut(lhs_desc), typedesc_remove_const_ref_mut(rhs_desc));
    break;
  }
  case OP_BIT_LSH:
  case OP_BIT_RSH:
  {
    if (!typedesc_is_integer(typedesc_remove_const_ref_mut(lhs_desc)))
      report_error_expected_integer_type(node->lhs->tok->loc);

    if (!typedesc_is_integer(typedesc_remove_const_ref_mut(rhs_desc)))
      report_error_expected_integer_type(node->rhs->tok->loc);

    desc = typedesc_remove_const_ref_mut(lhs_desc);
    break;
  }
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
  {
    if (typedesc_remove_const_ref_mut(lhs_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->lhs->tok->loc);

    if (typedesc_remove_const_ref_mut(rhs_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->rhs->tok->loc);

    desc = typebuilder_build_bool(ctx->typebuilder);
    break;
  }
  case OP_COMP_EQ:
  case OP_COMP_NE:
  case OP_COMP_LT:
  case OP_COMP_LE:
  case OP_COMP_GT:
  case OP_COMP_GE:
  {
    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(lhs_desc)))
      report_error_expected_arithmetic_type(node->lhs->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(rhs_desc)))
      report_error_expected_arithmetic_type(node->rhs->tok->loc);

    desc = typebuilder_build_bool(ctx->typebuilder);
    break;
  }
  case OP_ASSIGN:
  {
    if (typedesc_remove_const(lhs_desc)->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->lhs->tok->loc);

    if (typedesc_remove_const_ref(lhs_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->lhs->tok->loc);

    if (typedesc_remove_const_ref_mut(lhs_desc) != typedesc_remove_const_ref_mut(rhs_desc))
      report_error_type_mismatch(node->lhs->tok->loc, lhs_desc, rhs_desc);

    desc = lhs_desc;
    break;
  }
  default:
    unreachable();
  }

  if (lhs_desc->kind == TYPEDESC_CONST && rhs_desc->kind == TYPEDESC_CONST)
    desc = typebuilder_build_const(ctx->typebuilder, desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_bin_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node)
{
  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->lhs);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->rhs);

  switch (node->op_kind)
  {
  case OP_ARIT_ADD:
  {
    node->llvm_value = LLVMBuildAdd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "add_tmp");
    break;
  }
  case OP_ARIT_SUB:
  {
    node->llvm_value = LLVMBuildSub(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "sub_tmp");
    break;
  }
  case OP_ARIT_MUL:
  {
    node->llvm_value = LLVMBuildMul(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "mul_tmp");
    break;
  }
  case OP_ARIT_DIV:
  {
    node->llvm_value = LLVMBuildSDiv(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "sdiv_tmp");
    break;
  }
  case OP_ARIT_MOD:
  {
    node->llvm_value = LLVMBuildSRem(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "srem_tmp");
    break;
  }
  case OP_BIT_AND:
  case OP_LOGIC_AND:
  {
    node->llvm_value = LLVMBuildAnd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "and_tmp");
    break;
  }
  case OP_BIT_OR:
  case OP_LOGIC_OR:
  {
    node->llvm_value = LLVMBuildOr(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "or_tmp");
    break;
  }
  case OP_BIT_XOR:
  {
    node->llvm_value = LLVMBuildXor(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "xor_tmp");
    break;
  }
  case OP_BIT_LSH:
  {
    node->llvm_value = LLVMBuildShl(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "shl_tmp");
    break;
  }
  case OP_BIT_RSH:
  {
    node->llvm_value = LLVMBuildLShr(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "lshr_tmp");
    break;
  }
  case OP_COMP_EQ:
  {
    node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntEQ, llvm_lhs_value, llvm_rhs_value, "icmp_eq_tmp");
    break;
  }
  case OP_COMP_NE:
  {
    node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntNE, llvm_lhs_value, llvm_rhs_value, "icmp_ne_tmp");
    break;
  }
  case OP_COMP_LT:
  {
    node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSLT, llvm_lhs_value, llvm_rhs_value, "icmp_slt_tmp");
    break;
  }
  case OP_COMP_LE:
  {
    node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSLE, llvm_lhs_value, llvm_rhs_value, "icmp_sle_tmp");
    break;
  }
  case OP_COMP_GT:
  {
    node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSGT, llvm_lhs_value, llvm_rhs_value, "icmp_sgt_tmp");
    break;
  }
  case OP_COMP_GE:
  {
    node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSGE, llvm_lhs_value, llvm_rhs_value, "icmp_sge_tmp");
    break;
  }
  case OP_ASSIGN:
  {
    LLVMBuildStore(ctx->llvm_builder, llvm_rhs_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_ARIT_ADD_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildAdd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "add_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_ARIT_SUB_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildSub(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "sub_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_ARIT_MUL_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildMul(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "mul_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_ARIT_DIV_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildSDiv(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "sdiv_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_ARIT_MOD_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildSRem(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "srem_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_BIT_AND_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildAnd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "and_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_BIT_OR_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildOr(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "or_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_BIT_XOR_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildXor(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "xor_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_BIT_LSH_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildShl(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "shl_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_BIT_RSH_ASSIGN:
  {
    LLVMValueRef llvm_tmp_value = LLVMBuildLShr(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "lshr_tmp");
    LLVMBuildStore(ctx->llvm_builder, llvm_tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  default:
    unreachable();
  }
}

void ast_expr_op_bin_dump_json(FILE* stream, ast_expr_op_bin_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"lhs\":");
  ast_node_dump_json(stream, node->lhs);
  fprintf(stream, ",\"rhs\":");
  ast_node_dump_json(stream, node->rhs);
  fputc('}', stream);
}
