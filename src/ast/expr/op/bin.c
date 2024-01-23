/**
 * \file bin.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

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
