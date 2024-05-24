/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un.h"

#include "ast/registry.h"
#include "stages/codegen/utils.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_un_t* ast_expr_op_un_init(void)
{
  ast_expr_op_un_t* node = (ast_expr_op_un_t*)malloc(sizeof(ast_expr_op_un_t));
  CLEAROBJ(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_UNARY;

  return node;
}

void ast_expr_op_un_free(ast_expr_op_un_t* node)
{
  free(node);
}

void ast_expr_op_un_nameres(nameres_ctx_t* ctx, ast_expr_op_un_t* node)
{
  ast_node_nameres(ctx, node->expr);
}

void ast_expr_op_un_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_t* node)
{
  ast_node_typecheck(ctx, node->expr);

  typedesc_t* expr_desc = typetable_lookup(ctx->typetable, node->expr);
  ASSERT(expr_desc != NULL);

  typedesc_t* desc = NULL;

  switch (node->op_kind)
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
  {
    desc = typebuilder_build_usize(ctx->typebuilder);
    break;
  }
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
  {
    if (expr_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    if (typedesc_remove_ref(expr_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->expr->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    desc = expr_desc;
    break;
  }
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
  {
    if (expr_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    if (typedesc_remove_ref(expr_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->expr->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    desc = typedesc_remove_ref_mut(expr_desc);
    break;
  }
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
  {
    if (!typedesc_is_arithmetic(typedesc_remove_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    desc = typedesc_remove_ref_mut(expr_desc);
    break;
  }
  case OP_LOGIC_NOT:
  {
    if (typedesc_remove_ref_mut(expr_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->expr->tok->loc);

    desc = typedesc_remove_ref_mut(expr_desc);
    break;
  }
  case OP_IND:
  {
    if (typedesc_remove_ref_mut(expr_desc)->kind != TYPEDESC_PTR)
      report_error_expected_pointer_type(node->expr->tok->loc);
    
    typedesc_t* pointed_desc = ((typedesc_ptr_t*)typedesc_remove_ref_mut(expr_desc))->base_type;

    desc = typebuilder_build_ref(ctx->typebuilder, pointed_desc);
    break;
  }
  case OP_ADDR:
  {
    if (expr_desc->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    desc = typebuilder_build_ptr(ctx->typebuilder, typedesc_remove_ref(expr_desc));
    break;
  }
  case OP_UNWRAP_SAFE:
  case OP_UNWRAP_UNSAFE:
  {
    if (typedesc_remove_ref_mut(expr_desc)->kind != TYPEDESC_OPT)
      report_error_expected_optional_type(node->expr->tok->loc);
    
    typedesc_opt_t* opt_desc = (typedesc_opt_t*)typedesc_remove_ref_mut(expr_desc);

    desc = opt_desc->base_type;
    break;
  }
  default:
    UNREACHABLE();
  }

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_codegen(codegen_ctx_t* ctx, ast_expr_op_un_t* node)
{
  ast_node_codegen(ctx, node->expr);

  typedesc_t* desc = typetable_lookup(ctx->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  typedesc_t* expr_desc = typedesc_remove_ref_mut(typetable_lookup(ctx->typetable, node->expr));

  switch (node->op_kind)
  {
  case OP_SIZEOF:
  {
    uint64_t value = LLVMABISizeOfType(ctx->llvm_layout, expr->llvm_type);
    node->llvm_value = LLVMConstInt(node->llvm_type, value, false);
    break;
  }
  case OP_ALIGNOF:
  {
    uint64_t value = LLVMABIAlignmentOfType(ctx->llvm_layout, expr->llvm_type);
    node->llvm_value = LLVMConstInt(node->llvm_type, value, false);
    break;
  }
  case OP_ARIT_INC_PRE:
  {
    LLVMValueRef llvm_value = LLVMBuildLoad2(ctx->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
    LLVMValueRef llvm_inc_value = NULL;

    if (typedesc_is_integer(expr_desc))
      llvm_inc_value = LLVMBuildAdd(ctx->llvm_builder, llvm_value, LLVMConstInt(expr->llvm_type, 1, false), "pre_inc_tmp");
    else if (typedesc_is_float(expr_desc))
      llvm_inc_value = LLVMBuildFAdd(ctx->llvm_builder, llvm_value, LLVMConstReal(expr->llvm_type, 1.0), "pre_inc_tmp");
    else
      UNREACHABLE();

    LLVMBuildStore(ctx->llvm_builder, llvm_inc_value, expr->llvm_value);    
    node->llvm_value = expr->llvm_value;
    break;
  }
  case OP_ARIT_INC_POST:
  {
    LLVMValueRef llvm_value = LLVMBuildLoad2(ctx->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
    LLVMValueRef llvm_inc_value = NULL;

    if (typedesc_is_integer(expr_desc))
      llvm_inc_value = LLVMBuildAdd(ctx->llvm_builder, llvm_value, LLVMConstInt(expr->llvm_type, 1, false), "post_inc_tmp");
    else if (typedesc_is_float(expr_desc))
      llvm_inc_value = LLVMBuildFAdd(ctx->llvm_builder, llvm_value, LLVMConstReal(expr->llvm_type, 1.0), "post_inc_tmp");
    else
      UNREACHABLE();
    
    LLVMBuildStore(ctx->llvm_builder, llvm_inc_value, expr->llvm_value);
    node->llvm_value = llvm_value;
    break;
  }
  case OP_ARIT_DEC_PRE:
  {
    LLVMValueRef llvm_value = LLVMBuildLoad2(ctx->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
    LLVMValueRef llvm_dec_value = NULL;

    if (typedesc_is_integer(expr_desc))
      llvm_dec_value = LLVMBuildSub(ctx->llvm_builder, llvm_value, LLVMConstInt(expr->llvm_type, 1, false), "pre_dec_tmp");
    else if (typedesc_is_float(expr_desc))
      llvm_dec_value = LLVMBuildFSub(ctx->llvm_builder, llvm_value, LLVMConstReal(expr->llvm_type, 1.0), "pre_dec_tmp");
    else
      UNREACHABLE();

    LLVMBuildStore(ctx->llvm_builder, llvm_dec_value, expr->llvm_value);
    node->llvm_value = expr->llvm_value;
    break;
  }
  case OP_ARIT_DEC_POST:
  {
    LLVMValueRef llvm_value = LLVMBuildLoad2(ctx->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
    LLVMValueRef llvm_dec_value = NULL;

    if (typedesc_is_integer(expr_desc))
      llvm_dec_value = LLVMBuildSub(ctx->llvm_builder, llvm_value, LLVMConstInt(expr->llvm_type, 1, false), "post_dec_tmp");
    else if (typedesc_is_float(expr_desc))
      llvm_dec_value = LLVMBuildFSub(ctx->llvm_builder, llvm_value, LLVMConstReal(expr->llvm_type, 1.0), "post_dec_tmp");
    else
      UNREACHABLE();

    LLVMBuildStore(ctx->llvm_builder, llvm_dec_value, expr->llvm_value);
    node->llvm_value = llvm_value;
    break;
  }
  case OP_ARIT_POS:
  {
    node->llvm_value = codegen_build_load_if_ref(ctx, expr);
    break;
  }
  case OP_ARIT_NEG:
  {
    LLVMValueRef llvm_value = codegen_build_load_if_ref(ctx, expr);

    if (typedesc_is_integer(expr_desc))
      node->llvm_value = LLVMBuildNeg(ctx->llvm_builder, llvm_value, "neg_tmp");
    else if (typedesc_is_float(expr_desc))
      node->llvm_value = LLVMBuildFNeg(ctx->llvm_builder, llvm_value, "fneg_tmp");
    else
      UNREACHABLE();

    break;
  }
  case OP_BIT_NOT:
  case OP_LOGIC_NOT:
  {
    LLVMValueRef llvm_value = codegen_build_load_if_ref(ctx, expr);
    node->llvm_value = LLVMBuildNot(ctx->llvm_builder, llvm_value, "not_tmp");
    break;
  }
  case OP_IND:
  {
    node->llvm_value = LLVMBuildLoad2(ctx->llvm_builder, ((typedesc_ptr_t*)expr_desc)->base_type->llvm_type, expr->llvm_value, "load2_tmp");
    break;
  }
  case OP_ADDR:
  {
    node->llvm_value = expr->llvm_value;
    break;
  }
  case OP_UNWRAP_SAFE:
  {
    LLVMBasicBlockRef llvm_cond_block = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "unwrap_safe_cond");
    LLVMBasicBlockRef llvm_exit_block = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "unwrap_safe_exit");
    LLVMBasicBlockRef llvm_end_block = LLVMCreateBasicBlockInContext(ctx->llvm_ctx, "unwrap_safe_end");

    LLVMBuildBr(ctx->llvm_builder, llvm_cond_block);

    LLVMInsertExistingBasicBlockAfterInsertBlock(ctx->llvm_builder, llvm_cond_block);
    LLVMPositionBuilderAtEnd(ctx->llvm_builder, llvm_cond_block);

    LLVMValueRef llvm_flag_ptr = LLVMBuildStructGEP2(ctx->llvm_builder, expr_desc->llvm_type, expr->llvm_value, 0, "gep2_tmp");
    LLVMValueRef llvm_flag_value = LLVMBuildLoad2(ctx->llvm_builder, LLVMInt1TypeInContext(ctx->llvm_ctx), llvm_flag_ptr, "load2_tmp");
    LLVMValueRef llvm_cond_value = LLVMBuildICmp(ctx->llvm_builder, LLVMIntEQ, llvm_flag_value, LLVMConstInt(LLVMInt1TypeInContext(ctx->llvm_ctx), 0, false), "icmp_eq_tmp");

    LLVMBuildCondBr(ctx->llvm_builder, llvm_cond_value, llvm_exit_block, llvm_end_block);

    LLVMTypeRef llvm_param_types[] = {
      LLVMInt32TypeInContext(ctx->llvm_ctx)
    };

    LLVMTypeRef llvm_exit_type = LLVMFunctionType(LLVMVoidTypeInContext(ctx->llvm_ctx), llvm_param_types, COUNTOF(llvm_param_types), false);
    LLVMValueRef llvm_exit_func = LLVMAddFunction(ctx->llvm_mod, "exit", llvm_exit_type);
    LLVMSetFunctionCallConv(llvm_exit_func, LLVMCCallConv);

    LLVMInsertExistingBasicBlockAfterInsertBlock(ctx->llvm_builder, llvm_exit_block);
    LLVMPositionBuilderAtEnd(ctx->llvm_builder, llvm_exit_block);
    
    LLVMValueRef llvm_args[] = {
      LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx), 1, false)
    };

    LLVMBuildCall2(ctx->llvm_builder, llvm_exit_type, llvm_exit_func, llvm_args, COUNTOF(llvm_args), "");
    LLVMBuildUnreachable(ctx->llvm_builder);

    LLVMInsertExistingBasicBlockAfterInsertBlock(ctx->llvm_builder, llvm_end_block);
    LLVMPositionBuilderAtEnd(ctx->llvm_builder, llvm_end_block);

    LLVMValueRef llvm_value_ptr = LLVMBuildStructGEP2(ctx->llvm_builder, expr_desc->llvm_type, expr->llvm_value, 1, "gep2_tmp");
    node->llvm_value = LLVMBuildLoad2(ctx->llvm_builder, ((typedesc_opt_t*)expr_desc)->base_type->llvm_type, llvm_value_ptr, "load2_tmp");
    break;
  }
  case OP_UNWRAP_UNSAFE:
  {
    LLVMValueRef llvm_value_ptr = LLVMBuildStructGEP2(ctx->llvm_builder, expr_desc->llvm_type, expr->llvm_value, 1, "gep2_tmp");
    node->llvm_value = LLVMBuildLoad2(ctx->llvm_builder, ((typedesc_opt_t*)expr_desc)->base_type->llvm_type, llvm_value_ptr, "load2_tmp");
    break;
  }
  default:
    UNREACHABLE();
  }
}

void ast_expr_op_un_dump_json(FILE* stream, ast_expr_op_un_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
