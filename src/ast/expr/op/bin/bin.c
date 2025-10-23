/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/bin.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_bin_t* tau_ast_expr_op_bin_init(void)
{
  tau_ast_expr_op_bin_t* node = (tau_ast_expr_op_bin_t*)malloc(sizeof(tau_ast_expr_op_bin_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_BINARY;

  return node;
}

void tau_ast_expr_op_bin_free(tau_ast_expr_op_bin_t* node)
{
  free(node);
}

void tau_ast_expr_op_bin_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_bin_t* node)
{
  switch (node->op_kind)
  {
  case OP_AS:              tau_ast_expr_op_bin_as_nameres             (ctx, (tau_ast_expr_op_bin_as_t*             )node); break;
  case OP_ARIT_ADD:        tau_ast_expr_op_bin_arit_add_nameres       (ctx, (tau_ast_expr_op_bin_arit_add_t*       )node); break;
  case OP_ARIT_SUB:        tau_ast_expr_op_bin_arit_sub_nameres       (ctx, (tau_ast_expr_op_bin_arit_sub_t*       )node); break;
  case OP_ARIT_MUL:        tau_ast_expr_op_bin_arit_mul_nameres       (ctx, (tau_ast_expr_op_bin_arit_mul_t*       )node); break;
  case OP_ARIT_DIV:        tau_ast_expr_op_bin_arit_div_nameres       (ctx, (tau_ast_expr_op_bin_arit_div_t*       )node); break;
  case OP_ARIT_MOD:        tau_ast_expr_op_bin_arit_mod_nameres       (ctx, (tau_ast_expr_op_bin_arit_mod_t*       )node); break;
  case OP_BIT_AND:         tau_ast_expr_op_bin_bit_and_nameres        (ctx, (tau_ast_expr_op_bin_bit_and_t*        )node); break;
  case OP_BIT_OR:          tau_ast_expr_op_bin_bit_or_nameres         (ctx, (tau_ast_expr_op_bin_bit_or_t*         )node); break;
  case OP_BIT_XOR:         tau_ast_expr_op_bin_bit_xor_nameres        (ctx, (tau_ast_expr_op_bin_bit_xor_t*        )node); break;
  case OP_BIT_LSH:         tau_ast_expr_op_bin_bit_lsh_nameres        (ctx, (tau_ast_expr_op_bin_bit_lsh_t*        )node); break;
  case OP_BIT_RSH:         tau_ast_expr_op_bin_bit_rsh_nameres        (ctx, (tau_ast_expr_op_bin_bit_rsh_t*        )node); break;
  case OP_LOGIC_AND:       tau_ast_expr_op_bin_logic_and_nameres      (ctx, (tau_ast_expr_op_bin_logic_and_t*      )node); break;
  case OP_LOGIC_OR:        tau_ast_expr_op_bin_logic_or_nameres       (ctx, (tau_ast_expr_op_bin_logic_or_t*       )node); break;
  case OP_CMP_EQ:          tau_ast_expr_op_bin_cmp_eq_nameres         (ctx, (tau_ast_expr_op_bin_cmp_eq_t*         )node); break;
  case OP_CMP_NE:          tau_ast_expr_op_bin_cmp_ne_nameres         (ctx, (tau_ast_expr_op_bin_cmp_ne_t*         )node); break;
  case OP_CMP_LT:          tau_ast_expr_op_bin_cmp_lt_nameres         (ctx, (tau_ast_expr_op_bin_cmp_lt_t*         )node); break;
  case OP_CMP_LE:          tau_ast_expr_op_bin_cmp_le_nameres         (ctx, (tau_ast_expr_op_bin_cmp_le_t*         )node); break;
  case OP_CMP_GT:          tau_ast_expr_op_bin_cmp_gt_nameres         (ctx, (tau_ast_expr_op_bin_cmp_gt_t*         )node); break;
  case OP_CMP_GE:          tau_ast_expr_op_bin_cmp_ge_nameres         (ctx, (tau_ast_expr_op_bin_cmp_ge_t*         )node); break;
  case OP_ASSIGN:          tau_ast_expr_op_bin_assign_nameres         (ctx, (tau_ast_expr_op_bin_assign_t*         )node); break;
  case OP_ASSIGN_ARIT_ADD: tau_ast_expr_op_bin_assign_arit_add_nameres(ctx, (tau_ast_expr_op_bin_assign_arit_add_t*)node); break;
  case OP_ASSIGN_ARIT_SUB: tau_ast_expr_op_bin_assign_arit_sub_nameres(ctx, (tau_ast_expr_op_bin_assign_arit_sub_t*)node); break;
  case OP_ASSIGN_ARIT_MUL: tau_ast_expr_op_bin_assign_arit_mul_nameres(ctx, (tau_ast_expr_op_bin_assign_arit_mul_t*)node); break;
  case OP_ASSIGN_ARIT_DIV: tau_ast_expr_op_bin_assign_arit_div_nameres(ctx, (tau_ast_expr_op_bin_assign_arit_div_t*)node); break;
  case OP_ASSIGN_ARIT_MOD: tau_ast_expr_op_bin_assign_arit_mod_nameres(ctx, (tau_ast_expr_op_bin_assign_arit_mod_t*)node); break;
  case OP_ASSIGN_BIT_AND:  tau_ast_expr_op_bin_assign_bit_and_nameres (ctx, (tau_ast_expr_op_bin_assign_bit_and_t* )node); break;
  case OP_ASSIGN_BIT_OR:   tau_ast_expr_op_bin_assign_bit_or_nameres  (ctx, (tau_ast_expr_op_bin_assign_bit_or_t*  )node); break;
  case OP_ASSIGN_BIT_XOR:  tau_ast_expr_op_bin_assign_bit_xor_nameres (ctx, (tau_ast_expr_op_bin_assign_bit_xor_t* )node); break;
  case OP_ASSIGN_BIT_LSH:  tau_ast_expr_op_bin_assign_bit_lsh_nameres (ctx, (tau_ast_expr_op_bin_assign_bit_lsh_t* )node); break;
  case OP_ASSIGN_BIT_RSH:  tau_ast_expr_op_bin_assign_bit_rsh_nameres (ctx, (tau_ast_expr_op_bin_assign_bit_rsh_t* )node); break;
  case OP_SUBS:            tau_ast_expr_op_bin_subs_nameres           (ctx, (tau_ast_expr_op_bin_subs_t*           )node); break;
  case OP_ACCESS_DIRECT:   tau_ast_expr_op_bin_access_direct_nameres  (ctx, (tau_ast_expr_op_bin_access_direct_t*  )node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_expr_op_bin_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_bin_t* node)
{
  switch (node->op_kind)
  {
  case OP_AS:              tau_ast_expr_op_bin_as_typecheck             (ctx, (tau_ast_expr_op_bin_as_t*             )node); break;
  case OP_ARIT_ADD:        tau_ast_expr_op_bin_arit_add_typecheck       (ctx, (tau_ast_expr_op_bin_arit_add_t*       )node); break;
  case OP_ARIT_SUB:        tau_ast_expr_op_bin_arit_sub_typecheck       (ctx, (tau_ast_expr_op_bin_arit_sub_t*       )node); break;
  case OP_ARIT_MUL:        tau_ast_expr_op_bin_arit_mul_typecheck       (ctx, (tau_ast_expr_op_bin_arit_mul_t*       )node); break;
  case OP_ARIT_DIV:        tau_ast_expr_op_bin_arit_div_typecheck       (ctx, (tau_ast_expr_op_bin_arit_div_t*       )node); break;
  case OP_ARIT_MOD:        tau_ast_expr_op_bin_arit_mod_typecheck       (ctx, (tau_ast_expr_op_bin_arit_mod_t*       )node); break;
  case OP_BIT_AND:         tau_ast_expr_op_bin_bit_and_typecheck        (ctx, (tau_ast_expr_op_bin_bit_and_t*        )node); break;
  case OP_BIT_OR:          tau_ast_expr_op_bin_bit_or_typecheck         (ctx, (tau_ast_expr_op_bin_bit_or_t*         )node); break;
  case OP_BIT_XOR:         tau_ast_expr_op_bin_bit_xor_typecheck        (ctx, (tau_ast_expr_op_bin_bit_xor_t*        )node); break;
  case OP_BIT_LSH:         tau_ast_expr_op_bin_bit_lsh_typecheck        (ctx, (tau_ast_expr_op_bin_bit_lsh_t*        )node); break;
  case OP_BIT_RSH:         tau_ast_expr_op_bin_bit_rsh_typecheck        (ctx, (tau_ast_expr_op_bin_bit_rsh_t*        )node); break;
  case OP_LOGIC_AND:       tau_ast_expr_op_bin_logic_and_typecheck      (ctx, (tau_ast_expr_op_bin_logic_and_t*      )node); break;
  case OP_LOGIC_OR:        tau_ast_expr_op_bin_logic_or_typecheck       (ctx, (tau_ast_expr_op_bin_logic_or_t*       )node); break;
  case OP_CMP_EQ:          tau_ast_expr_op_bin_cmp_eq_typecheck         (ctx, (tau_ast_expr_op_bin_cmp_eq_t*         )node); break;
  case OP_CMP_NE:          tau_ast_expr_op_bin_cmp_ne_typecheck         (ctx, (tau_ast_expr_op_bin_cmp_ne_t*         )node); break;
  case OP_CMP_LT:          tau_ast_expr_op_bin_cmp_lt_typecheck         (ctx, (tau_ast_expr_op_bin_cmp_lt_t*         )node); break;
  case OP_CMP_LE:          tau_ast_expr_op_bin_cmp_le_typecheck         (ctx, (tau_ast_expr_op_bin_cmp_le_t*         )node); break;
  case OP_CMP_GT:          tau_ast_expr_op_bin_cmp_gt_typecheck         (ctx, (tau_ast_expr_op_bin_cmp_gt_t*         )node); break;
  case OP_CMP_GE:          tau_ast_expr_op_bin_cmp_ge_typecheck         (ctx, (tau_ast_expr_op_bin_cmp_ge_t*         )node); break;
  case OP_ASSIGN:          tau_ast_expr_op_bin_assign_typecheck         (ctx, (tau_ast_expr_op_bin_assign_t*         )node); break;
  case OP_ASSIGN_ARIT_ADD: tau_ast_expr_op_bin_assign_arit_add_typecheck(ctx, (tau_ast_expr_op_bin_assign_arit_add_t*)node); break;
  case OP_ASSIGN_ARIT_SUB: tau_ast_expr_op_bin_assign_arit_sub_typecheck(ctx, (tau_ast_expr_op_bin_assign_arit_sub_t*)node); break;
  case OP_ASSIGN_ARIT_MUL: tau_ast_expr_op_bin_assign_arit_mul_typecheck(ctx, (tau_ast_expr_op_bin_assign_arit_mul_t*)node); break;
  case OP_ASSIGN_ARIT_DIV: tau_ast_expr_op_bin_assign_arit_div_typecheck(ctx, (tau_ast_expr_op_bin_assign_arit_div_t*)node); break;
  case OP_ASSIGN_ARIT_MOD: tau_ast_expr_op_bin_assign_arit_mod_typecheck(ctx, (tau_ast_expr_op_bin_assign_arit_mod_t*)node); break;
  case OP_ASSIGN_BIT_AND:  tau_ast_expr_op_bin_assign_bit_and_typecheck (ctx, (tau_ast_expr_op_bin_assign_bit_and_t* )node); break;
  case OP_ASSIGN_BIT_OR:   tau_ast_expr_op_bin_assign_bit_or_typecheck  (ctx, (tau_ast_expr_op_bin_assign_bit_or_t*  )node); break;
  case OP_ASSIGN_BIT_XOR:  tau_ast_expr_op_bin_assign_bit_xor_typecheck (ctx, (tau_ast_expr_op_bin_assign_bit_xor_t* )node); break;
  case OP_ASSIGN_BIT_LSH:  tau_ast_expr_op_bin_assign_bit_lsh_typecheck (ctx, (tau_ast_expr_op_bin_assign_bit_lsh_t* )node); break;
  case OP_ASSIGN_BIT_RSH:  tau_ast_expr_op_bin_assign_bit_rsh_typecheck (ctx, (tau_ast_expr_op_bin_assign_bit_rsh_t* )node); break;
  case OP_SUBS:            tau_ast_expr_op_bin_subs_typecheck           (ctx, (tau_ast_expr_op_bin_subs_t*           )node); break;
  case OP_ACCESS_DIRECT:   tau_ast_expr_op_bin_access_direct_typecheck  (ctx, (tau_ast_expr_op_bin_access_direct_t*  )node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_expr_op_bin_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_bin_t* node)
{
  switch (node->op_kind)
  {
  case OP_AS:              tau_ast_expr_op_bin_as_codegen             (ctx, (tau_ast_expr_op_bin_as_t*             )node); break;
  case OP_ARIT_ADD:        tau_ast_expr_op_bin_arit_add_codegen       (ctx, (tau_ast_expr_op_bin_arit_add_t*       )node); break;
  case OP_ARIT_SUB:        tau_ast_expr_op_bin_arit_sub_codegen       (ctx, (tau_ast_expr_op_bin_arit_sub_t*       )node); break;
  case OP_ARIT_MUL:        tau_ast_expr_op_bin_arit_mul_codegen       (ctx, (tau_ast_expr_op_bin_arit_mul_t*       )node); break;
  case OP_ARIT_DIV:        tau_ast_expr_op_bin_arit_div_codegen       (ctx, (tau_ast_expr_op_bin_arit_div_t*       )node); break;
  case OP_ARIT_MOD:        tau_ast_expr_op_bin_arit_mod_codegen       (ctx, (tau_ast_expr_op_bin_arit_mod_t*       )node); break;
  case OP_BIT_AND:         tau_ast_expr_op_bin_bit_and_codegen        (ctx, (tau_ast_expr_op_bin_bit_and_t*        )node); break;
  case OP_BIT_OR:          tau_ast_expr_op_bin_bit_or_codegen         (ctx, (tau_ast_expr_op_bin_bit_or_t*         )node); break;
  case OP_BIT_XOR:         tau_ast_expr_op_bin_bit_xor_codegen        (ctx, (tau_ast_expr_op_bin_bit_xor_t*        )node); break;
  case OP_BIT_LSH:         tau_ast_expr_op_bin_bit_lsh_codegen        (ctx, (tau_ast_expr_op_bin_bit_lsh_t*        )node); break;
  case OP_BIT_RSH:         tau_ast_expr_op_bin_bit_rsh_codegen        (ctx, (tau_ast_expr_op_bin_bit_rsh_t*        )node); break;
  case OP_LOGIC_AND:       tau_ast_expr_op_bin_logic_and_codegen      (ctx, (tau_ast_expr_op_bin_logic_and_t*      )node); break;
  case OP_LOGIC_OR:        tau_ast_expr_op_bin_logic_or_codegen       (ctx, (tau_ast_expr_op_bin_logic_or_t*       )node); break;
  case OP_CMP_EQ:          tau_ast_expr_op_bin_cmp_eq_codegen         (ctx, (tau_ast_expr_op_bin_cmp_eq_t*         )node); break;
  case OP_CMP_NE:          tau_ast_expr_op_bin_cmp_ne_codegen         (ctx, (tau_ast_expr_op_bin_cmp_ne_t*         )node); break;
  case OP_CMP_LT:          tau_ast_expr_op_bin_cmp_lt_codegen         (ctx, (tau_ast_expr_op_bin_cmp_lt_t*         )node); break;
  case OP_CMP_LE:          tau_ast_expr_op_bin_cmp_le_codegen         (ctx, (tau_ast_expr_op_bin_cmp_le_t*         )node); break;
  case OP_CMP_GT:          tau_ast_expr_op_bin_cmp_gt_codegen         (ctx, (tau_ast_expr_op_bin_cmp_gt_t*         )node); break;
  case OP_CMP_GE:          tau_ast_expr_op_bin_cmp_ge_codegen         (ctx, (tau_ast_expr_op_bin_cmp_ge_t*         )node); break;
  case OP_ASSIGN:          tau_ast_expr_op_bin_assign_codegen         (ctx, (tau_ast_expr_op_bin_assign_t*         )node); break;
  case OP_ASSIGN_ARIT_ADD: tau_ast_expr_op_bin_assign_arit_add_codegen(ctx, (tau_ast_expr_op_bin_assign_arit_add_t*)node); break;
  case OP_ASSIGN_ARIT_SUB: tau_ast_expr_op_bin_assign_arit_sub_codegen(ctx, (tau_ast_expr_op_bin_assign_arit_sub_t*)node); break;
  case OP_ASSIGN_ARIT_MUL: tau_ast_expr_op_bin_assign_arit_mul_codegen(ctx, (tau_ast_expr_op_bin_assign_arit_mul_t*)node); break;
  case OP_ASSIGN_ARIT_DIV: tau_ast_expr_op_bin_assign_arit_div_codegen(ctx, (tau_ast_expr_op_bin_assign_arit_div_t*)node); break;
  case OP_ASSIGN_ARIT_MOD: tau_ast_expr_op_bin_assign_arit_mod_codegen(ctx, (tau_ast_expr_op_bin_assign_arit_mod_t*)node); break;
  case OP_ASSIGN_BIT_AND:  tau_ast_expr_op_bin_assign_bit_and_codegen (ctx, (tau_ast_expr_op_bin_assign_bit_and_t* )node); break;
  case OP_ASSIGN_BIT_OR:   tau_ast_expr_op_bin_assign_bit_or_codegen  (ctx, (tau_ast_expr_op_bin_assign_bit_or_t*  )node); break;
  case OP_ASSIGN_BIT_XOR:  tau_ast_expr_op_bin_assign_bit_xor_codegen (ctx, (tau_ast_expr_op_bin_assign_bit_xor_t* )node); break;
  case OP_ASSIGN_BIT_LSH:  tau_ast_expr_op_bin_assign_bit_lsh_codegen (ctx, (tau_ast_expr_op_bin_assign_bit_lsh_t* )node); break;
  case OP_ASSIGN_BIT_RSH:  tau_ast_expr_op_bin_assign_bit_rsh_codegen (ctx, (tau_ast_expr_op_bin_assign_bit_rsh_t* )node); break;
  case OP_SUBS:            tau_ast_expr_op_bin_subs_codegen           (ctx, (tau_ast_expr_op_bin_subs_t*           )node); break;
  case OP_ACCESS_DIRECT:   tau_ast_expr_op_bin_access_direct_codegen  (ctx, (tau_ast_expr_op_bin_access_direct_t*  )node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_expr_op_bin_dump_json(FILE* stream, tau_ast_expr_op_bin_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"lhs\":");
  tau_ast_node_dump_json(stream, node->lhs);
  fprintf(stream, ",\"rhs\":");
  tau_ast_node_dump_json(stream, node->rhs);
  fputc('}', stream);
}
