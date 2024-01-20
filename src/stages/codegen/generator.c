/**
 * \file generator.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/codegen/generator.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/Transforms/PassBuilder.h>

#include "stages/lexer/location.h"
#include "stages/lexer/token.h"
#include "utils/common.h"
#include "utils/memory/memtrace.h"

struct generator_t
{
  typetable_t* typetable; // The type table for resolving types.
  LLVMContextRef llvm_context; // The LLVM context for code generation.
  LLVMModuleRef llvm_module;   // The LLVM module for the current translation unit.
  LLVMBuilderRef llvm_builder; // The LLVM IR builder for generating code.
  LLVMTargetMachineRef llvm_machine; // The LLVM target machine.
  LLVMTargetDataRef llvm_layout; // The LLVM target data layout.
};

static LLVMValueRef generator_build_load_if_ref(generator_t* gen, ast_expr_t* node)
{
  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);

  if (typedesc_remove_const(desc)->kind == TYPEDESC_REF)
    return LLVMBuildLoad2(gen->llvm_builder, node->llvm_type, node->llvm_value, "load_tmp");

  return node->llvm_value;
}

generator_t* generator_init(void)
{
  generator_t* gen = (generator_t*)malloc(sizeof(generator_t));
  memset(gen, 0, sizeof(generator_t));
  return gen;
}

void generator_free(generator_t* gen)
{
  free(gen);
}

void generator_visit_type_mut(generator_t* gen, ast_type_mut_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->base_type);

  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
}

void generator_visit_type_ptr(generator_t* gen, ast_type_ptr_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->base_type);

  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
}

void generator_visit_type_array(generator_t* gen, ast_type_array_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->base_type);

  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
}

void generator_visit_type_fun(generator_t* gen, ast_type_fun_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->return_type);

  typedesc_t* fun_desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = fun_desc->llvm_type;

  typedesc_t* return_desc = typetable_lookup(gen->typetable, (ast_node_t*)node->return_type);
  ((ast_type_t*)node->return_type)->llvm_type = return_desc->llvm_type;

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_type_t* param_type_node = (ast_type_t*)vector_get(node->params, i);

    generator_visit_type(gen, param_type_node);

    typedesc_t* param_desc = typetable_lookup(gen->typetable, (ast_node_t*)param_type_node);
    param_type_node->llvm_type = param_desc->llvm_type;
  }
}

void generator_visit_type_id(generator_t* gen, ast_type_id_t* node)
{
  switch (node->decl->kind)
  {
  case AST_DECL_STRUCT: node->llvm_type = ((ast_decl_struct_t*)node->decl)->llvm_type; break;
  default: unreachable();
  }
}

void generator_visit_type(generator_t* gen, ast_type_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_ID:    generator_visit_type_id   (gen, (ast_type_id_t*   )node); break;
  case AST_TYPE_MUT:   generator_visit_type_mut  (gen, (ast_type_mut_t*  )node); break;
  case AST_TYPE_PTR:   generator_visit_type_ptr  (gen, (ast_type_ptr_t*  )node); break;
  case AST_TYPE_ARRAY: generator_visit_type_array(gen, (ast_type_array_t*)node); break;
  case AST_TYPE_FUN:   generator_visit_type_fun  (gen, (ast_type_fun_t*  )node); break;
  case AST_TYPE_PRIM_I8:
  case AST_TYPE_PRIM_I16:
  case AST_TYPE_PRIM_I32:
  case AST_TYPE_PRIM_I64:
  case AST_TYPE_PRIM_ISIZE:
  case AST_TYPE_PRIM_U8:
  case AST_TYPE_PRIM_U16:
  case AST_TYPE_PRIM_U32:
  case AST_TYPE_PRIM_U64:
  case AST_TYPE_PRIM_USIZE:
  case AST_TYPE_PRIM_F32:
  case AST_TYPE_PRIM_F64:
  case AST_TYPE_PRIM_BOOL:
  case AST_TYPE_PRIM_UNIT:
  {
    typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
    node->llvm_type = desc->llvm_type;
    break;
  }
  default: unreachable();
  }
}

void generator_visit_expr_lit_int(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
  node->llvm_value = LLVMConstIntOfStringAndSize(node->llvm_type, node->tok->loc->ptr, (uint32_t)node->tok->loc->len, 10);
}

void generator_visit_expr_lit_flt(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
  node->llvm_value = LLVMConstRealOfStringAndSize(node->llvm_type, node->tok->loc->ptr, (uint32_t)node->tok->loc->len);
}

void generator_visit_expr_lit_str(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  size_t lit_len = 0;

  for (char* ch = (char*)node->tok->loc->ptr + 1; *ch != '"'; ch++, lit_len++)
    if (*ch == '\\')
    {
      ch++;

      switch (*ch)
      {
      case '\\':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
      case '\'':
      case '"':
        break;
      default:
        unreachable();
      }
    }

  char* lit_value = malloc(lit_len + 1);

  for (char *ch = (char*)node->tok->loc->ptr + 1, *ptr = lit_value; *ch != '"'; ch++, ptr++)
    if (*ch == '\\')
    {
      ch++;

      switch (*ch)
      {
      case '\\': *ptr = '\\'; break;
      case 'b':  *ptr = '\b'; break;
      case 'f':  *ptr = '\f'; break;
      case 'n':  *ptr = '\n'; break;
      case 'r':  *ptr = '\r'; break;
      case 't':  *ptr = '\t'; break;
      case '\'': *ptr = '\''; break;
      case '"':  *ptr = '"';  break;
      default: unreachable();
      }
    }
    else
      *ptr = *ch;

  lit_value[lit_len] = '\0';

  node->llvm_value = LLVMBuildGlobalStringPtr(gen->llvm_builder, lit_value, "global_str");

  free(lit_value);

  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
}

void generator_visit_expr_lit_bool(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_lit_t* node)
{
  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  bool lit_value = strncmp("true", node->tok->loc->ptr, node->tok->loc->len) == 0;
  node->llvm_value = LLVMConstInt(node->llvm_type, (uint32_t)lit_value, false);
}

void generator_visit_expr_op_un(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  switch (node->op_kind)
  {
  case OP_ARIT_INC_PRE:  generator_visit_expr_op_un_arit_inc_pre (gen, fun_node, node); break;
  case OP_ARIT_INC_POST: generator_visit_expr_op_un_arit_inc_post(gen, fun_node, node); break;
  case OP_ARIT_DEC_PRE:  generator_visit_expr_op_un_arit_dec_pre (gen, fun_node, node); break;
  case OP_ARIT_DEC_POST: generator_visit_expr_op_un_arit_dec_post(gen, fun_node, node); break;
  case OP_ARIT_POS:      generator_visit_expr_op_un_arit_pos     (gen, fun_node, node); break;
  case OP_ARIT_NEG:      generator_visit_expr_op_un_arit_neg     (gen, fun_node, node); break;
  case OP_LOGIC_NOT:     generator_visit_expr_op_un_logic_not    (gen, fun_node, node); break;
  case OP_BIT_NOT:       generator_visit_expr_op_un_bit_not      (gen, fun_node, node); break;
  default: unreachable();
  }
}

void generator_visit_expr_op_un_arit_inc_pre(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  LLVMValueRef value = LLVMBuildLoad2(gen->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
  LLVMValueRef one = LLVMConstInt(expr->llvm_type, 1, false);
  LLVMValueRef inc_value = LLVMBuildAdd(gen->llvm_builder, value, one, "pre_inc_tmp");
  LLVMBuildStore(gen->llvm_builder, inc_value, expr->llvm_value);

  node->llvm_type = expr->llvm_type;
  node->llvm_value = expr->llvm_value;
}

void generator_visit_expr_op_un_arit_inc_post(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  LLVMValueRef value = LLVMBuildLoad2(gen->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
  LLVMValueRef one = LLVMConstInt(expr->llvm_type, 1, false);
  LLVMValueRef inc_value = LLVMBuildAdd(gen->llvm_builder, value, one, "post_inc_tmp");
  LLVMBuildStore(gen->llvm_builder, inc_value, expr->llvm_value);

  node->llvm_type = expr->llvm_type;
  node->llvm_value = value;
}

void generator_visit_expr_op_un_arit_dec_pre(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  LLVMValueRef value = LLVMBuildLoad2(gen->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
  LLVMValueRef one = LLVMConstInt(expr->llvm_type, 1, false);
  LLVMValueRef dec_value = LLVMBuildSub(gen->llvm_builder, value, one, "pre_dec_tmp");
  LLVMBuildStore(gen->llvm_builder, dec_value, expr->llvm_value);

  node->llvm_type = expr->llvm_type;
  node->llvm_value = expr->llvm_value;
}

void generator_visit_expr_op_un_arit_dec_post(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  ast_expr_t* expr = (ast_expr_t*)node->expr;

  LLVMValueRef value = LLVMBuildLoad2(gen->llvm_builder, expr->llvm_type, expr->llvm_value, "load_tmp");
  LLVMValueRef one = LLVMConstInt(expr->llvm_type, 1, false);
  LLVMValueRef dec_value = LLVMBuildSub(gen->llvm_builder, value, one, "post_dec_tmp");
  LLVMBuildStore(gen->llvm_builder, dec_value, expr->llvm_value);

  node->llvm_type = expr->llvm_type;
  node->llvm_value = value;
}

void generator_visit_expr_op_un_arit_pos(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  node->llvm_type = ((ast_expr_t*)node->expr)->llvm_type;
  node->llvm_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->expr);
}

void generator_visit_expr_op_un_arit_neg(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  node->llvm_type = ((ast_expr_t*)node->expr)->llvm_type;
  LLVMValueRef value = generator_build_load_if_ref(gen, (ast_expr_t*)node->expr);
  node->llvm_value = LLVMBuildNeg(gen->llvm_builder, value, "neg_tmp");
}

void generator_visit_expr_op_un_logic_not(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  node->llvm_type = ((ast_expr_t*)node->expr)->llvm_type;
  LLVMValueRef value = generator_build_load_if_ref(gen, (ast_expr_t*)node->expr);
  node->llvm_value = LLVMBuildNot(gen->llvm_builder, value, "not_tmp");
}

void generator_visit_expr_op_un_bit_not(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_un_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  node->llvm_type = ((ast_expr_t*)node->expr)->llvm_type;
  LLVMValueRef value = generator_build_load_if_ref(gen, (ast_expr_t*)node->expr);
  node->llvm_value = LLVMBuildNot(gen->llvm_builder, value, "not_tmp");
}

void generator_visit_expr_op_bin(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  switch (node->op_kind)
  {
  case OP_ARIT_ADD:        generator_visit_expr_op_bin_arit_add       (gen, fun_node, node); break;
  case OP_ARIT_SUB:        generator_visit_expr_op_bin_arit_sub       (gen, fun_node, node); break;
  case OP_ARIT_MUL:        generator_visit_expr_op_bin_arit_mul       (gen, fun_node, node); break;
  case OP_ARIT_DIV:        generator_visit_expr_op_bin_arit_div       (gen, fun_node, node); break;
  case OP_ARIT_MOD:        generator_visit_expr_op_bin_arit_mod       (gen, fun_node, node); break;
  case OP_BIT_AND:         generator_visit_expr_op_bin_bit_and        (gen, fun_node, node); break;
  case OP_BIT_OR:          generator_visit_expr_op_bin_bit_or         (gen, fun_node, node); break;
  case OP_BIT_XOR:         generator_visit_expr_op_bin_bit_xor        (gen, fun_node, node); break;
  case OP_BIT_LSH:         generator_visit_expr_op_bin_bit_lsh        (gen, fun_node, node); break;
  case OP_BIT_RSH:         generator_visit_expr_op_bin_bit_rsh        (gen, fun_node, node); break;
  case OP_LOGIC_AND:       generator_visit_expr_op_bin_logic_and      (gen, fun_node, node); break;
  case OP_LOGIC_OR:        generator_visit_expr_op_bin_logic_or       (gen, fun_node, node); break;
  case OP_COMP_EQ:         generator_visit_expr_op_bin_comp_eq        (gen, fun_node, node); break;
  case OP_COMP_NE:         generator_visit_expr_op_bin_comp_ne        (gen, fun_node, node); break;
  case OP_COMP_LT:         generator_visit_expr_op_bin_comp_lt        (gen, fun_node, node); break;
  case OP_COMP_LE:         generator_visit_expr_op_bin_comp_le        (gen, fun_node, node); break;
  case OP_COMP_GT:         generator_visit_expr_op_bin_comp_gt        (gen, fun_node, node); break;
  case OP_COMP_GE:         generator_visit_expr_op_bin_comp_ge        (gen, fun_node, node); break;
  case OP_ASSIGN:          generator_visit_expr_op_bin_assign         (gen, fun_node, node); break;
  case OP_ARIT_ADD_ASSIGN: generator_visit_expr_op_bin_arit_add_assign(gen, fun_node, node); break;
  case OP_ARIT_SUB_ASSIGN: generator_visit_expr_op_bin_arit_sub_assign(gen, fun_node, node); break;
  case OP_ARIT_MUL_ASSIGN: generator_visit_expr_op_bin_arit_mul_assign(gen, fun_node, node); break;
  case OP_ARIT_DIV_ASSIGN: generator_visit_expr_op_bin_arit_div_assign(gen, fun_node, node); break;
  case OP_ARIT_MOD_ASSIGN: generator_visit_expr_op_bin_arit_mod_assign(gen, fun_node, node); break;
  case OP_BIT_AND_ASSIGN:  generator_visit_expr_op_bin_bit_and_assign (gen, fun_node, node); break;
  case OP_BIT_OR_ASSIGN:   generator_visit_expr_op_bin_bit_or_assign  (gen, fun_node, node); break;
  case OP_BIT_XOR_ASSIGN:  generator_visit_expr_op_bin_bit_xor_assign (gen, fun_node, node); break;
  case OP_BIT_LSH_ASSIGN:  generator_visit_expr_op_bin_bit_lsh_assign (gen, fun_node, node); break;
  case OP_BIT_RSH_ASSIGN:  generator_visit_expr_op_bin_bit_rsh_assign (gen, fun_node, node); break;
  default: unreachable();
  }
}

void generator_visit_expr_op_bin_arit_add(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildAdd(gen->llvm_builder, lhs_value, rhs_value, "add_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_arit_sub(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildSub(gen->llvm_builder, lhs_value, rhs_value, "sub_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_arit_mul(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildMul(gen->llvm_builder, lhs_value, rhs_value, "mul_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_arit_div(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildSDiv(gen->llvm_builder, lhs_value, rhs_value, "sdiv_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_arit_mod(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildSRem(gen->llvm_builder, lhs_value, rhs_value, "srem_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_bit_and(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildAnd(gen->llvm_builder, lhs_value, rhs_value, "and_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_bit_or(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildOr(gen->llvm_builder, lhs_value, rhs_value, "or_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_bit_xor(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildXor(gen->llvm_builder, lhs_value, rhs_value, "xor_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_bit_lsh(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildShl(gen->llvm_builder, lhs_value, rhs_value, "shl_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_bit_rsh(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildLShr(gen->llvm_builder, lhs_value, rhs_value, "lshr_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_logic_and(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildAnd(gen->llvm_builder, lhs_value, rhs_value, "and_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_logic_or(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildOr(gen->llvm_builder, lhs_value, rhs_value, "or_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_comp_eq(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildICmp(gen->llvm_builder, LLVMIntEQ, lhs_value, rhs_value, "icmp_eq_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_comp_ne(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildICmp(gen->llvm_builder, LLVMIntNE, lhs_value, rhs_value, "icmp_ne_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_comp_lt(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildICmp(gen->llvm_builder, LLVMIntSLT, lhs_value, rhs_value, "icmp_slt_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_comp_le(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildICmp(gen->llvm_builder, LLVMIntSLE, lhs_value, rhs_value, "icmp_sle_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_comp_gt(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildICmp(gen->llvm_builder, LLVMIntSGT, lhs_value, rhs_value, "icmp_sgt_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_comp_ge(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  node->llvm_value = LLVMBuildICmp(gen->llvm_builder, LLVMIntSGE, lhs_value, rhs_value, "icmp_sge_tmp");
  node->llvm_type = LLVMTypeOf(node->llvm_value);
}

void generator_visit_expr_op_bin_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMBuildStore(gen->llvm_builder, rhs_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_arit_add_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildAdd(gen->llvm_builder, lhs_value, rhs_value, "add_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_arit_sub_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildSub(gen->llvm_builder, lhs_value, rhs_value, "sub_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_arit_mul_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildMul(gen->llvm_builder, lhs_value, rhs_value, "mul_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_arit_div_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildSDiv(gen->llvm_builder, lhs_value, rhs_value, "sdiv_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_arit_mod_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildSRem(gen->llvm_builder, lhs_value, rhs_value, "srem_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_bit_and_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildAnd(gen->llvm_builder, lhs_value, rhs_value, "and_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_bit_or_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildOr(gen->llvm_builder, lhs_value, rhs_value, "or_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_bit_xor_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildXor(gen->llvm_builder, lhs_value, rhs_value, "xor_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_bit_lsh_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildShl(gen->llvm_builder, lhs_value, rhs_value, "shl_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_bin_bit_rsh_assign(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_bin_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->lhs);
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->rhs);

  LLVMValueRef lhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->lhs);
  LLVMValueRef rhs_value = generator_build_load_if_ref(gen, (ast_expr_t*)node->rhs);

  LLVMValueRef tmp_value = LLVMBuildLShr(gen->llvm_builder, lhs_value, rhs_value, "lshr_tmp");
  LLVMBuildStore(gen->llvm_builder, tmp_value, ((ast_expr_t*)node->lhs)->llvm_value);

  node->llvm_value = ((ast_expr_t*)node->lhs)->llvm_value;
  node->llvm_type = ((ast_expr_t*)node->lhs)->llvm_type;
}

void generator_visit_expr_op_call(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_op_call_t* node)
{
  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->callee);
  
  LLVMValueRef* param_values = malloc(vector_size(node->params) * sizeof(LLVMValueRef));

  VECTOR_FOR_LOOP(i, node->params)
  {
    ast_expr_t* param = (ast_expr_t*)vector_get(node->params, i);
    generator_visit_expr(gen, fun_node, param);

    param_values[i] = param->llvm_value;
  }

  node->llvm_value = LLVMBuildCall2(
    gen->llvm_builder,
    ((ast_expr_t*)node->callee)->llvm_type,
    ((ast_expr_t*)node->callee)->llvm_value,
    param_values,
    (uint32_t)vector_size(node->params),
    "call2_tmp"
  );

  free(param_values);

  node->llvm_type = LLVMGetReturnType(((ast_expr_t*)node->callee)->llvm_type);
}

void generator_visit_expr_id_var(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_id_t* node)
{
  node->llvm_type = ((ast_decl_var_t*)node->decl)->llvm_type;
  node->llvm_value = ((ast_decl_var_t*)node->decl)->llvm_value;
}

void generator_visit_expr_id_param(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_id_t* node)
{
  node->llvm_type = ((ast_decl_param_t*)node->decl)->llvm_type;
  node->llvm_value = ((ast_decl_param_t*)node->decl)->llvm_value;
}

void generator_visit_expr_id_fun(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_id_t* node)
{
  node->llvm_type = ((ast_decl_fun_t*)node->decl)->llvm_type;
  node->llvm_value = ((ast_decl_fun_t*)node->decl)->llvm_value;
}

void generator_visit_expr_id(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_id_t* node)
{
  switch (node->decl->kind)
  {
  case AST_DECL_VAR:   generator_visit_expr_id_var  (gen, fun_node, node); break;
  case AST_DECL_PARAM: generator_visit_expr_id_param(gen, fun_node, node); break;
  case AST_DECL_FUN:   generator_visit_expr_id_fun  (gen, fun_node, node); break;
  default: unreachable();
  }
}

void generator_visit_expr(generator_t* gen, ast_decl_fun_t* fun_node, ast_expr_t* node)
{
  switch (node->kind)
  {
  case AST_EXPR_ID:        generator_visit_expr_id       (gen, fun_node, (ast_expr_id_t*     )node); break;
  case AST_EXPR_LIT_INT:   generator_visit_expr_lit_int  (gen, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_LIT_FLT:   generator_visit_expr_lit_flt  (gen, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_LIT_STR:   generator_visit_expr_lit_str  (gen, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_LIT_BOOL:  generator_visit_expr_lit_bool (gen, fun_node, (ast_expr_lit_t*    )node); break;
  case AST_EXPR_OP_UNARY:  generator_visit_expr_op_un    (gen, fun_node, (ast_expr_op_un_t*  )node); break;
  case AST_EXPR_OP_BINARY: generator_visit_expr_op_bin   (gen, fun_node, (ast_expr_op_bin_t* )node); break;
  case AST_EXPR_OP_CALL:   generator_visit_expr_op_call  (gen, fun_node, (ast_expr_op_call_t*)node); break;
  default: unreachable();
  }
}

void generator_visit_stmt_while(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_while_t* node)
{
  node->llvm_cond = LLVMCreateBasicBlockInContext(gen->llvm_context, "while_cond");
  node->llvm_loop = LLVMCreateBasicBlockInContext(gen->llvm_context, "while_loop");
  node->llvm_end = LLVMCreateBasicBlockInContext(gen->llvm_context, "while_end");

  LLVMBuildBr(gen->llvm_builder, node->llvm_cond);
  LLVMAppendExistingBasicBlock(fun_node->llvm_value, node->llvm_cond);
  LLVMPositionBuilderAtEnd(gen->llvm_builder, node->llvm_cond);

  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->cond);

  LLVMBuildCondBr(gen->llvm_builder, ((ast_expr_t*)node->cond)->llvm_value, node->llvm_loop, node->llvm_end);
  LLVMAppendExistingBasicBlock(fun_node->llvm_value, node->llvm_loop);
  LLVMPositionBuilderAtEnd(gen->llvm_builder, node->llvm_loop);

  generator_visit_stmt(gen, fun_node, (ast_stmt_t*)node->stmt);

  LLVMBuildBr(gen->llvm_builder, node->llvm_cond);
  LLVMAppendExistingBasicBlock(fun_node->llvm_value, node->llvm_end);
  LLVMPositionBuilderAtEnd(gen->llvm_builder, node->llvm_end);
}

void generator_visit_stmt_break(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_break_t* node)
{
  switch (node->loop->kind)
  {
  case AST_STMT_WHILE: LLVMBuildBr(gen->llvm_builder, ((ast_stmt_while_t*)node->loop)->llvm_end); break;
  default: unreachable();
  }
}

void generator_visit_stmt_continue(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_continue_t* node)
{
  switch (node->loop->kind)
  {
  case AST_STMT_WHILE: LLVMBuildBr(gen->llvm_builder, ((ast_stmt_while_t*)node->loop)->llvm_cond); break;
  default: unreachable();
  }
}

void generator_visit_stmt_if(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_if_t* node)
{
  node->llvm_then = LLVMCreateBasicBlockInContext(gen->llvm_context, "if_then");

  if (node->stmt_else != NULL)
    node->llvm_else = LLVMCreateBasicBlockInContext(gen->llvm_context, "if_else");
  
  node->llvm_end = LLVMCreateBasicBlockInContext(gen->llvm_context, "if_end");

  generator_visit_expr(gen, fun_node, (ast_expr_t*)node->cond);

  LLVMBuildCondBr(gen->llvm_builder, ((ast_expr_t*)node->cond)->llvm_value, node->llvm_then, node->stmt_else != NULL ? node->llvm_else : node->llvm_end);
  LLVMAppendExistingBasicBlock(fun_node->llvm_value, node->llvm_then);
  LLVMPositionBuilderAtEnd(gen->llvm_builder, node->llvm_then);

  generator_visit_stmt(gen, fun_node, (ast_stmt_t*)node->stmt);

  if (LLVMGetBasicBlockTerminator(node->llvm_then) == NULL)
    LLVMBuildBr(gen->llvm_builder, node->llvm_end);

  if (node->stmt_else != NULL)
  {
    LLVMAppendExistingBasicBlock(fun_node->llvm_value, node->llvm_else);
    LLVMPositionBuilderAtEnd(gen->llvm_builder, node->llvm_else);

    generator_visit_stmt(gen, fun_node, (ast_stmt_t*)node->stmt_else);

    if (LLVMGetBasicBlockTerminator(node->llvm_else) == NULL)
      LLVMBuildBr(gen->llvm_builder, node->llvm_end);
  }

  LLVMAppendExistingBasicBlock(fun_node->llvm_value, node->llvm_end);
  LLVMPositionBuilderAtEnd(gen->llvm_builder, node->llvm_end);
}

void generator_visit_stmt_return(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_return_t* node)
{
  if (node->expr == NULL)
    LLVMBuildRetVoid(gen->llvm_builder);
  else
  {
    generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);
    LLVMBuildRet(gen->llvm_builder, ((ast_expr_t*)node->expr)->llvm_value);
  }
}

void generator_visit_stmt_block(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_block_t* node)
{
  VECTOR_FOR_LOOP(i, node->stmts)
    generator_visit_stmt(gen, fun_node, (ast_stmt_t*)vector_get(node->stmts, i));
}

void generator_visit_stmt(generator_t* gen, ast_decl_fun_t* fun_node, ast_stmt_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:      generator_visit_decl_var     (gen, fun_node, (ast_decl_var_t*     )node); break;
  case AST_STMT_IF:       generator_visit_stmt_if      (gen, fun_node, (ast_stmt_if_t*      )node); break;
  case AST_STMT_WHILE:    generator_visit_stmt_while   (gen, fun_node, (ast_stmt_while_t*   )node); break;
  case AST_STMT_BREAK:    generator_visit_stmt_break   (gen, fun_node, (ast_stmt_break_t*   )node); break;
  case AST_STMT_CONTINUE: generator_visit_stmt_continue(gen, fun_node, (ast_stmt_continue_t*)node); break;
  case AST_STMT_RETURN:   generator_visit_stmt_return  (gen, fun_node, (ast_stmt_return_t*  )node); break;
  case AST_STMT_BLOCK:    generator_visit_stmt_block   (gen, fun_node, (ast_stmt_block_t*   )node); break;
  case AST_STMT_EXPR:     generator_visit_expr         (gen, fun_node, (ast_expr_t*         )((ast_stmt_expr_t*)node)->expr); break;
  default: unreachable();
  }
}

void generator_visit_decl_var(generator_t* gen, ast_decl_fun_t* fun_node, ast_decl_var_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->type);

  if (node->expr != NULL)
    generator_visit_expr(gen, fun_node, (ast_expr_t*)node->expr);

  node->llvm_type = ((ast_type_t*)node->type)->llvm_type;
  node->llvm_value = LLVMBuildAlloca(gen->llvm_builder, node->llvm_type, "alloca_tmp");

  if (node->expr != NULL)
    LLVMBuildStore(gen->llvm_builder, ((ast_expr_t*)node->expr)->llvm_value, node->llvm_value);
}

void generator_visit_decl_param(generator_t* gen, ast_decl_fun_t* fun_node, ast_decl_param_t* node, size_t idx)
{
  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  node->llvm_value = LLVMBuildAlloca(gen->llvm_builder, node->llvm_type, "alloca_tmp");

  LLVMValueRef param_value = LLVMGetParam(fun_node->llvm_value, (uint32_t)idx);
  LLVMBuildStore(gen->llvm_builder, param_value, node->llvm_value);
}

void generator_visit_decl_fun(generator_t* gen, ast_decl_fun_t* node)
{
  generator_visit_type(gen, (ast_type_t*)node->return_type);

  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  size_t fun_id_len = node->id->tok->loc->len;

  char* fun_id_str = malloc(fun_id_len + 1);
  memcpy(fun_id_str, node->id->tok->loc->ptr, fun_id_len);
  fun_id_str[fun_id_len] = '\0';

  node->llvm_value = LLVMAddFunction(gen->llvm_module, fun_id_str, node->llvm_type);

  free(fun_id_str);

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
  default: unreachable();
  }

  LLVMSetFunctionCallConv(node->llvm_value, llvm_callconv);

  if (!node->is_extern)
  {
    node->llvm_entry = LLVMAppendBasicBlockInContext(gen->llvm_context, node->llvm_value, "entry");

    LLVMPositionBuilderAtEnd(gen->llvm_builder, node->llvm_entry);

    VECTOR_FOR_LOOP(i, node->params)
      generator_visit_decl_param(gen, node, (ast_decl_param_t*)vector_get(node->params, i), i);

    generator_visit_stmt(gen, node, (ast_stmt_t*)node->stmt);
  }
}

void generator_visit_decl_struct(generator_t* gen, ast_decl_struct_t* node)
{
  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
}

void generator_visit_decl_union(generator_t* gen, ast_decl_union_t* node)
{
  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;
}

void generator_visit_decl_enum(generator_t* gen, ast_decl_enum_t* node)
{
  typedesc_t* desc = typetable_lookup(gen->typetable, (ast_node_t*)node);
  node->llvm_type = desc->llvm_type;

  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_decl_enum_constant_t* member = (ast_decl_enum_constant_t*)vector_get(node->members, i);

    member->llvm_type = node->llvm_type;
    member->llvm_value = LLVMConstInt(node->llvm_type, (uint64_t)i, false);
  }
}

void generator_visit_decl_mod(generator_t* gen, ast_decl_mod_t* node)
{
  VECTOR_FOR_LOOP(i, node->members)
  {
    ast_decl_t* decl = (ast_decl_t*)vector_get(node->members, i);
    generator_visit_decl(gen, decl);
  }
}

void generator_visit_decl(generator_t* gen, ast_decl_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_FUN:    generator_visit_decl_fun   (gen, (ast_decl_fun_t*   )node); break;
  case AST_DECL_STRUCT: generator_visit_decl_struct(gen, (ast_decl_struct_t*)node); break;
  case AST_DECL_UNION:  generator_visit_decl_union (gen, (ast_decl_union_t* )node); break;
  case AST_DECL_MOD:    generator_visit_decl_mod   (gen, (ast_decl_mod_t*   )node); break;
  default: unreachable();
  }
}

void generator_visit_prog(generator_t* gen, ast_prog_t* node)
{
  VECTOR_FOR_LOOP(i, node->decls)
  {
    ast_decl_t* decl_node = (ast_decl_t*)vector_get(node->decls, i);
    generator_visit_decl(gen, decl_node);
  }
}

void generator_generate(generator_t* gen, LLVMContextRef llvm_context, LLVMTargetMachineRef llvm_machine, LLVMTargetDataRef llvm_layout, LLVMModuleRef llvm_module, typetable_t* typetable, ast_node_t* node)
{
  assert(node->kind == AST_PROG);

  gen->typetable = typetable;
  gen->llvm_context = llvm_context;
  gen->llvm_machine = llvm_machine;
  gen->llvm_layout = llvm_layout;
  gen->llvm_module = llvm_module;
  gen->llvm_builder = LLVMCreateBuilderInContext(gen->llvm_context);

  generator_visit_prog(gen, (ast_prog_t*)node);

  LLVMPassBuilderOptionsRef options = LLVMCreatePassBuilderOptions();

  LLVMPassBuilderOptionsSetDebugLogging(options, log_get_level() <= LOG_LEVEL_DEBUG);

  LLVMErrorRef error_ref = LLVMRunPasses(gen->llvm_module, "default<O0>", NULL, options);

  char* error_str = NULL;

  if (error_ref != NULL)
  {
    log_fatal("generator", "Failed to run passes.");
    error_str = LLVMGetErrorMessage(error_ref);
    fputs(error_str, stderr);
    LLVMDisposeMessage(error_str);
    exit(EXIT_FAILURE);
  }

  LLVMDisposePassBuilderOptions(options);
  LLVMDisposeBuilder(gen->llvm_builder);

  if (LLVMVerifyModule(gen->llvm_module, LLVMReturnStatusAction, &error_str))
  {
    log_fatal("generator", "Failed to verify module.");
    fputs(error_str, stderr);
    LLVMDisposeMessage(error_str);
    exit(EXIT_FAILURE);
  }
}
