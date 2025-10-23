/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/un.h"

#include "ast/ast.h"
#include "ast/registry.h"

tau_ast_expr_op_un_t* tau_ast_expr_op_un_init(void)
{
  tau_ast_expr_op_un_t* node = (tau_ast_expr_op_un_t*)malloc(sizeof(tau_ast_expr_op_un_t));
  TAU_CLEAROBJ(node);

  tau_ast_registry_register((tau_ast_node_t*)node);

  node->kind = TAU_AST_EXPR_OP_UNARY;

  return node;
}

void tau_ast_expr_op_un_free(tau_ast_expr_op_un_t* node)
{
  free(node);
}

void tau_ast_expr_op_un_nameres(tau_nameres_ctx_t* ctx, tau_ast_expr_op_un_t* node)
{
  switch (node->op_kind)
  {
  case OP_SIZEOF:        tau_ast_expr_op_un_sizeof_nameres       (ctx, (tau_ast_expr_op_un_sizeof_t*       )node); break;
  case OP_ALIGNOF:       tau_ast_expr_op_un_alignof_nameres      (ctx, (tau_ast_expr_op_un_alignof_t*      )node); break;
  case OP_ARIT_INC_PRE:  tau_ast_expr_op_un_arit_inc_pre_nameres (ctx, (tau_ast_expr_op_un_arit_inc_pre_t* )node); break;
  case OP_ARIT_INC_POST: tau_ast_expr_op_un_arit_inc_post_nameres(ctx, (tau_ast_expr_op_un_arit_inc_post_t*)node); break;
  case OP_ARIT_DEC_PRE:  tau_ast_expr_op_un_arit_dec_pre_nameres (ctx, (tau_ast_expr_op_un_arit_dec_pre_t* )node); break;
  case OP_ARIT_DEC_POST: tau_ast_expr_op_un_arit_dec_post_nameres(ctx, (tau_ast_expr_op_un_arit_dec_post_t*)node); break;
  case OP_ARIT_POS:      tau_ast_expr_op_un_arit_pos_nameres     (ctx, (tau_ast_expr_op_un_arit_pos_t*     )node); break;
  case OP_ARIT_NEG:      tau_ast_expr_op_un_arit_neg_nameres     (ctx, (tau_ast_expr_op_un_arit_neg_t*     )node); break;
  case OP_BIT_NOT:       tau_ast_expr_op_un_bit_not_nameres      (ctx, (tau_ast_expr_op_un_bit_not_t*      )node); break;
  case OP_LOGIC_NOT:     tau_ast_expr_op_un_logic_not_nameres    (ctx, (tau_ast_expr_op_un_logic_not_t*    )node); break;
  case OP_IND:           tau_ast_expr_op_un_ind_nameres          (ctx, (tau_ast_expr_op_un_ind_t*          )node); break;
  case OP_ADDR:          tau_ast_expr_op_un_addr_nameres         (ctx, (tau_ast_expr_op_un_addr_t*         )node); break;
  case OP_UNWRAP_SAFE:   tau_ast_expr_op_un_unwrap_safe_nameres  (ctx, (tau_ast_expr_op_un_unwrap_safe_t*  )node); break;
  case OP_UNWRAP_UNSAFE: tau_ast_expr_op_un_unwrap_unsafe_nameres(ctx, (tau_ast_expr_op_un_unwrap_unsafe_t*)node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_expr_op_un_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_expr_op_un_t* node)
{
  switch (node->op_kind)
  {
  case OP_SIZEOF:        tau_ast_expr_op_un_sizeof_typecheck       (ctx, (tau_ast_expr_op_un_sizeof_t*       )node); break;
  case OP_ALIGNOF:       tau_ast_expr_op_un_alignof_typecheck      (ctx, (tau_ast_expr_op_un_alignof_t*      )node); break;
  case OP_ARIT_INC_PRE:  tau_ast_expr_op_un_arit_inc_pre_typecheck (ctx, (tau_ast_expr_op_un_arit_inc_pre_t*      )node); break;
  case OP_ARIT_INC_POST: tau_ast_expr_op_un_arit_inc_post_typecheck(ctx, (tau_ast_expr_op_un_arit_inc_post_t*)node); break;
  case OP_ARIT_DEC_PRE:  tau_ast_expr_op_un_arit_dec_pre_typecheck (ctx, (tau_ast_expr_op_un_arit_dec_pre_t* )node); break;
  case OP_ARIT_DEC_POST: tau_ast_expr_op_un_arit_dec_post_typecheck(ctx, (tau_ast_expr_op_un_arit_dec_post_t*)node); break;
  case OP_ARIT_POS:      tau_ast_expr_op_un_arit_pos_typecheck     (ctx, (tau_ast_expr_op_un_arit_pos_t*     )node); break;
  case OP_ARIT_NEG:      tau_ast_expr_op_un_arit_neg_typecheck     (ctx, (tau_ast_expr_op_un_arit_neg_t*     )node); break;
  case OP_BIT_NOT:       tau_ast_expr_op_un_bit_not_typecheck      (ctx, (tau_ast_expr_op_un_bit_not_t*      )node); break;
  case OP_LOGIC_NOT:     tau_ast_expr_op_un_logic_not_typecheck    (ctx, (tau_ast_expr_op_un_logic_not_t*    )node); break;
  case OP_IND:           tau_ast_expr_op_un_ind_typecheck          (ctx, (tau_ast_expr_op_un_ind_t*          )node); break;
  case OP_ADDR:          tau_ast_expr_op_un_addr_typecheck         (ctx, (tau_ast_expr_op_un_addr_t*         )node); break;
  case OP_UNWRAP_SAFE:   tau_ast_expr_op_un_unwrap_safe_typecheck  (ctx, (tau_ast_expr_op_un_unwrap_safe_t*  )node); break;
  case OP_UNWRAP_UNSAFE: tau_ast_expr_op_un_unwrap_unsafe_typecheck(ctx, (tau_ast_expr_op_un_unwrap_unsafe_t*)node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_expr_op_un_codegen(tau_codegen_ctx_t* ctx, tau_ast_expr_op_un_t* node)
{
  switch (node->op_kind)
  {
  case OP_SIZEOF:        tau_ast_expr_op_un_sizeof_codegen       (ctx, (tau_ast_expr_op_un_sizeof_t*       )node); break;
  case OP_ALIGNOF:       tau_ast_expr_op_un_alignof_codegen      (ctx, (tau_ast_expr_op_un_alignof_t*      )node); break;
  case OP_ARIT_INC_PRE:  tau_ast_expr_op_un_arit_inc_pre_codegen (ctx, (tau_ast_expr_op_un_arit_inc_pre_t*      )node); break;
  case OP_ARIT_INC_POST: tau_ast_expr_op_un_arit_inc_post_codegen(ctx, (tau_ast_expr_op_un_arit_inc_post_t*)node); break;
  case OP_ARIT_DEC_PRE:  tau_ast_expr_op_un_arit_dec_pre_codegen (ctx, (tau_ast_expr_op_un_arit_dec_pre_t* )node); break;
  case OP_ARIT_DEC_POST: tau_ast_expr_op_un_arit_dec_post_codegen(ctx, (tau_ast_expr_op_un_arit_dec_post_t*)node); break;
  case OP_ARIT_POS:      tau_ast_expr_op_un_arit_pos_codegen     (ctx, (tau_ast_expr_op_un_arit_pos_t*     )node); break;
  case OP_ARIT_NEG:      tau_ast_expr_op_un_arit_neg_codegen     (ctx, (tau_ast_expr_op_un_arit_neg_t*     )node); break;
  case OP_BIT_NOT:       tau_ast_expr_op_un_bit_not_codegen      (ctx, (tau_ast_expr_op_un_bit_not_t*      )node); break;
  case OP_LOGIC_NOT:     tau_ast_expr_op_un_logic_not_codegen    (ctx, (tau_ast_expr_op_un_logic_not_t*    )node); break;
  case OP_IND:           tau_ast_expr_op_un_ind_codegen          (ctx, (tau_ast_expr_op_un_ind_t*          )node); break;
  case OP_ADDR:          tau_ast_expr_op_un_addr_codegen         (ctx, (tau_ast_expr_op_un_addr_t*         )node); break;
  case OP_UNWRAP_SAFE:   tau_ast_expr_op_un_unwrap_safe_codegen  (ctx, (tau_ast_expr_op_un_unwrap_safe_t*  )node); break;
  case OP_UNWRAP_UNSAFE: tau_ast_expr_op_un_unwrap_unsafe_codegen(ctx, (tau_ast_expr_op_un_unwrap_unsafe_t*)node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_expr_op_un_dump_json(FILE* stream, tau_ast_expr_op_un_t* node)
{
  fprintf(stream, "{\"kind\":\"%s\"", tau_ast_kind_to_cstr(node->kind));
  fprintf(stream, ",\"op_kind\":\"%s\"", op_kind_to_cstr(node->op_kind));
  fprintf(stream, ",\"expr\":");
  tau_ast_node_dump_json(stream, node->expr);
  fputc('}', stream);
}
