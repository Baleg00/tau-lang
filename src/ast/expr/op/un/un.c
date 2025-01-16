/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/un/un.h"

#include "ast/registry.h"
#include "utils/diagnostics.h"

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
  switch (node->op_kind)
  {
  case OP_SIZEOF:        ast_expr_op_un_sizeof_nameres       (ctx, (ast_expr_op_un_sizeof_t*       )node); break;
  case OP_ALIGNOF:       ast_expr_op_un_alignof_nameres      (ctx, (ast_expr_op_un_alignof_t*      )node); break;
  case OP_ARIT_INC_PRE:  ast_expr_op_un_arit_inc_pre_nameres (ctx, (ast_expr_op_un_arit_inc_pre_t* )node); break;
  case OP_ARIT_INC_POST: ast_expr_op_un_arit_inc_post_nameres(ctx, (ast_expr_op_un_arit_inc_post_t*)node); break;
  case OP_ARIT_DEC_PRE:  ast_expr_op_un_arit_dec_pre_nameres (ctx, (ast_expr_op_un_arit_dec_pre_t* )node); break;
  case OP_ARIT_DEC_POST: ast_expr_op_un_arit_dec_post_nameres(ctx, (ast_expr_op_un_arit_dec_post_t*)node); break;
  case OP_ARIT_POS:      ast_expr_op_un_arit_pos_nameres     (ctx, (ast_expr_op_un_arit_pos_t*     )node); break;
  case OP_ARIT_NEG:      ast_expr_op_un_arit_neg_nameres     (ctx, (ast_expr_op_un_arit_neg_t*     )node); break;
  case OP_BIT_NOT:       ast_expr_op_un_bit_not_nameres      (ctx, (ast_expr_op_un_bit_not_t*      )node); break;
  case OP_LOGIC_NOT:     ast_expr_op_un_logic_not_nameres    (ctx, (ast_expr_op_un_logic_not_t*    )node); break;
  case OP_IND:           ast_expr_op_un_ind_nameres          (ctx, (ast_expr_op_un_ind_t*          )node); break;
  case OP_ADDR:          ast_expr_op_un_addr_nameres         (ctx, (ast_expr_op_un_addr_t*         )node); break;
  case OP_UNWRAP_SAFE:   ast_expr_op_un_unwrap_safe_nameres  (ctx, (ast_expr_op_un_unwrap_safe_t*  )node); break;
  case OP_UNWRAP_UNSAFE: ast_expr_op_un_unwrap_unsafe_nameres(ctx, (ast_expr_op_un_unwrap_unsafe_t*)node); break;
  default: UNREACHABLE();
  }
}

void ast_expr_op_un_typecheck(typecheck_ctx_t* ctx, ast_expr_op_un_t* node)
{
  switch (node->op_kind)
  {
  case OP_SIZEOF:        ast_expr_op_un_sizeof_typecheck       (ctx, (ast_expr_op_un_sizeof_t*       )node); break;
  case OP_ALIGNOF:       ast_expr_op_un_alignof_typecheck      (ctx, (ast_expr_op_un_alignof_t*      )node); break;
  case OP_ARIT_INC_PRE:  ast_expr_op_un_arit_inc_pre_typecheck (ctx, (ast_expr_op_un_arit_inc_pre_t*      )node); break;
  case OP_ARIT_INC_POST: ast_expr_op_un_arit_inc_post_typecheck(ctx, (ast_expr_op_un_arit_inc_post_t*)node); break;
  case OP_ARIT_DEC_PRE:  ast_expr_op_un_arit_dec_pre_typecheck (ctx, (ast_expr_op_un_arit_dec_pre_t* )node); break;
  case OP_ARIT_DEC_POST: ast_expr_op_un_arit_dec_post_typecheck(ctx, (ast_expr_op_un_arit_dec_post_t*)node); break;
  case OP_ARIT_POS:      ast_expr_op_un_arit_pos_typecheck     (ctx, (ast_expr_op_un_arit_pos_t*     )node); break;
  case OP_ARIT_NEG:      ast_expr_op_un_arit_neg_typecheck     (ctx, (ast_expr_op_un_arit_neg_t*     )node); break;
  case OP_BIT_NOT:       ast_expr_op_un_bit_not_typecheck      (ctx, (ast_expr_op_un_bit_not_t*      )node); break;
  case OP_LOGIC_NOT:     ast_expr_op_un_logic_not_typecheck    (ctx, (ast_expr_op_un_logic_not_t*    )node); break;
  case OP_IND:           ast_expr_op_un_ind_typecheck          (ctx, (ast_expr_op_un_ind_t*          )node); break;
  case OP_ADDR:          ast_expr_op_un_addr_typecheck         (ctx, (ast_expr_op_un_addr_t*         )node); break;
  case OP_UNWRAP_SAFE:   ast_expr_op_un_unwrap_safe_typecheck  (ctx, (ast_expr_op_un_unwrap_safe_t*  )node); break;
  case OP_UNWRAP_UNSAFE: ast_expr_op_un_unwrap_unsafe_typecheck(ctx, (ast_expr_op_un_unwrap_unsafe_t*)node); break;
  default: UNREACHABLE();
  }
}

void ast_expr_op_un_codegen(codegen_ctx_t* ctx, ast_expr_op_un_t* node)
{
  switch (node->op_kind)
  {
  case OP_SIZEOF:        ast_expr_op_un_sizeof_codegen       (ctx, (ast_expr_op_un_sizeof_t*       )node); break;
  case OP_ALIGNOF:       ast_expr_op_un_alignof_codegen      (ctx, (ast_expr_op_un_alignof_t*      )node); break;
  case OP_ARIT_INC_PRE:  ast_expr_op_un_arit_inc_pre_codegen (ctx, (ast_expr_op_un_arit_inc_pre_t*      )node); break;
  case OP_ARIT_INC_POST: ast_expr_op_un_arit_inc_post_codegen(ctx, (ast_expr_op_un_arit_inc_post_t*)node); break;
  case OP_ARIT_DEC_PRE:  ast_expr_op_un_arit_dec_pre_codegen (ctx, (ast_expr_op_un_arit_dec_pre_t* )node); break;
  case OP_ARIT_DEC_POST: ast_expr_op_un_arit_dec_post_codegen(ctx, (ast_expr_op_un_arit_dec_post_t*)node); break;
  case OP_ARIT_POS:      ast_expr_op_un_arit_pos_codegen     (ctx, (ast_expr_op_un_arit_pos_t*     )node); break;
  case OP_ARIT_NEG:      ast_expr_op_un_arit_neg_codegen     (ctx, (ast_expr_op_un_arit_neg_t*     )node); break;
  case OP_BIT_NOT:       ast_expr_op_un_bit_not_codegen      (ctx, (ast_expr_op_un_bit_not_t*      )node); break;
  case OP_LOGIC_NOT:     ast_expr_op_un_logic_not_codegen    (ctx, (ast_expr_op_un_logic_not_t*    )node); break;
  case OP_IND:           ast_expr_op_un_ind_codegen          (ctx, (ast_expr_op_un_ind_t*          )node); break;
  case OP_ADDR:          ast_expr_op_un_addr_codegen         (ctx, (ast_expr_op_un_addr_t*         )node); break;
  case OP_UNWRAP_SAFE:   ast_expr_op_un_unwrap_safe_codegen  (ctx, (ast_expr_op_un_unwrap_safe_t*  )node); break;
  case OP_UNWRAP_UNSAFE: ast_expr_op_un_unwrap_unsafe_codegen(ctx, (ast_expr_op_un_unwrap_unsafe_t*)node); break;
  default: UNREACHABLE();
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
