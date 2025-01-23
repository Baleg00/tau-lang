/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/bin/bin.h"

#include "ast/ast.h"
#include "ast/registry.h"

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
  switch (node->op_kind)
  {
  case OP_AS:              ast_expr_op_bin_as_nameres             (ctx, (ast_expr_op_bin_as_t*             )node); break;
  case OP_ARIT_ADD:        ast_expr_op_bin_arit_add_nameres       (ctx, (ast_expr_op_bin_arit_add_t*       )node); break;
  case OP_ARIT_SUB:        ast_expr_op_bin_arit_sub_nameres       (ctx, (ast_expr_op_bin_arit_sub_t*       )node); break;
  case OP_ARIT_MUL:        ast_expr_op_bin_arit_mul_nameres       (ctx, (ast_expr_op_bin_arit_mul_t*       )node); break;
  case OP_ARIT_DIV:        ast_expr_op_bin_arit_div_nameres       (ctx, (ast_expr_op_bin_arit_div_t*       )node); break;
  case OP_ARIT_MOD:        ast_expr_op_bin_arit_mod_nameres       (ctx, (ast_expr_op_bin_arit_mod_t*       )node); break;
  case OP_BIT_AND:         ast_expr_op_bin_bit_and_nameres        (ctx, (ast_expr_op_bin_bit_and_t*        )node); break;
  case OP_BIT_OR:          ast_expr_op_bin_bit_or_nameres         (ctx, (ast_expr_op_bin_bit_or_t*         )node); break;
  case OP_BIT_XOR:         ast_expr_op_bin_bit_xor_nameres        (ctx, (ast_expr_op_bin_bit_xor_t*        )node); break;
  case OP_BIT_LSH:         ast_expr_op_bin_bit_lsh_nameres        (ctx, (ast_expr_op_bin_bit_lsh_t*        )node); break;
  case OP_BIT_RSH:         ast_expr_op_bin_bit_rsh_nameres        (ctx, (ast_expr_op_bin_bit_rsh_t*        )node); break;
  case OP_LOGIC_AND:       ast_expr_op_bin_logic_and_nameres      (ctx, (ast_expr_op_bin_logic_and_t*      )node); break;
  case OP_LOGIC_OR:        ast_expr_op_bin_logic_or_nameres       (ctx, (ast_expr_op_bin_logic_or_t*       )node); break;
  case OP_CMP_EQ:          ast_expr_op_bin_cmp_eq_nameres         (ctx, (ast_expr_op_bin_cmp_eq_t*         )node); break;
  case OP_CMP_NE:          ast_expr_op_bin_cmp_ne_nameres         (ctx, (ast_expr_op_bin_cmp_ne_t*         )node); break;
  case OP_CMP_LT:          ast_expr_op_bin_cmp_lt_nameres         (ctx, (ast_expr_op_bin_cmp_lt_t*         )node); break;
  case OP_CMP_LE:          ast_expr_op_bin_cmp_le_nameres         (ctx, (ast_expr_op_bin_cmp_le_t*         )node); break;
  case OP_CMP_GT:          ast_expr_op_bin_cmp_gt_nameres         (ctx, (ast_expr_op_bin_cmp_gt_t*         )node); break;
  case OP_CMP_GE:          ast_expr_op_bin_cmp_ge_nameres         (ctx, (ast_expr_op_bin_cmp_ge_t*         )node); break;
  case OP_ASSIGN:          ast_expr_op_bin_assign_nameres         (ctx, (ast_expr_op_bin_assign_t*         )node); break;
  case OP_ASSIGN_ARIT_ADD: ast_expr_op_bin_assign_arit_add_nameres(ctx, (ast_expr_op_bin_assign_arit_add_t*)node); break;
  case OP_ASSIGN_ARIT_SUB: ast_expr_op_bin_assign_arit_sub_nameres(ctx, (ast_expr_op_bin_assign_arit_sub_t*)node); break;
  case OP_ASSIGN_ARIT_MUL: ast_expr_op_bin_assign_arit_mul_nameres(ctx, (ast_expr_op_bin_assign_arit_mul_t*)node); break;
  case OP_ASSIGN_ARIT_DIV: ast_expr_op_bin_assign_arit_div_nameres(ctx, (ast_expr_op_bin_assign_arit_div_t*)node); break;
  case OP_ASSIGN_ARIT_MOD: ast_expr_op_bin_assign_arit_mod_nameres(ctx, (ast_expr_op_bin_assign_arit_mod_t*)node); break;
  case OP_ASSIGN_BIT_AND:  ast_expr_op_bin_assign_bit_and_nameres (ctx, (ast_expr_op_bin_assign_bit_and_t* )node); break;
  case OP_ASSIGN_BIT_OR:   ast_expr_op_bin_assign_bit_or_nameres  (ctx, (ast_expr_op_bin_assign_bit_or_t*  )node); break;
  case OP_ASSIGN_BIT_XOR:  ast_expr_op_bin_assign_bit_xor_nameres (ctx, (ast_expr_op_bin_assign_bit_xor_t* )node); break;
  case OP_ASSIGN_BIT_LSH:  ast_expr_op_bin_assign_bit_lsh_nameres (ctx, (ast_expr_op_bin_assign_bit_lsh_t* )node); break;
  case OP_ASSIGN_BIT_RSH:  ast_expr_op_bin_assign_bit_rsh_nameres (ctx, (ast_expr_op_bin_assign_bit_rsh_t* )node); break;
  case OP_SUBS:            ast_expr_op_bin_subs_nameres           (ctx, (ast_expr_op_bin_subs_t*           )node); break;
  case OP_ACCESS:          ast_expr_op_bin_access_nameres         (ctx, (ast_expr_op_bin_access_t*         )node); break;
  default: UNREACHABLE();
  }
}

