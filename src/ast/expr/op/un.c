/**
 * \file un.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un.h"

#include "ast/registry.h"
#include "utils/common.h"
#include "utils/diagnostics.h"
#include "utils/memory/memtrace.h"

ast_expr_op_un_t* ast_expr_op_un_init(void)
{
  ast_expr_op_un_t* node = (ast_expr_op_un_t*)malloc(sizeof(ast_expr_op_un_t));
  clearobj(node);

  ast_registry_register((ast_node_t*)node);

  node->kind = AST_EXPR_OP_BINARY;

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
  assert(expr_desc != NULL);

  typedesc_t* desc = NULL;

  switch (node->op_kind)
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
  {
    desc = typebuilder_build_const(ctx->typebuilder, typebuilder_build_usize(ctx->typebuilder));
    break;
  }
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
  {
    if (typedesc_remove_const(expr_desc)->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    if (typedesc_remove_const_ref(expr_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->expr->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    desc = typedesc_remove_const(expr_desc);
    break;
  }
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
  {
    if (typedesc_remove_const(expr_desc)->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    if (typedesc_remove_const_ref(expr_desc)->kind != TYPEDESC_MUT)
      report_error_expected_mutable_type(node->expr->tok->loc);

    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    desc = typedesc_remove_const_ref_mut(expr_desc);
    break;
  }
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
  {
    if (!typedesc_is_arithmetic(typedesc_remove_const_ref_mut(expr_desc)))
      report_error_expected_arithmetic_type(node->expr->tok->loc);

    desc = typedesc_remove_const_ref_mut(expr_desc);
    break;
  }
  case OP_LOGIC_NOT:
  {
    if (typedesc_remove_const_ref_mut(expr_desc)->kind != TYPEDESC_BOOL)
      report_error_expected_bool_type(node->expr->tok->loc);

    desc = typedesc_remove_const_ref_mut(expr_desc);
    break;
  }
  case OP_IND:
  {
    if (typedesc_remove_const_ref_mut(expr_desc)->kind != TYPEDESC_PTR)
      report_error_expected_ptr_type(node->expr->tok->loc);
    
    typedesc_t* pointed_desc = ((typedesc_ptr_t*)typedesc_remove_const_ref_mut(expr_desc))->base_type;

    desc = typebuilder_build_ref(ctx->typebuilder, pointed_desc);
    break;
  }
  case OP_ADDR:
  {
    if (typedesc_remove_const(expr_desc)->kind != TYPEDESC_REF)
      report_error_expected_reference_type(node->expr->tok->loc);

    desc = typebuilder_build_ptr(ctx->typebuilder, typedesc_remove_const_ref(expr_desc));
    break;
  }
  default:
    unreachable();
  }

  if (expr_desc->kind == TYPEDESC_CONST && desc->kind != TYPEDESC_CONST)
    desc = typebuilder_build_const(ctx->typebuilder, desc);

  typetable_insert(ctx->typetable, (ast_node_t*)node, desc);
}

void ast_expr_op_un_dump_json(FILE* stream, ast_expr_op_un_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"expr\":");
  ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
