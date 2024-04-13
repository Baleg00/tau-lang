/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/bin.h"

#include "ast/registry.h"
#include "stages/codegen/utils.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_bin_t* ast_expr_op_bin_init(void)
{
  ast_expr_op_bin_t* node = (ast_expr_op_bin_t*)malloc(sizeof(ast_expr_op_bin_t));
  CLEAROBJ(node);

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
  if (node->op_kind == OP_ACCESS)
  {
    ast_expr_op_bin_access_nameres(ctx, (ast_expr_op_bin_access_t*)node);
    return;
  }

  ast_node_nameres(ctx, node->lhs);
  ast_node_nameres(ctx, node->rhs);
}

void ast_expr_op_bin_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node)
{
  if (node->op_kind == OP_ACCESS)
  {
    ast_expr_op_bin_access_typecheck(ctx, (ast_expr_op_bin_access_t*)node);
    return;
  }

  ast_node_typecheck(ctx, node->lhs);
  ast_node_typecheck(ctx, node->rhs);

  typedesc_t* lhs_desc = typetable_lookup(ctx->typetable, node->lhs);
  ASSERT(lhs_desc != NULL);

  typedesc_t* rhs_desc = typetable_lookup(ctx->typetable, node->rhs);
  ASSERT(rhs_desc != NULL);

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
    if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)))
      report_error_expected_arithmetic_type(node->lhs->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
      report_error_expected_arithmetic_type(node->rhs->tok->loc);

    if (typedesc_is_signed(typedesc_remove_ref_mut(lhs_desc)) != typedesc_is_signed(typedesc_remove_ref_mut(rhs_desc)))
      report_warning_mixed_signedness(node->tok->loc);

    desc = typedesc_arithmetic_promote(typedesc_remove_ref_mut(lhs_desc), typedesc_remove_ref_mut(rhs_desc));
    break;
  }
  case OP_BIT_LSH:
  case OP_BIT_RSH:
  {
    if (!typedesc_is_integer(typedesc_remove_ref_mut(lhs_desc)))
      report_error_expected_integer_type(node->lhs->tok->loc);

    if (!typedesc_is_integer(typedesc_remove_ref_mut(rhs_desc)))
      report_error_expected_integer_type(node->rhs->tok->loc);

    desc = typedesc_remove_ref_mut(lhs_desc);
    break;
  }
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
  {
    if (typedesc_remove_ref_mut(lhs_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->lhs->tok->loc);

    if (typedesc_remove_ref_mut(rhs_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->rhs->tok->loc);

    desc = typebuilder_build_bool(ctx->typebuilder);
    break;
  }
  case OP_CMP_EQ:
  case OP_CMP_NE:
  case OP_CMP_LT:
  case OP_CMP_LE:
  case OP_CMP_GT:
  case OP_CMP_GE:
  {
    if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(lhs_desc)))
      report_error_expected_arithmetic_type(node->lhs->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(rhs_desc)))
      report_error_expected_arithmetic_type(node->rhs->tok->loc);

    desc = typebuilder_build_bool(ctx->typebuilder);
    break;
  }
  case OP_ASSIGN:
  {
    if (lhs_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->lhs->tok->loc);

    if (typedesc_remove_ref(lhs_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->lhs->tok->loc);

    if (typedesc_remove_ref_mut(lhs_desc) != typedesc_remove_ref_mut(rhs_desc))
      report_error_type_mismatch(node->lhs->tok->loc, lhs_desc, rhs_desc);

    desc = lhs_desc;
    break;
  }
  case OP_SUBS:
  {
    if (typedesc_remove_ref_mut(lhs_desc)->kind != TYPEDESC_ARRAY)
      report_error_expected_pointer_type(node->lhs->tok->loc);

    if (!typedesc_is_integer(typedesc_remove_ref_mut(rhs_desc)))
      report_error_expected_integer_type(node->rhs->tok->loc);

    typedesc_array_t* array_desc = (typedesc_array_t*)typedesc_remove_ref_mut(lhs_desc);
    desc = typebuilder_build_ref(ctx->typebuilder, array_desc->base_type);
    break;
  }
  default:
    UNREACHABLE();
  }

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_bin_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node)
{
  if (node->op_kind == OP_ACCESS)
  {
    ast_expr_op_bin_access_codegen(ctx, (ast_expr_op_bin_access_t*)node);
    return;
  }

  ast_node_codegen(ctx, node->lhs);
  ast_node_codegen(ctx, node->rhs);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  typedesc_t* lhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->lhs));
  typedesc_t* rhs_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->rhs));

  LLVMValueRef llvm_lhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->lhs);
  LLVMValueRef llvm_rhs_value = codegen_build_load_if_ref(ctx, (ast_expr_t*)node->rhs);

  typedesc_t* promoted_desc = NULL;

  if (op_is_arithmetic(node->op_kind) || op_is_comparison(node->op_kind))
  {
    promoted_desc = typedesc_arithmetic_promote(lhs_desc, rhs_desc);

    llvm_lhs_value = codegen_build_arithmetic_cast(ctx, llvm_lhs_value, lhs_desc, promoted_desc);
    llvm_rhs_value = codegen_build_arithmetic_cast(ctx, llvm_rhs_value, rhs_desc, promoted_desc);
  }

  switch (node->op_kind)
  {
  case OP_ARIT_ADD:
  case OP_ASSIGN_ARIT_ADD:
  {
    if (typedesc_is_integer(desc))
      node->llvm_value = LLVMBuildAdd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "add_tmp");
    else if (typedesc_is_float(desc))
      node->llvm_value = LLVMBuildFAdd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "fadd_tmp");
    else
      UNREACHABLE();
    
    break;
  }
  case OP_ARIT_SUB:
  case OP_ASSIGN_ARIT_SUB:
  {
    if (typedesc_is_integer(desc))
      node->llvm_value = LLVMBuildSub(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "sub_tmp");
    else if (typedesc_is_float(desc))
      node->llvm_value = LLVMBuildFSub(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "fsub_tmp");
    else
      UNREACHABLE();
    
    break;
  }
  case OP_ARIT_MUL:
  case OP_ASSIGN_ARIT_MUL:
  {
    if (typedesc_is_integer(desc))
      node->llvm_value = LLVMBuildMul(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "mul_tmp");
    else if (typedesc_is_float(desc))
      node->llvm_value = LLVMBuildFMul(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "fmul_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_ARIT_DIV:
  case OP_ASSIGN_ARIT_DIV:
  {
    if (typedesc_is_integer(desc))
      if (typedesc_is_signed(desc))
        node->llvm_value = LLVMBuildSDiv(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "sdiv_tmp");
      else
        node->llvm_value = LLVMBuildUDiv(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "udiv_tmp");
    else if (typedesc_is_float(desc))
      node->llvm_value = LLVMBuildFDiv(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "fdiv_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_ARIT_MOD:
  case OP_ASSIGN_ARIT_MOD:
  {
    if (typedesc_is_integer(desc))
      if (typedesc_is_signed(desc))
        node->llvm_value = LLVMBuildSRem(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "srem_tmp");
      else
        node->llvm_value = LLVMBuildURem(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "urem_tmp");
    else if (typedesc_is_float(desc))
      node->llvm_value = LLVMBuildFRem(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "frem_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_BIT_AND:
  case OP_LOGIC_AND:
  case OP_ASSIGN_BIT_AND:
  {
    node->llvm_value = LLVMBuildAnd(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "and_tmp");
    break;
  }
  case OP_BIT_OR:
  case OP_LOGIC_OR:
  case OP_ASSIGN_BIT_OR:
  {
    node->llvm_value = LLVMBuildOr(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "or_tmp");
    break;
  }
  case OP_BIT_XOR:
  case OP_ASSIGN_BIT_XOR:
  {
    node->llvm_value = LLVMBuildXor(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "xor_tmp");
    break;
  }
  case OP_BIT_LSH:
  case OP_ASSIGN_BIT_LSH:
  {
    node->llvm_value = LLVMBuildShl(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "shl_tmp");
    break;
  }
  case OP_BIT_RSH:
  case OP_ASSIGN_BIT_RSH:
  {
    node->llvm_value = LLVMBuildLShr(ctx->llvm_builder, llvm_lhs_value, llvm_rhs_value, "lshr_tmp");
    break;
  }
  case OP_CMP_EQ:
  {
    if (typedesc_is_integer(promoted_desc))
      node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntEQ, llvm_lhs_value, llvm_rhs_value, "icmp_eq_tmp");
    else if (typedesc_is_float(promoted_desc))
      node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOEQ, llvm_lhs_value, llvm_rhs_value, "fcmp_oeq_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_CMP_NE:
  {
    if (typedesc_is_integer(promoted_desc))
      node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntNE, llvm_lhs_value, llvm_rhs_value, "icmp_ne_tmp");
    else if (typedesc_is_float(promoted_desc))
      node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealONE, llvm_lhs_value, llvm_rhs_value, "fcmp_one_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_CMP_LT:
  {
    if (typedesc_is_integer(promoted_desc))
      if (typedesc_is_signed(promoted_desc))
        node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSLT, llvm_lhs_value, llvm_rhs_value, "icmp_slt_tmp");
      else
        node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntULT, llvm_lhs_value, llvm_rhs_value, "icmp_ult_tmp");
    else if (typedesc_is_float(promoted_desc))
      node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOLT, llvm_lhs_value, llvm_rhs_value, "fcmp_olt_tmp");
    else
      UNREACHABLE();
    
    break;
  }
  case OP_CMP_LE:
  {
    if (typedesc_is_integer(promoted_desc))
      if (typedesc_is_signed(promoted_desc))
        node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSLE, llvm_lhs_value, llvm_rhs_value, "icmp_sle_tmp");
      else
        node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntULE, llvm_lhs_value, llvm_rhs_value, "icmp_ule_tmp");
    else if (typedesc_is_float(promoted_desc))
      node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOLE, llvm_lhs_value, llvm_rhs_value, "fcmp_ole_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_CMP_GT:
  {
    if (typedesc_is_integer(promoted_desc))
      if (typedesc_is_signed(promoted_desc))
        node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSGT, llvm_lhs_value, llvm_rhs_value, "icmp_sgt_tmp");
      else
        node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntUGT, llvm_lhs_value, llvm_rhs_value, "icmp_ugt_tmp");
    else if (typedesc_is_float(promoted_desc))
      node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOGT, llvm_lhs_value, llvm_rhs_value, "fcmp_ogt_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_CMP_GE:
  {
    if (typedesc_is_integer(promoted_desc))
      if (typedesc_is_signed(promoted_desc))
        node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntSGE, llvm_lhs_value, llvm_rhs_value, "icmp_sge_tmp");
      else
        node->llvm_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntUGE, llvm_lhs_value, llvm_rhs_value, "icmp_uge_tmp");
    else if (typedesc_is_float(promoted_desc))
      node->llvm_value = LLVMBuildFCmp(ctx->llvm_builder, LLVMRealOGE, llvm_lhs_value, llvm_rhs_value, "fcmp_oge_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_ASSIGN:
  {
    LLVMBuildStore(ctx->llvm_builder, llvm_rhs_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  case OP_SUBS:
  {
    LLVMValueRef llvm_ptr_value = ((ast_expr_t*)node->lhs)->llvm_value;
    LLVMTypeRef llvm_base_type = ((typedesc_array_t*)lhs_desc)->base_type->llvm_type;
    node->llvm_value = LLVMBuildGEP2(ctx->llvm_builder, llvm_base_type, llvm_ptr_value, &llvm_rhs_value, 1, "gep2_tmp");
    break;
  }
  default:
    UNREACHABLE();
  }

  switch (node->op_kind)
  {
  case OP_ASSIGN_ARIT_ADD:
  case OP_ASSIGN_ARIT_SUB:
  case OP_ASSIGN_ARIT_MUL:
  case OP_ASSIGN_ARIT_DIV:
  case OP_ASSIGN_ARIT_MOD:
  case OP_ASSIGN_BIT_AND:
  case OP_ASSIGN_BIT_OR:
  case OP_ASSIGN_BIT_XOR:
  case OP_ASSIGN_BIT_LSH:
  case OP_ASSIGN_BIT_RSH:
  {
    LLVMBuildStore(ctx->llvm_builder, node->llvm_value, ((ast_expr_t*)node->lhs)->llvm_value);
    node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
    break;
  }
  default:
    NOOP();
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