void ast_expr_op_bin_typecheck(typecheck_ctx_t* ctx, ast_expr_op_bin_t* node)
{
  switch (node->op_kind)
  {
  case OP_AS:              ast_expr_op_bin_as_typecheck             (ctx, (ast_expr_op_bin_as_t*             )node); break;
  case OP_ARIT_ADD:        ast_expr_op_bin_arit_add_typecheck       (ctx, (ast_expr_op_bin_arit_add_t*       )node); break;
  case OP_ARIT_SUB:        ast_expr_op_bin_arit_sub_typecheck       (ctx, (ast_expr_op_bin_arit_sub_t*       )node); break;
  case OP_ARIT_MUL:        ast_expr_op_bin_arit_mul_typecheck       (ctx, (ast_expr_op_bin_arit_mul_t*       )node); break;
  case OP_ARIT_DIV:        ast_expr_op_bin_arit_div_typecheck       (ctx, (ast_expr_op_bin_arit_div_t*       )node); break;
  case OP_ARIT_MOD:        ast_expr_op_bin_arit_mod_typecheck       (ctx, (ast_expr_op_bin_arit_mod_t*       )node); break;
  case OP_BIT_AND:         ast_expr_op_bin_bit_and_typecheck        (ctx, (ast_expr_op_bin_bit_and_t*        )node); break;
  case OP_BIT_OR:          ast_expr_op_bin_bit_or_typecheck         (ctx, (ast_expr_op_bin_bit_or_t*         )node); break;
  case OP_BIT_XOR:         ast_expr_op_bin_bit_xor_typecheck        (ctx, (ast_expr_op_bin_bit_xor_t*        )node); break;
  case OP_BIT_LSH:         ast_expr_op_bin_bit_lsh_typecheck        (ctx, (ast_expr_op_bin_bit_lsh_t*        )node); break;
  case OP_BIT_RSH:         ast_expr_op_bin_bit_rsh_typecheck        (ctx, (ast_expr_op_bin_bit_rsh_t*        )node); break;
  case OP_LOGIC_AND:       ast_expr_op_bin_logic_and_typecheck      (ctx, (ast_expr_op_bin_logic_and_t*      )node); break;
  case OP_LOGIC_OR:        ast_expr_op_bin_logic_or_typecheck       (ctx, (ast_expr_op_bin_logic_or_t*       )node); break;
  case OP_CMP_EQ:          ast_expr_op_bin_cmp_eq_typecheck         (ctx, (ast_expr_op_bin_cmp_eq_t*         )node); break;
  case OP_CMP_NE:          ast_expr_op_bin_cmp_ne_typecheck         (ctx, (ast_expr_op_bin_cmp_ne_t*         )node); break;
  case OP_CMP_LT:          ast_expr_op_bin_cmp_lt_typecheck         (ctx, (ast_expr_op_bin_cmp_lt_t*         )node); break;
  case OP_CMP_LE:          ast_expr_op_bin_cmp_le_typecheck         (ctx, (ast_expr_op_bin_cmp_le_t*         )node); break;
  case OP_CMP_GT:          ast_expr_op_bin_cmp_gt_typecheck         (ctx, (ast_expr_op_bin_cmp_gt_t*         )node); break;
  case OP_CMP_GE:          ast_expr_op_bin_cmp_ge_typecheck         (ctx, (ast_expr_op_bin_cmp_ge_t*         )node); break;
  case OP_ASSIGN:          ast_expr_op_bin_assign_typecheck         (ctx, (ast_expr_op_bin_assign_t*         )node); break;
  case OP_ASSIGN_ARIT_ADD: ast_expr_op_bin_assign_arit_add_typecheck(ctx, (ast_expr_op_bin_assign_arit_add_t*)node); break;
  case OP_ASSIGN_ARIT_SUB: ast_expr_op_bin_assign_arit_sub_typecheck(ctx, (ast_expr_op_bin_assign_arit_sub_t*)node); break;
  case OP_ASSIGN_ARIT_MUL: ast_expr_op_bin_assign_arit_mul_typecheck(ctx, (ast_expr_op_bin_assign_arit_mul_t*)node); break;
  case OP_ASSIGN_ARIT_DIV: ast_expr_op_bin_assign_arit_div_typecheck(ctx, (ast_expr_op_bin_assign_arit_div_t*)node); break;
  case OP_ASSIGN_ARIT_MOD: ast_expr_op_bin_assign_arit_mod_typecheck(ctx, (ast_expr_op_bin_assign_arit_mod_t*)node); break;
  case OP_ASSIGN_BIT_AND:  ast_expr_op_bin_assign_bit_and_typecheck (ctx, (ast_expr_op_bin_assign_bit_and_t* )node); break;
  case OP_ASSIGN_BIT_OR:   ast_expr_op_bin_assign_bit_or_typecheck  (ctx, (ast_expr_op_bin_assign_bit_or_t*  )node); break;
  case OP_ASSIGN_BIT_XOR:  ast_expr_op_bin_assign_bit_xor_typecheck (ctx, (ast_expr_op_bin_assign_bit_xor_t* )node); break;
  case OP_ASSIGN_BIT_LSH:  ast_expr_op_bin_assign_bit_lsh_typecheck (ctx, (ast_expr_op_bin_assign_bit_lsh_t* )node); break;
  case OP_ASSIGN_BIT_RSH:  ast_expr_op_bin_assign_bit_rsh_typecheck (ctx, (ast_expr_op_bin_assign_bit_rsh_t* )node); break;
  case OP_SUBS:            ast_expr_op_bin_subs_typecheck           (ctx, (ast_expr_op_bin_subs_t*           )node); break;
  case OP_ACCESS:          ast_expr_op_bin_access_typecheck         (ctx, (ast_expr_op_bin_access_t*         )node); break;
  default: UNREACHABLE();
  }
}

void ast_expr_op_bin_codegen(codegen_ctx_t* ctx, ast_expr_op_bin_t* node)
{
  switch (node->op_kind)
  {
  case OP_AS:              ast_expr_op_bin_as_codegen             (ctx, (ast_expr_op_bin_as_t*             )node); break;
  case OP_ARIT_ADD:        ast_expr_op_bin_arit_add_codegen       (ctx, (ast_expr_op_bin_arit_add_t*       )node); break;
  case OP_ARIT_SUB:        ast_expr_op_bin_arit_sub_codegen       (ctx, (ast_expr_op_bin_arit_sub_t*       )node); break;
  case OP_ARIT_MUL:        ast_expr_op_bin_arit_mul_codegen       (ctx, (ast_expr_op_bin_arit_mul_t*       )node); break;
  case OP_ARIT_DIV:        ast_expr_op_bin_arit_div_codegen       (ctx, (ast_expr_op_bin_arit_div_t*       )node); break;
  case OP_ARIT_MOD:        ast_expr_op_bin_arit_mod_codegen       (ctx, (ast_expr_op_bin_arit_mod_t*       )node); break;
  case OP_BIT_AND:         ast_expr_op_bin_bit_and_codegen        (ctx, (ast_expr_op_bin_bit_and_t*        )node); break;
  case OP_BIT_OR:          ast_expr_op_bin_bit_or_codegen         (ctx, (ast_expr_op_bin_bit_or_t*         )node); break;
  case OP_BIT_XOR:         ast_expr_op_bin_bit_xor_codegen        (ctx, (ast_expr_op_bin_bit_xor_t*        )node); break;
  case OP_BIT_LSH:         ast_expr_op_bin_bit_lsh_codegen        (ctx, (ast_expr_op_bin_bit_lsh_t*        )node); break;
  case OP_BIT_RSH:         ast_expr_op_bin_bit_rsh_codegen        (ctx, (ast_expr_op_bin_bit_rsh_t*        )node); break;
  case OP_LOGIC_AND:       ast_expr_op_bin_logic_and_codegen      (ctx, (ast_expr_op_bin_logic_and_t*      )node); break;
  case OP_LOGIC_OR:        ast_expr_op_bin_logic_or_codegen       (ctx, (ast_expr_op_bin_logic_or_t*       )node); break;
  case OP_CMP_EQ:          ast_expr_op_bin_cmp_eq_codegen         (ctx, (ast_expr_op_bin_cmp_eq_t*         )node); break;
  case OP_CMP_NE:          ast_expr_op_bin_cmp_ne_codegen         (ctx, (ast_expr_op_bin_cmp_ne_t*         )node); break;
  case OP_CMP_LT:          ast_expr_op_bin_cmp_lt_codegen         (ctx, (ast_expr_op_bin_cmp_lt_t*         )node); break;
  case OP_CMP_LE:          ast_expr_op_bin_cmp_le_codegen         (ctx, (ast_expr_op_bin_cmp_le_t*         )node); break;
  case OP_CMP_GT:          ast_expr_op_bin_cmp_gt_codegen         (ctx, (ast_expr_op_bin_cmp_gt_t*         )node); break;
  case OP_CMP_GE:          ast_expr_op_bin_cmp_ge_codegen         (ctx, (ast_expr_op_bin_cmp_ge_t*         )node); break;
  case OP_ASSIGN:          ast_expr_op_bin_assign_codegen         (ctx, (ast_expr_op_bin_assign_t*         )node); break;
  case OP_ASSIGN_ARIT_ADD: ast_expr_op_bin_assign_arit_add_codegen(ctx, (ast_expr_op_bin_assign_arit_add_t*)node); break;
  case OP_ASSIGN_ARIT_SUB: ast_expr_op_bin_assign_arit_sub_codegen(ctx, (ast_expr_op_bin_assign_arit_sub_t*)node); break;
  case OP_ASSIGN_ARIT_MUL: ast_expr_op_bin_assign_arit_mul_codegen(ctx, (ast_expr_op_bin_assign_arit_mul_t*)node); break;
  case OP_ASSIGN_ARIT_DIV: ast_expr_op_bin_assign_arit_div_codegen(ctx, (ast_expr_op_bin_assign_arit_div_t*)node); break;
  case OP_ASSIGN_ARIT_MOD: ast_expr_op_bin_assign_arit_mod_codegen(ctx, (ast_expr_op_bin_assign_arit_mod_t*)node); break;
  case OP_ASSIGN_BIT_AND:  ast_expr_op_bin_assign_bit_and_codegen (ctx, (ast_expr_op_bin_assign_bit_and_t* )node); break;
  case OP_ASSIGN_BIT_OR:   ast_expr_op_bin_assign_bit_or_codegen  (ctx, (ast_expr_op_bin_assign_bit_or_t*  )node); break;
  case OP_ASSIGN_BIT_XOR:  ast_expr_op_bin_assign_bit_xor_codegen (ctx, (ast_expr_op_bin_assign_bit_xor_t* )node); break;
  case OP_ASSIGN_BIT_LSH:  ast_expr_op_bin_assign_bit_lsh_codegen (ctx, (ast_expr_op_bin_assign_bit_lsh_t* )node); break;
  case OP_ASSIGN_BIT_RSH:  ast_expr_op_bin_assign_bit_rsh_codegen (ctx, (ast_expr_op_bin_assign_bit_rsh_t* )node); break;
  case OP_SUBS:            ast_expr_op_bin_subs_codegen           (ctx, (ast_expr_op_bin_subs_t*           )node); break;
  case OP_ACCESS:          ast_expr_op_bin_access_codegen         (ctx, (ast_expr_op_bin_access_t*         )node); break;
  default: UNREACHABLE();
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
