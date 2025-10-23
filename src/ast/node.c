/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/node.h"

#include "ast/ast.h"

void tau_ast_node_free(tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_ID:                 tau_ast_id_free                ((tau_ast_id_t*                )node); break;
  case TAU_AST_POISON:             tau_ast_poison_free            ((tau_ast_poison_t*            )node); break;
  case TAU_AST_TYPE_ID:            tau_ast_type_id_free           ((tau_ast_type_id_t*           )node); break;
  case TAU_AST_TYPE_MUT:           tau_ast_type_mut_free          ((tau_ast_type_mut_t*          )node); break;
  case TAU_AST_TYPE_PTR:           tau_ast_type_ptr_free          ((tau_ast_type_ptr_t*          )node); break;
  case TAU_AST_TYPE_ARRAY:         tau_ast_type_array_free        ((tau_ast_type_array_t*        )node); break;
  case TAU_AST_TYPE_REF:           tau_ast_type_ref_free          ((tau_ast_type_ref_t*          )node); break;
  case TAU_AST_TYPE_OPT:           tau_ast_type_opt_free          ((tau_ast_type_opt_t*          )node); break;
  case TAU_AST_TYPE_FUN:           tau_ast_type_fun_free          ((tau_ast_type_fun_t*          )node); break;
  case TAU_AST_TYPE_VEC:           tau_ast_type_vec_free          ((tau_ast_type_vec_t*          )node); break;
  case TAU_AST_TYPE_MAT:           tau_ast_type_mat_free          ((tau_ast_type_mat_t*          )node); break;
  case TAU_AST_TYPE_PRIM_I8:
  case TAU_AST_TYPE_PRIM_I16:
  case TAU_AST_TYPE_PRIM_I32:
  case TAU_AST_TYPE_PRIM_I64:
  case TAU_AST_TYPE_PRIM_ISIZE:
  case TAU_AST_TYPE_PRIM_U8:
  case TAU_AST_TYPE_PRIM_U16:
  case TAU_AST_TYPE_PRIM_U32:
  case TAU_AST_TYPE_PRIM_U64:
  case TAU_AST_TYPE_PRIM_USIZE:
  case TAU_AST_TYPE_PRIM_F32:
  case TAU_AST_TYPE_PRIM_F64:
  case TAU_AST_TYPE_PRIM_C64:
  case TAU_AST_TYPE_PRIM_C128:
  case TAU_AST_TYPE_PRIM_CHAR:
  case TAU_AST_TYPE_PRIM_BOOL:
  case TAU_AST_TYPE_PRIM_UNIT:     tau_ast_type_prim_free         ((tau_ast_type_prim_t*         )node); break;
  case TAU_AST_TYPE_MEMBER:        tau_ast_type_mbr_free          ((tau_ast_type_mbr_t*          )node); break;
  case TAU_AST_TYPE_TYPE:          tau_ast_type_type_free         ((tau_ast_type_type_t*         )node); break;
  case TAU_AST_EXPR_ID:            tau_ast_expr_id_free           ((tau_ast_expr_id_t*           )node); break;
  case TAU_AST_EXPR_LIT_INT:       tau_ast_expr_lit_int_free      ((tau_ast_expr_lit_int_t*      )node); break;
  case TAU_AST_EXPR_LIT_FLT:       tau_ast_expr_lit_flt_free      ((tau_ast_expr_lit_flt_t*      )node); break;
  case TAU_AST_EXPR_LIT_STR:       tau_ast_expr_lit_str_free      ((tau_ast_expr_lit_str_t*      )node); break;
  case TAU_AST_EXPR_LIT_CHAR:      tau_ast_expr_lit_char_free     ((tau_ast_expr_lit_char_t*     )node); break;
  case TAU_AST_EXPR_LIT_BOOL:      tau_ast_expr_lit_bool_free     ((tau_ast_expr_lit_bool_t*     )node); break;
  case TAU_AST_EXPR_LIT_NULL:      tau_ast_expr_lit_null_free     ((tau_ast_expr_lit_null_t*     )node); break;
  case TAU_AST_EXPR_LIT_VEC:       tau_ast_expr_lit_vec_free      ((tau_ast_expr_lit_vec_t*      )node); break;
  case TAU_AST_EXPR_LIT_MAT:       tau_ast_expr_lit_mat_free      ((tau_ast_expr_lit_mat_t*      )node); break;
  case TAU_AST_EXPR_OP_UNARY:      tau_ast_expr_op_un_free        ((tau_ast_expr_op_un_t*        )node); break;
  case TAU_AST_EXPR_OP_BINARY:     tau_ast_expr_op_bin_free       ((tau_ast_expr_op_bin_t*       )node); break;
  case TAU_AST_EXPR_OP_CALL:       tau_ast_expr_op_call_free      ((tau_ast_expr_op_call_t*      )node); break;
  case TAU_AST_EXPR_OP_SPEC:       tau_ast_expr_op_spec_free      ((tau_ast_expr_op_spec_t*      )node); break;
  case TAU_AST_STMT_IF:            tau_ast_stmt_if_free           ((tau_ast_stmt_if_t*           )node); break;
  case TAU_AST_STMT_FOR:           tau_ast_stmt_for_free          ((tau_ast_stmt_for_t*          )node); break;
  case TAU_AST_STMT_WHILE:         tau_ast_stmt_while_free        ((tau_ast_stmt_while_t*        )node); break;
  case TAU_AST_STMT_DO_WHILE:      tau_ast_stmt_do_while_free     ((tau_ast_stmt_do_while_t*     )node); break;
  case TAU_AST_STMT_LOOP:          tau_ast_stmt_loop_free         ((tau_ast_stmt_loop_t*         )node); break;
  case TAU_AST_STMT_BREAK:         tau_ast_stmt_break_free        ((tau_ast_stmt_break_t*        )node); break;
  case TAU_AST_STMT_CONTINUE:      tau_ast_stmt_continue_free     ((tau_ast_stmt_continue_t*     )node); break;
  case TAU_AST_STMT_RETURN:        tau_ast_stmt_return_free       ((tau_ast_stmt_return_t*       )node); break;
  case TAU_AST_STMT_DEFER:         tau_ast_stmt_defer_free        ((tau_ast_stmt_defer_t*        )node); break;
  case TAU_AST_STMT_BLOCK:         tau_ast_stmt_block_free        ((tau_ast_stmt_block_t*        )node); break;
  case TAU_AST_STMT_EXPR:          tau_ast_stmt_expr_free         ((tau_ast_stmt_expr_t*         )node); break;
  case TAU_AST_DECL_VAR:           tau_ast_decl_var_free          ((tau_ast_decl_var_t*          )node); break;
  case TAU_AST_DECL_PARAM:         tau_ast_decl_param_free        ((tau_ast_decl_param_t*        )node); break;
  case TAU_AST_DECL_FUN:           tau_ast_decl_fun_free          ((tau_ast_decl_fun_t*          )node); break;
  case TAU_AST_DECL_STRUCT:        tau_ast_decl_struct_free       ((tau_ast_decl_struct_t*       )node); break;
  case TAU_AST_DECL_UNION:         tau_ast_decl_union_free        ((tau_ast_decl_union_t*        )node); break;
  case TAU_AST_DECL_ENUM:          tau_ast_decl_enum_free         ((tau_ast_decl_enum_t*         )node); break;
  case TAU_AST_DECL_ENUM_CONSTANT: tau_ast_decl_enum_constant_free((tau_ast_decl_enum_constant_t*)node); break;
  case TAU_AST_DECL_MOD:           tau_ast_decl_mod_free          ((tau_ast_decl_mod_t*          )node); break;
  case TAU_AST_DECL_TYPE_ALIAS:    tau_ast_decl_type_alias_free   ((tau_ast_decl_type_alias_t*   )node); break;
  case TAU_AST_DECL_GENERIC_FUN:   tau_ast_decl_generic_fun_free  ((tau_ast_decl_generic_fun_t*  )node); break;
  case TAU_AST_DECL_GENERIC_PARAM: tau_ast_decl_generic_param_free((tau_ast_decl_generic_param_t*)node); break;
  case TAU_AST_PATH_SEGMENT:       tau_ast_path_segment_free      ((tau_ast_path_segment_t*      )node); break;
  case TAU_AST_PATH_ACCESS:        tau_ast_path_access_free       ((tau_ast_path_access_t*       )node); break;
  case TAU_AST_PATH_LIST:          tau_ast_path_list_free         ((tau_ast_path_list_t*         )node); break;
  case TAU_AST_PATH_WILDCARD:      tau_ast_path_wildcard_free     ((tau_ast_path_wildcard_t*     )node); break;
  case TAU_AST_PATH_ALIAS:         tau_ast_path_alias_free        ((tau_ast_path_alias_t*        )node); break;
  case TAU_AST_USE:                tau_ast_use_free               ((tau_ast_use_t*               )node); break;
  case TAU_AST_PROG:               tau_ast_prog_free              ((tau_ast_prog_t*              )node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_node_nameres(tau_nameres_ctx_t* ctx, tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_ID:
  case TAU_AST_POISON:
  case TAU_AST_PATH_SEGMENT:
  case TAU_AST_PATH_ACCESS:
  case TAU_AST_PATH_LIST:
  case TAU_AST_PATH_WILDCARD:
  case TAU_AST_PATH_ALIAS:
  case TAU_AST_USE:                break;
  case TAU_AST_TYPE_ID:            tau_ast_type_id_nameres           (ctx, (tau_ast_type_id_t*           )node); break;
  case TAU_AST_TYPE_MUT:           tau_ast_type_mut_nameres          (ctx, (tau_ast_type_mut_t*          )node); break;
  case TAU_AST_TYPE_PTR:           tau_ast_type_ptr_nameres          (ctx, (tau_ast_type_ptr_t*          )node); break;
  case TAU_AST_TYPE_ARRAY:         tau_ast_type_array_nameres        (ctx, (tau_ast_type_array_t*        )node); break;
  case TAU_AST_TYPE_REF:           tau_ast_type_ref_nameres          (ctx, (tau_ast_type_ref_t*          )node); break;
  case TAU_AST_TYPE_OPT:           tau_ast_type_opt_nameres          (ctx, (tau_ast_type_opt_t*          )node); break;
  case TAU_AST_TYPE_FUN:           tau_ast_type_fun_nameres          (ctx, (tau_ast_type_fun_t*          )node); break;
  case TAU_AST_TYPE_VEC:           tau_ast_type_vec_nameres          (ctx, (tau_ast_type_vec_t*          )node); break;
  case TAU_AST_TYPE_MAT:           tau_ast_type_mat_nameres          (ctx, (tau_ast_type_mat_t*          )node); break;
  case TAU_AST_TYPE_PRIM_I8:
  case TAU_AST_TYPE_PRIM_I16:
  case TAU_AST_TYPE_PRIM_I32:
  case TAU_AST_TYPE_PRIM_I64:
  case TAU_AST_TYPE_PRIM_ISIZE:
  case TAU_AST_TYPE_PRIM_U8:
  case TAU_AST_TYPE_PRIM_U16:
  case TAU_AST_TYPE_PRIM_U32:
  case TAU_AST_TYPE_PRIM_U64:
  case TAU_AST_TYPE_PRIM_USIZE:
  case TAU_AST_TYPE_PRIM_F32:
  case TAU_AST_TYPE_PRIM_F64:
  case TAU_AST_TYPE_PRIM_C64:
  case TAU_AST_TYPE_PRIM_C128:
  case TAU_AST_TYPE_PRIM_CHAR:
  case TAU_AST_TYPE_PRIM_BOOL:
  case TAU_AST_TYPE_PRIM_UNIT:     tau_ast_type_prim_nameres         (ctx, (tau_ast_type_prim_t*         )node); break;
  case TAU_AST_TYPE_MEMBER:        tau_ast_type_mbr_nameres          (ctx, (tau_ast_type_mbr_t*          )node); break;
  case TAU_AST_TYPE_TYPE:          tau_ast_type_type_nameres         (ctx, (tau_ast_type_type_t*         )node); break;
  case TAU_AST_EXPR_ID:            tau_ast_expr_id_nameres           (ctx, (tau_ast_expr_id_t*           )node); break;
  case TAU_AST_EXPR_LIT_INT:       tau_ast_expr_lit_int_nameres      (ctx, (tau_ast_expr_lit_int_t*      )node); break;
  case TAU_AST_EXPR_LIT_FLT:       tau_ast_expr_lit_flt_nameres      (ctx, (tau_ast_expr_lit_flt_t*      )node); break;
  case TAU_AST_EXPR_LIT_STR:       tau_ast_expr_lit_str_nameres      (ctx, (tau_ast_expr_lit_str_t*      )node); break;
  case TAU_AST_EXPR_LIT_CHAR:      tau_ast_expr_lit_char_nameres     (ctx, (tau_ast_expr_lit_char_t*     )node); break;
  case TAU_AST_EXPR_LIT_BOOL:      tau_ast_expr_lit_bool_nameres     (ctx, (tau_ast_expr_lit_bool_t*     )node); break;
  case TAU_AST_EXPR_LIT_NULL:      tau_ast_expr_lit_null_nameres     (ctx, (tau_ast_expr_lit_null_t*     )node); break;
  case TAU_AST_EXPR_LIT_VEC:       tau_ast_expr_lit_vec_nameres      (ctx, (tau_ast_expr_lit_vec_t*      )node); break;
  case TAU_AST_EXPR_LIT_MAT:       tau_ast_expr_lit_mat_nameres      (ctx, (tau_ast_expr_lit_mat_t*      )node); break;
  case TAU_AST_EXPR_OP_UNARY:      tau_ast_expr_op_un_nameres        (ctx, (tau_ast_expr_op_un_t*        )node); break;
  case TAU_AST_EXPR_OP_BINARY:     tau_ast_expr_op_bin_nameres       (ctx, (tau_ast_expr_op_bin_t*       )node); break;
  case TAU_AST_EXPR_OP_CALL:       tau_ast_expr_op_call_nameres      (ctx, (tau_ast_expr_op_call_t*      )node); break;
  case TAU_AST_EXPR_OP_SPEC:       tau_ast_expr_op_spec_nameres      (ctx, (tau_ast_expr_op_spec_t*      )node); break;
  case TAU_AST_STMT_IF:            tau_ast_stmt_if_nameres           (ctx, (tau_ast_stmt_if_t*           )node); break;
  case TAU_AST_STMT_FOR:           tau_ast_stmt_for_nameres          (ctx, (tau_ast_stmt_for_t*          )node); break;
  case TAU_AST_STMT_WHILE:         tau_ast_stmt_while_nameres        (ctx, (tau_ast_stmt_while_t*        )node); break;
  case TAU_AST_STMT_DO_WHILE:      tau_ast_stmt_do_while_nameres     (ctx, (tau_ast_stmt_do_while_t*     )node); break;
  case TAU_AST_STMT_LOOP:          tau_ast_stmt_loop_nameres         (ctx, (tau_ast_stmt_loop_t*         )node); break;
  case TAU_AST_STMT_BREAK:         tau_ast_stmt_break_nameres        (ctx, (tau_ast_stmt_break_t*        )node); break;
  case TAU_AST_STMT_CONTINUE:      tau_ast_stmt_continue_nameres     (ctx, (tau_ast_stmt_continue_t*     )node); break;
  case TAU_AST_STMT_RETURN:        tau_ast_stmt_return_nameres       (ctx, (tau_ast_stmt_return_t*       )node); break;
  case TAU_AST_STMT_DEFER:         tau_ast_stmt_defer_nameres        (ctx, (tau_ast_stmt_defer_t*        )node); break;
  case TAU_AST_STMT_BLOCK:         tau_ast_stmt_block_nameres        (ctx, (tau_ast_stmt_block_t*        )node); break;
  case TAU_AST_STMT_EXPR:          tau_ast_stmt_expr_nameres         (ctx, (tau_ast_stmt_expr_t*         )node); break;
  case TAU_AST_DECL_VAR:           tau_ast_decl_var_nameres          (ctx, (tau_ast_decl_var_t*          )node); break;
  case TAU_AST_DECL_PARAM:         tau_ast_decl_param_nameres        (ctx, (tau_ast_decl_param_t*        )node); break;
  case TAU_AST_DECL_FUN:           tau_ast_decl_fun_nameres          (ctx, (tau_ast_decl_fun_t*          )node); break;
  case TAU_AST_DECL_STRUCT:        tau_ast_decl_struct_nameres       (ctx, (tau_ast_decl_struct_t*       )node); break;
  case TAU_AST_DECL_UNION:         tau_ast_decl_union_nameres        (ctx, (tau_ast_decl_union_t*        )node); break;
  case TAU_AST_DECL_ENUM:          tau_ast_decl_enum_nameres         (ctx, (tau_ast_decl_enum_t*         )node); break;
  case TAU_AST_DECL_ENUM_CONSTANT: tau_ast_decl_enum_constant_nameres(ctx, (tau_ast_decl_enum_constant_t*)node); break;
  case TAU_AST_DECL_MOD:           tau_ast_decl_mod_nameres          (ctx, (tau_ast_decl_mod_t*          )node); break;
  case TAU_AST_DECL_TYPE_ALIAS:    tau_ast_decl_type_alias_nameres   (ctx, (tau_ast_decl_type_alias_t*   )node); break;
  case TAU_AST_DECL_GENERIC_FUN:   tau_ast_decl_generic_fun_nameres  (ctx, (tau_ast_decl_generic_fun_t*  )node); break;
  case TAU_AST_DECL_GENERIC_PARAM: tau_ast_decl_generic_param_nameres(ctx, (tau_ast_decl_generic_param_t*)node); break;
  case TAU_AST_PROG:               tau_ast_prog_nameres              (ctx, (tau_ast_prog_t*              )node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_node_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_ID:
  case TAU_AST_POISON:
  case TAU_AST_DECL_GENERIC_FUN:
  case TAU_AST_DECL_GENERIC_PARAM:
  case TAU_AST_PATH_SEGMENT:
  case TAU_AST_PATH_ACCESS:
  case TAU_AST_PATH_LIST:
  case TAU_AST_PATH_WILDCARD:
  case TAU_AST_PATH_ALIAS:
  case TAU_AST_USE:                break;
  case TAU_AST_TYPE_ID:            tau_ast_type_id_typecheck           (ctx, (tau_ast_type_id_t*           )node); break;
  case TAU_AST_TYPE_MUT:           tau_ast_type_mut_typecheck          (ctx, (tau_ast_type_mut_t*          )node); break;
  case TAU_AST_TYPE_PTR:           tau_ast_type_ptr_typecheck          (ctx, (tau_ast_type_ptr_t*          )node); break;
  case TAU_AST_TYPE_ARRAY:         tau_ast_type_array_typecheck        (ctx, (tau_ast_type_array_t*        )node); break;
  case TAU_AST_TYPE_REF:           tau_ast_type_ref_typecheck          (ctx, (tau_ast_type_ref_t*          )node); break;
  case TAU_AST_TYPE_OPT:           tau_ast_type_opt_typecheck          (ctx, (tau_ast_type_opt_t*          )node); break;
  case TAU_AST_TYPE_FUN:           tau_ast_type_fun_typecheck          (ctx, (tau_ast_type_fun_t*          )node); break;
  case TAU_AST_TYPE_VEC:           tau_ast_type_vec_typecheck          (ctx, (tau_ast_type_vec_t*          )node); break;
  case TAU_AST_TYPE_MAT:           tau_ast_type_mat_typecheck          (ctx, (tau_ast_type_mat_t*          )node); break;
  case TAU_AST_TYPE_PRIM_I8:
  case TAU_AST_TYPE_PRIM_I16:
  case TAU_AST_TYPE_PRIM_I32:
  case TAU_AST_TYPE_PRIM_I64:
  case TAU_AST_TYPE_PRIM_ISIZE:
  case TAU_AST_TYPE_PRIM_U8:
  case TAU_AST_TYPE_PRIM_U16:
  case TAU_AST_TYPE_PRIM_U32:
  case TAU_AST_TYPE_PRIM_U64:
  case TAU_AST_TYPE_PRIM_USIZE:
  case TAU_AST_TYPE_PRIM_F32:
  case TAU_AST_TYPE_PRIM_F64:
  case TAU_AST_TYPE_PRIM_C64:
  case TAU_AST_TYPE_PRIM_C128:
  case TAU_AST_TYPE_PRIM_CHAR:
  case TAU_AST_TYPE_PRIM_BOOL:
  case TAU_AST_TYPE_PRIM_UNIT:     tau_ast_type_prim_typecheck         (ctx, (tau_ast_type_prim_t*         )node); break;
  case TAU_AST_TYPE_MEMBER:        tau_ast_type_mbr_typecheck          (ctx, (tau_ast_type_mbr_t*          )node); break;
  case TAU_AST_TYPE_TYPE:          tau_ast_type_type_typecheck         (ctx, (tau_ast_type_type_t*         )node); break;
  case TAU_AST_EXPR_ID:            tau_ast_expr_id_typecheck           (ctx, (tau_ast_expr_id_t*           )node); break;
  case TAU_AST_EXPR_LIT_INT:       tau_ast_expr_lit_int_typecheck      (ctx, (tau_ast_expr_lit_int_t*      )node); break;
  case TAU_AST_EXPR_LIT_FLT:       tau_ast_expr_lit_flt_typecheck      (ctx, (tau_ast_expr_lit_flt_t*      )node); break;
  case TAU_AST_EXPR_LIT_STR:       tau_ast_expr_lit_str_typecheck      (ctx, (tau_ast_expr_lit_str_t*      )node); break;
  case TAU_AST_EXPR_LIT_CHAR:      tau_ast_expr_lit_char_typecheck     (ctx, (tau_ast_expr_lit_char_t*     )node); break;
  case TAU_AST_EXPR_LIT_BOOL:      tau_ast_expr_lit_bool_typecheck     (ctx, (tau_ast_expr_lit_bool_t*     )node); break;
  case TAU_AST_EXPR_LIT_NULL:      tau_ast_expr_lit_null_typecheck     (ctx, (tau_ast_expr_lit_null_t*     )node); break;
  case TAU_AST_EXPR_LIT_VEC:       tau_ast_expr_lit_vec_typecheck      (ctx, (tau_ast_expr_lit_vec_t*      )node); break;
  case TAU_AST_EXPR_LIT_MAT:       tau_ast_expr_lit_mat_typecheck      (ctx, (tau_ast_expr_lit_mat_t*      )node); break;
  case TAU_AST_EXPR_OP_UNARY:      tau_ast_expr_op_un_typecheck        (ctx, (tau_ast_expr_op_un_t*        )node); break;
  case TAU_AST_EXPR_OP_BINARY:     tau_ast_expr_op_bin_typecheck       (ctx, (tau_ast_expr_op_bin_t*       )node); break;
  case TAU_AST_EXPR_OP_CALL:       tau_ast_expr_op_call_typecheck      (ctx, (tau_ast_expr_op_call_t*      )node); break;
  case TAU_AST_EXPR_OP_SPEC:       tau_ast_expr_op_spec_typecheck      (ctx, (tau_ast_expr_op_spec_t*      )node); break;
  case TAU_AST_STMT_IF:            tau_ast_stmt_if_typecheck           (ctx, (tau_ast_stmt_if_t*           )node); break;
  case TAU_AST_STMT_FOR:           tau_ast_stmt_for_typecheck          (ctx, (tau_ast_stmt_for_t*          )node); break;
  case TAU_AST_STMT_WHILE:         tau_ast_stmt_while_typecheck        (ctx, (tau_ast_stmt_while_t*        )node); break;
  case TAU_AST_STMT_DO_WHILE:      tau_ast_stmt_do_while_typecheck     (ctx, (tau_ast_stmt_do_while_t*     )node); break;
  case TAU_AST_STMT_LOOP:          tau_ast_stmt_loop_typecheck         (ctx, (tau_ast_stmt_loop_t*         )node); break;
  case TAU_AST_STMT_BREAK:         tau_ast_stmt_break_typecheck        (ctx, (tau_ast_stmt_break_t*        )node); break;
  case TAU_AST_STMT_CONTINUE:      tau_ast_stmt_continue_typecheck     (ctx, (tau_ast_stmt_continue_t*     )node); break;
  case TAU_AST_STMT_RETURN:        tau_ast_stmt_return_typecheck       (ctx, (tau_ast_stmt_return_t*       )node); break;
  case TAU_AST_STMT_DEFER:         tau_ast_stmt_defer_typecheck        (ctx, (tau_ast_stmt_defer_t*        )node); break;
  case TAU_AST_STMT_BLOCK:         tau_ast_stmt_block_typecheck        (ctx, (tau_ast_stmt_block_t*        )node); break;
  case TAU_AST_STMT_EXPR:          tau_ast_stmt_expr_typecheck         (ctx, (tau_ast_stmt_expr_t*         )node); break;
  case TAU_AST_DECL_VAR:           tau_ast_decl_var_typecheck          (ctx, (tau_ast_decl_var_t*          )node); break;
  case TAU_AST_DECL_PARAM:         tau_ast_decl_param_typecheck        (ctx, (tau_ast_decl_param_t*        )node); break;
  case TAU_AST_DECL_FUN:           tau_ast_decl_fun_typecheck          (ctx, (tau_ast_decl_fun_t*          )node); break;
  case TAU_AST_DECL_STRUCT:        tau_ast_decl_struct_typecheck       (ctx, (tau_ast_decl_struct_t*       )node); break;
  case TAU_AST_DECL_UNION:         tau_ast_decl_union_typecheck        (ctx, (tau_ast_decl_union_t*        )node); break;
  case TAU_AST_DECL_ENUM:          tau_ast_decl_enum_typecheck         (ctx, (tau_ast_decl_enum_t*         )node); break;
  case TAU_AST_DECL_ENUM_CONSTANT: tau_ast_decl_enum_constant_typecheck(ctx, (tau_ast_decl_enum_constant_t*)node); break;
  case TAU_AST_DECL_MOD:           tau_ast_decl_mod_typecheck          (ctx, (tau_ast_decl_mod_t*          )node); break;
  case TAU_AST_DECL_TYPE_ALIAS:    tau_ast_decl_type_alias_typecheck   (ctx, (tau_ast_decl_type_alias_t*   )node); break;
  case TAU_AST_PROG:               tau_ast_prog_typecheck              (ctx, (tau_ast_prog_t*              )node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_node_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_ID:
  case TAU_AST_POISON:
  case TAU_AST_TYPE_ID:
  case TAU_AST_TYPE_MUT:
  case TAU_AST_TYPE_CONST:
  case TAU_AST_TYPE_PTR:
  case TAU_AST_TYPE_ARRAY:
  case TAU_AST_TYPE_REF:
  case TAU_AST_TYPE_OPT:
  case TAU_AST_TYPE_FUN:
  case TAU_AST_TYPE_VEC:
  case TAU_AST_TYPE_MAT:
  case TAU_AST_TYPE_PRIM_I8:
  case TAU_AST_TYPE_PRIM_I16:
  case TAU_AST_TYPE_PRIM_I32:
  case TAU_AST_TYPE_PRIM_I64:
  case TAU_AST_TYPE_PRIM_ISIZE:
  case TAU_AST_TYPE_PRIM_U8:
  case TAU_AST_TYPE_PRIM_U16:
  case TAU_AST_TYPE_PRIM_U32:
  case TAU_AST_TYPE_PRIM_U64:
  case TAU_AST_TYPE_PRIM_USIZE:
  case TAU_AST_TYPE_PRIM_F32:
  case TAU_AST_TYPE_PRIM_F64:
  case TAU_AST_TYPE_PRIM_C64:
  case TAU_AST_TYPE_PRIM_C128:
  case TAU_AST_TYPE_PRIM_CHAR:
  case TAU_AST_TYPE_PRIM_BOOL:
  case TAU_AST_TYPE_PRIM_UNIT:
  case TAU_AST_TYPE_MEMBER:
  case TAU_AST_TYPE_TYPE:
  case TAU_AST_EXPR_ID:
  case TAU_AST_EXPR_LIT_INT:
  case TAU_AST_EXPR_LIT_FLT:
  case TAU_AST_EXPR_LIT_STR:
  case TAU_AST_EXPR_LIT_CHAR:
  case TAU_AST_EXPR_LIT_BOOL:
  case TAU_AST_EXPR_LIT_NULL:
  case TAU_AST_EXPR_LIT_VEC:
  case TAU_AST_EXPR_LIT_MAT:
  case TAU_AST_EXPR_OP_UNARY:
  case TAU_AST_EXPR_OP_BINARY:
  case TAU_AST_EXPR_OP_CALL:
  case TAU_AST_EXPR_OP_SPEC:
  case TAU_AST_STMT_EXPR:
  case TAU_AST_DECL_VAR:
  case TAU_AST_DECL_PARAM:
  case TAU_AST_DECL_STRUCT:
  case TAU_AST_DECL_UNION:
  case TAU_AST_DECL_ENUM:
  case TAU_AST_DECL_ENUM_CONSTANT:
  case TAU_AST_DECL_TYPE_ALIAS:
  case TAU_AST_DECL_GENERIC_FUN:
  case TAU_AST_DECL_GENERIC_PARAM:
  case TAU_AST_PATH_SEGMENT:
  case TAU_AST_PATH_ACCESS:
  case TAU_AST_PATH_LIST:
  case TAU_AST_PATH_WILDCARD:
  case TAU_AST_PATH_ALIAS:
  case TAU_AST_USE:                break;
  case TAU_AST_STMT_IF:            tau_ast_stmt_if_ctrlflow           (ctx, (tau_ast_stmt_if_t*           )node); break;
  case TAU_AST_STMT_FOR:           tau_ast_stmt_for_ctrlflow          (ctx, (tau_ast_stmt_for_t*          )node); break;
  case TAU_AST_STMT_WHILE:         tau_ast_stmt_while_ctrlflow        (ctx, (tau_ast_stmt_while_t*        )node); break;
  case TAU_AST_STMT_DO_WHILE:      tau_ast_stmt_do_while_ctrlflow     (ctx, (tau_ast_stmt_do_while_t*     )node); break;
  case TAU_AST_STMT_LOOP:          tau_ast_stmt_loop_ctrlflow         (ctx, (tau_ast_stmt_loop_t*         )node); break;
  case TAU_AST_STMT_BREAK:         tau_ast_stmt_break_ctrlflow        (ctx, (tau_ast_stmt_break_t*        )node); break;
  case TAU_AST_STMT_CONTINUE:      tau_ast_stmt_continue_ctrlflow     (ctx, (tau_ast_stmt_continue_t*     )node); break;
  case TAU_AST_STMT_RETURN:        tau_ast_stmt_return_ctrlflow       (ctx, (tau_ast_stmt_return_t*       )node); break;
  case TAU_AST_STMT_DEFER:         tau_ast_stmt_defer_ctrlflow        (ctx, (tau_ast_stmt_defer_t*        )node); break;
  case TAU_AST_STMT_BLOCK:         tau_ast_stmt_block_ctrlflow        (ctx, (tau_ast_stmt_block_t*        )node); break;
  case TAU_AST_DECL_FUN:           tau_ast_decl_fun_ctrlflow          (ctx, (tau_ast_decl_fun_t*          )node); break;
  case TAU_AST_DECL_MOD:           tau_ast_decl_mod_ctrlflow          (ctx, (tau_ast_decl_mod_t*          )node); break;
  case TAU_AST_PROG:               tau_ast_prog_ctrlflow              (ctx, (tau_ast_prog_t*              )node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_node_codegen(tau_codegen_ctx_t* ctx, tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_ID:
  case TAU_AST_POISON:
  case TAU_AST_DECL_GENERIC_FUN:
  case TAU_AST_DECL_GENERIC_PARAM:
  case TAU_AST_PATH_SEGMENT:
  case TAU_AST_PATH_ACCESS:
  case TAU_AST_PATH_LIST:
  case TAU_AST_PATH_WILDCARD:
  case TAU_AST_PATH_ALIAS:
  case TAU_AST_USE:                break;
  case TAU_AST_TYPE_ID:            tau_ast_type_id_codegen           (ctx, (tau_ast_type_id_t*           )node); break;
  case TAU_AST_TYPE_MUT:           tau_ast_type_mut_codegen          (ctx, (tau_ast_type_mut_t*          )node); break;
  case TAU_AST_TYPE_PTR:           tau_ast_type_ptr_codegen          (ctx, (tau_ast_type_ptr_t*          )node); break;
  case TAU_AST_TYPE_ARRAY:         tau_ast_type_array_codegen        (ctx, (tau_ast_type_array_t*        )node); break;
  case TAU_AST_TYPE_REF:           tau_ast_type_ref_codegen          (ctx, (tau_ast_type_ref_t*          )node); break;
  case TAU_AST_TYPE_OPT:           tau_ast_type_opt_codegen          (ctx, (tau_ast_type_opt_t*          )node); break;
  case TAU_AST_TYPE_FUN:           tau_ast_type_fun_codegen          (ctx, (tau_ast_type_fun_t*          )node); break;
  case TAU_AST_TYPE_VEC:           tau_ast_type_vec_codegen          (ctx, (tau_ast_type_vec_t*          )node); break;
  case TAU_AST_TYPE_MAT:           tau_ast_type_mat_codegen          (ctx, (tau_ast_type_mat_t*          )node); break;
  case TAU_AST_TYPE_PRIM_I8:
  case TAU_AST_TYPE_PRIM_I16:
  case TAU_AST_TYPE_PRIM_I32:
  case TAU_AST_TYPE_PRIM_I64:
  case TAU_AST_TYPE_PRIM_ISIZE:
  case TAU_AST_TYPE_PRIM_U8:
  case TAU_AST_TYPE_PRIM_U16:
  case TAU_AST_TYPE_PRIM_U32:
  case TAU_AST_TYPE_PRIM_U64:
  case TAU_AST_TYPE_PRIM_USIZE:
  case TAU_AST_TYPE_PRIM_F32:
  case TAU_AST_TYPE_PRIM_F64:
  case TAU_AST_TYPE_PRIM_C64:
  case TAU_AST_TYPE_PRIM_C128:
  case TAU_AST_TYPE_PRIM_CHAR:
  case TAU_AST_TYPE_PRIM_BOOL:
  case TAU_AST_TYPE_PRIM_UNIT:     tau_ast_type_prim_codegen         (ctx, (tau_ast_type_prim_t*         )node); break;
  case TAU_AST_TYPE_MEMBER:        tau_ast_type_mbr_codegen          (ctx, (tau_ast_type_mbr_t*          )node); break;
  case TAU_AST_TYPE_TYPE:          tau_ast_type_type_codegen         (ctx, (tau_ast_type_type_t*         )node); break;
  case TAU_AST_EXPR_ID:            tau_ast_expr_id_codegen           (ctx, (tau_ast_expr_id_t*           )node); break;
  case TAU_AST_EXPR_LIT_INT:       tau_ast_expr_lit_int_codegen      (ctx, (tau_ast_expr_lit_int_t*      )node); break;
  case TAU_AST_EXPR_LIT_FLT:       tau_ast_expr_lit_flt_codegen      (ctx, (tau_ast_expr_lit_flt_t*      )node); break;
  case TAU_AST_EXPR_LIT_STR:       tau_ast_expr_lit_str_codegen      (ctx, (tau_ast_expr_lit_str_t*      )node); break;
  case TAU_AST_EXPR_LIT_CHAR:      tau_ast_expr_lit_char_codegen     (ctx, (tau_ast_expr_lit_char_t*     )node); break;
  case TAU_AST_EXPR_LIT_BOOL:      tau_ast_expr_lit_bool_codegen     (ctx, (tau_ast_expr_lit_bool_t*     )node); break;
  case TAU_AST_EXPR_LIT_NULL:      tau_ast_expr_lit_null_codegen     (ctx, (tau_ast_expr_lit_null_t*     )node); break;
  case TAU_AST_EXPR_LIT_VEC:       tau_ast_expr_lit_vec_codegen      (ctx, (tau_ast_expr_lit_vec_t*      )node); break;
  case TAU_AST_EXPR_LIT_MAT:       tau_ast_expr_lit_mat_codegen      (ctx, (tau_ast_expr_lit_mat_t*      )node); break;
  case TAU_AST_EXPR_OP_UNARY:      tau_ast_expr_op_un_codegen        (ctx, (tau_ast_expr_op_un_t*        )node); break;
  case TAU_AST_EXPR_OP_BINARY:     tau_ast_expr_op_bin_codegen       (ctx, (tau_ast_expr_op_bin_t*       )node); break;
  case TAU_AST_EXPR_OP_CALL:       tau_ast_expr_op_call_codegen      (ctx, (tau_ast_expr_op_call_t*      )node); break;
  case TAU_AST_EXPR_OP_SPEC:       tau_ast_expr_op_spec_codegen      (ctx, (tau_ast_expr_op_spec_t*      )node); break;
  case TAU_AST_STMT_IF:            tau_ast_stmt_if_codegen           (ctx, (tau_ast_stmt_if_t*           )node); break;
  case TAU_AST_STMT_FOR:           tau_ast_stmt_for_codegen          (ctx, (tau_ast_stmt_for_t*          )node); break;
  case TAU_AST_STMT_WHILE:         tau_ast_stmt_while_codegen        (ctx, (tau_ast_stmt_while_t*        )node); break;
  case TAU_AST_STMT_DO_WHILE:      tau_ast_stmt_do_while_codegen     (ctx, (tau_ast_stmt_do_while_t*     )node); break;
  case TAU_AST_STMT_LOOP:          tau_ast_stmt_loop_codegen         (ctx, (tau_ast_stmt_loop_t*         )node); break;
  case TAU_AST_STMT_BREAK:         tau_ast_stmt_break_codegen        (ctx, (tau_ast_stmt_break_t*        )node); break;
  case TAU_AST_STMT_CONTINUE:      tau_ast_stmt_continue_codegen     (ctx, (tau_ast_stmt_continue_t*     )node); break;
  case TAU_AST_STMT_RETURN:        tau_ast_stmt_return_codegen       (ctx, (tau_ast_stmt_return_t*       )node); break;
  case TAU_AST_STMT_DEFER:         tau_ast_stmt_defer_codegen        (ctx, (tau_ast_stmt_defer_t*        )node); break;
  case TAU_AST_STMT_BLOCK:         tau_ast_stmt_block_codegen        (ctx, (tau_ast_stmt_block_t*        )node); break;
  case TAU_AST_STMT_EXPR:          tau_ast_stmt_expr_codegen         (ctx, (tau_ast_stmt_expr_t*         )node); break;
  case TAU_AST_DECL_VAR:           tau_ast_decl_var_codegen          (ctx, (tau_ast_decl_var_t*          )node); break;
  case TAU_AST_DECL_PARAM:         tau_ast_decl_param_codegen        (ctx, (tau_ast_decl_param_t*        )node); break;
  case TAU_AST_DECL_FUN:           tau_ast_decl_fun_codegen          (ctx, (tau_ast_decl_fun_t*          )node); break;
  case TAU_AST_DECL_STRUCT:        tau_ast_decl_struct_codegen       (ctx, (tau_ast_decl_struct_t*       )node); break;
  case TAU_AST_DECL_UNION:         tau_ast_decl_union_codegen        (ctx, (tau_ast_decl_union_t*        )node); break;
  case TAU_AST_DECL_ENUM:          tau_ast_decl_enum_codegen         (ctx, (tau_ast_decl_enum_t*         )node); break;
  case TAU_AST_DECL_ENUM_CONSTANT: tau_ast_decl_enum_constant_codegen(ctx, (tau_ast_decl_enum_constant_t*)node); break;
  case TAU_AST_DECL_MOD:           tau_ast_decl_mod_codegen          (ctx, (tau_ast_decl_mod_t*          )node); break;
  case TAU_AST_DECL_TYPE_ALIAS:    tau_ast_decl_type_alias_codegen   (ctx, (tau_ast_decl_type_alias_t*   )node); break;
  case TAU_AST_PROG:               tau_ast_prog_codegen              (ctx, (tau_ast_prog_t*              )node); break;
  default: TAU_UNREACHABLE();
  }
}

void tau_ast_node_dump_json_vector(FILE* stream, tau_vector_t* vec)
{
  if (vec == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  fputc('[', stream);

  TAU_VECTOR_FOR_LOOP(i, vec)
  {
    tau_ast_node_dump_json(stream, (tau_ast_node_t*)tau_vector_get(vec, i));

    if (i + 1 < tau_vector_size(vec))
      fputc(',', stream);
  }

  fputc(']', stream);
}

void tau_ast_node_dump_json(FILE* stream, tau_ast_node_t* node)
{
  if (node == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  switch (node->kind)
  {
  case TAU_AST_ID:                 tau_ast_id_dump_json                (stream, (tau_ast_id_t*                )node); break;
  case TAU_AST_POISON:             tau_ast_poison_dump_json            (stream, (tau_ast_poison_t*            )node); break;
  case TAU_AST_TYPE_ID:            tau_ast_type_id_dump_json           (stream, (tau_ast_type_id_t*           )node); break;
  case TAU_AST_TYPE_MUT:           tau_ast_type_mut_dump_json          (stream, (tau_ast_type_mut_t*          )node); break;
  case TAU_AST_TYPE_PTR:           tau_ast_type_ptr_dump_json          (stream, (tau_ast_type_ptr_t*          )node); break;
  case TAU_AST_TYPE_ARRAY:         tau_ast_type_array_dump_json        (stream, (tau_ast_type_array_t*        )node); break;
  case TAU_AST_TYPE_REF:           tau_ast_type_ref_dump_json          (stream, (tau_ast_type_ref_t*          )node); break;
  case TAU_AST_TYPE_OPT:           tau_ast_type_opt_dump_json          (stream, (tau_ast_type_opt_t*          )node); break;
  case TAU_AST_TYPE_FUN:           tau_ast_type_fun_dump_json          (stream, (tau_ast_type_fun_t*          )node); break;
  case TAU_AST_TYPE_VEC:           tau_ast_type_vec_dump_json          (stream, (tau_ast_type_vec_t*          )node); break;
  case TAU_AST_TYPE_MAT:           tau_ast_type_mat_dump_json          (stream, (tau_ast_type_mat_t*          )node); break;
  case TAU_AST_TYPE_PRIM_I8:
  case TAU_AST_TYPE_PRIM_I16:
  case TAU_AST_TYPE_PRIM_I32:
  case TAU_AST_TYPE_PRIM_I64:
  case TAU_AST_TYPE_PRIM_ISIZE:
  case TAU_AST_TYPE_PRIM_U8:
  case TAU_AST_TYPE_PRIM_U16:
  case TAU_AST_TYPE_PRIM_U32:
  case TAU_AST_TYPE_PRIM_U64:
  case TAU_AST_TYPE_PRIM_USIZE:
  case TAU_AST_TYPE_PRIM_F32:
  case TAU_AST_TYPE_PRIM_F64:
  case TAU_AST_TYPE_PRIM_C64:
  case TAU_AST_TYPE_PRIM_C128:
  case TAU_AST_TYPE_PRIM_CHAR:
  case TAU_AST_TYPE_PRIM_BOOL:
  case TAU_AST_TYPE_PRIM_UNIT:     tau_ast_type_prim_dump_json         (stream, (tau_ast_type_prim_t*         )node); break;
  case TAU_AST_TYPE_MEMBER:        tau_ast_type_mbr_dump_json          (stream, (tau_ast_type_mbr_t*          )node); break;
  case TAU_AST_TYPE_TYPE:          tau_ast_type_type_dump_json         (stream, (tau_ast_type_type_t*         )node); break;
  case TAU_AST_EXPR_ID:            tau_ast_expr_id_dump_json           (stream, (tau_ast_expr_id_t*           )node); break;
  case TAU_AST_EXPR_LIT_INT:       tau_ast_expr_lit_int_dump_json      (stream, (tau_ast_expr_lit_int_t*      )node); break;
  case TAU_AST_EXPR_LIT_FLT:       tau_ast_expr_lit_flt_dump_json      (stream, (tau_ast_expr_lit_flt_t*      )node); break;
  case TAU_AST_EXPR_LIT_STR:       tau_ast_expr_lit_str_dump_json      (stream, (tau_ast_expr_lit_str_t*      )node); break;
  case TAU_AST_EXPR_LIT_CHAR:      tau_ast_expr_lit_char_dump_json     (stream, (tau_ast_expr_lit_char_t*     )node); break;
  case TAU_AST_EXPR_LIT_BOOL:      tau_ast_expr_lit_bool_dump_json     (stream, (tau_ast_expr_lit_bool_t*     )node); break;
  case TAU_AST_EXPR_LIT_NULL:      tau_ast_expr_lit_null_dump_json     (stream, (tau_ast_expr_lit_null_t*     )node); break;
  case TAU_AST_EXPR_LIT_VEC:       tau_ast_expr_lit_vec_dump_json      (stream, (tau_ast_expr_lit_vec_t*      )node); break;
  case TAU_AST_EXPR_LIT_MAT:       tau_ast_expr_lit_mat_dump_json      (stream, (tau_ast_expr_lit_mat_t*      )node); break;
  case TAU_AST_EXPR_OP_UNARY:      tau_ast_expr_op_un_dump_json        (stream, (tau_ast_expr_op_un_t*        )node); break;
  case TAU_AST_EXPR_OP_BINARY:     tau_ast_expr_op_bin_dump_json       (stream, (tau_ast_expr_op_bin_t*       )node); break;
  case TAU_AST_EXPR_OP_CALL:       tau_ast_expr_op_call_dump_json      (stream, (tau_ast_expr_op_call_t*      )node); break;
  case TAU_AST_EXPR_OP_SPEC:       tau_ast_expr_op_spec_dump_json      (stream, (tau_ast_expr_op_spec_t*      )node); break;
  case TAU_AST_STMT_IF:            tau_ast_stmt_if_dump_json           (stream, (tau_ast_stmt_if_t*           )node); break;
  case TAU_AST_STMT_FOR:           tau_ast_stmt_for_dump_json          (stream, (tau_ast_stmt_for_t*          )node); break;
  case TAU_AST_STMT_WHILE:         tau_ast_stmt_while_dump_json        (stream, (tau_ast_stmt_while_t*        )node); break;
  case TAU_AST_STMT_DO_WHILE:      tau_ast_stmt_do_while_dump_json     (stream, (tau_ast_stmt_do_while_t*     )node); break;
  case TAU_AST_STMT_LOOP:          tau_ast_stmt_loop_dump_json         (stream, (tau_ast_stmt_loop_t*         )node); break;
  case TAU_AST_STMT_BREAK:         tau_ast_stmt_break_dump_json        (stream, (tau_ast_stmt_break_t*        )node); break;
  case TAU_AST_STMT_CONTINUE:      tau_ast_stmt_continue_dump_json     (stream, (tau_ast_stmt_continue_t*     )node); break;
  case TAU_AST_STMT_RETURN:        tau_ast_stmt_return_dump_json       (stream, (tau_ast_stmt_return_t*       )node); break;
  case TAU_AST_STMT_DEFER:         tau_ast_stmt_defer_dump_json        (stream, (tau_ast_stmt_defer_t*        )node); break;
  case TAU_AST_STMT_BLOCK:         tau_ast_stmt_block_dump_json        (stream, (tau_ast_stmt_block_t*        )node); break;
  case TAU_AST_STMT_EXPR:          tau_ast_stmt_expr_dump_json         (stream, (tau_ast_stmt_expr_t*         )node); break;
  case TAU_AST_DECL_VAR:           tau_ast_decl_var_dump_json          (stream, (tau_ast_decl_var_t*          )node); break;
  case TAU_AST_DECL_PARAM:         tau_ast_decl_param_dump_json        (stream, (tau_ast_decl_param_t*        )node); break;
  case TAU_AST_DECL_FUN:           tau_ast_decl_fun_dump_json          (stream, (tau_ast_decl_fun_t*          )node); break;
  case TAU_AST_DECL_STRUCT:        tau_ast_decl_struct_dump_json       (stream, (tau_ast_decl_struct_t*       )node); break;
  case TAU_AST_DECL_UNION:         tau_ast_decl_union_dump_json        (stream, (tau_ast_decl_union_t*        )node); break;
  case TAU_AST_DECL_ENUM:          tau_ast_decl_enum_dump_json         (stream, (tau_ast_decl_enum_t*         )node); break;
  case TAU_AST_DECL_ENUM_CONSTANT: tau_ast_decl_enum_constant_dump_json(stream, (tau_ast_decl_enum_constant_t*)node); break;
  case TAU_AST_DECL_MOD:           tau_ast_decl_mod_dump_json          (stream, (tau_ast_decl_mod_t*          )node); break;
  case TAU_AST_DECL_TYPE_ALIAS:    tau_ast_decl_type_alias_dump_json   (stream, (tau_ast_decl_type_alias_t*   )node); break;
  case TAU_AST_DECL_GENERIC_FUN:   tau_ast_decl_generic_fun_dump_json  (stream, (tau_ast_decl_generic_fun_t*  )node); break;
  case TAU_AST_DECL_GENERIC_PARAM: tau_ast_decl_generic_param_dump_json(stream, (tau_ast_decl_generic_param_t*)node); break;
  case TAU_AST_PATH_SEGMENT:       tau_ast_path_segment_dump_json      (stream, (tau_ast_path_segment_t*      )node); break;
  case TAU_AST_PATH_ACCESS:        tau_ast_path_access_dump_json       (stream, (tau_ast_path_access_t*       )node); break;
  case TAU_AST_PATH_LIST:          tau_ast_path_list_dump_json         (stream, (tau_ast_path_list_t*         )node); break;
  case TAU_AST_PATH_WILDCARD:      tau_ast_path_wildcard_dump_json     (stream, (tau_ast_path_wildcard_t*     )node); break;
  case TAU_AST_PATH_ALIAS:         tau_ast_path_alias_dump_json        (stream, (tau_ast_path_alias_t*        )node); break;
  case TAU_AST_USE:                tau_ast_use_dump_json               (stream, (tau_ast_use_t*               )node); break;
  case TAU_AST_PROG:               tau_ast_prog_dump_json              (stream, (tau_ast_prog_t*              )node); break;
  default: TAU_UNREACHABLE();
  }
}

const char* tau_ast_kind_to_cstr(tau_ast_kind_t kind)
{
  switch (kind)
  {
  case TAU_AST_ID:                 return "TAU_AST_ID";
  case TAU_AST_POISON:             return "TAU_AST_POISON";
  case TAU_AST_TYPE_ID:            return "TAU_AST_TYPE_ID";
  case TAU_AST_TYPE_MUT:           return "TAU_AST_TYPE_MUT";
  case TAU_AST_TYPE_CONST:         return "TAU_AST_TYPE_CONST";
  case TAU_AST_TYPE_PTR:           return "TAU_AST_TYPE_PTR";
  case TAU_AST_TYPE_ARRAY:         return "TAU_AST_TYPE_ARRAY";
  case TAU_AST_TYPE_REF:           return "TAU_AST_TYPE_REF";
  case TAU_AST_TYPE_OPT:           return "TAU_AST_TYPE_OPT";
  case TAU_AST_TYPE_FUN:           return "TAU_AST_TYPE_FUN";
  case TAU_AST_TYPE_VEC:           return "TAU_AST_TYPE_VEC";
  case TAU_AST_TYPE_MAT:           return "TAU_AST_TYPE_MAT";
  case TAU_AST_TYPE_PRIM_I8:       return "TAU_AST_TYPE_PRIM_I8";
  case TAU_AST_TYPE_PRIM_I16:      return "TAU_AST_TYPE_PRIM_I16";
  case TAU_AST_TYPE_PRIM_I32:      return "TAU_AST_TYPE_PRIM_I32";
  case TAU_AST_TYPE_PRIM_I64:      return "TAU_AST_TYPE_PRIM_I64";
  case TAU_AST_TYPE_PRIM_ISIZE:    return "TAU_AST_TYPE_PRIM_ISIZE";
  case TAU_AST_TYPE_PRIM_U8:       return "TAU_AST_TYPE_PRIM_U8";
  case TAU_AST_TYPE_PRIM_U16:      return "TAU_AST_TYPE_PRIM_U16";
  case TAU_AST_TYPE_PRIM_U32:      return "TAU_AST_TYPE_PRIM_U32";
  case TAU_AST_TYPE_PRIM_U64:      return "TAU_AST_TYPE_PRIM_U64";
  case TAU_AST_TYPE_PRIM_USIZE:    return "TAU_AST_TYPE_PRIM_USIZE";
  case TAU_AST_TYPE_PRIM_F32:      return "TAU_AST_TYPE_PRIM_F32";
  case TAU_AST_TYPE_PRIM_F64:      return "TAU_AST_TYPE_PRIM_F64";
  case TAU_AST_TYPE_PRIM_C64:      return "TAU_AST_TYPE_PRIM_C64";
  case TAU_AST_TYPE_PRIM_C128:     return "TAU_AST_TYPE_PRIM_C128";
  case TAU_AST_TYPE_PRIM_CHAR:     return "TAU_AST_TYPE_PRIM_CHAR";
  case TAU_AST_TYPE_PRIM_BOOL:     return "TAU_AST_TYPE_PRIM_BOOL";
  case TAU_AST_TYPE_PRIM_UNIT:     return "TAU_AST_TYPE_PRIM_UNIT";
  case TAU_AST_TYPE_MEMBER:        return "TAU_AST_TYPE_MEMBER";
  case TAU_AST_TYPE_TYPE:          return "TAU_AST_TYPE_TYPE";
  case TAU_AST_EXPR_ID:            return "TAU_AST_EXPR_ID";
  case TAU_AST_EXPR_LIT_INT:       return "TAU_AST_EXPR_LIT_INT";
  case TAU_AST_EXPR_LIT_FLT:       return "TAU_AST_EXPR_LIT_FLT";
  case TAU_AST_EXPR_LIT_STR:       return "TAU_AST_EXPR_LIT_STR";
  case TAU_AST_EXPR_LIT_CHAR:      return "TAU_AST_EXPR_LIT_CHAR";
  case TAU_AST_EXPR_LIT_BOOL:      return "TAU_AST_EXPR_LIT_BOOL";
  case TAU_AST_EXPR_LIT_NULL:      return "TAU_AST_EXPR_LIT_NULL";
  case TAU_AST_EXPR_LIT_VEC:       return "TAU_AST_EXPR_LIT_VEC";
  case TAU_AST_EXPR_LIT_MAT:       return "TAU_AST_EXPR_LIT_MAT";
  case TAU_AST_EXPR_OP_UNARY:      return "TAU_AST_EXPR_OP_UNARY";
  case TAU_AST_EXPR_OP_BINARY:     return "TAU_AST_EXPR_OP_BINARY";
  case TAU_AST_EXPR_OP_CALL:       return "TAU_AST_EXPR_OP_CALL";
  case TAU_AST_EXPR_OP_SPEC:       return "TAU_AST_EXPR_OP_SPEC";
  case TAU_AST_STMT_IF:            return "TAU_AST_STMT_IF";
  case TAU_AST_STMT_FOR:           return "TAU_AST_STMT_FOR";
  case TAU_AST_STMT_WHILE:         return "TAU_AST_STMT_WHILE";
  case TAU_AST_STMT_DO_WHILE:      return "TAU_AST_STMT_DO_WHILE";
  case TAU_AST_STMT_LOOP:          return "TAU_AST_STMT_LOOP";
  case TAU_AST_STMT_BREAK:         return "TAU_AST_STMT_BREAK";
  case TAU_AST_STMT_CONTINUE:      return "TAU_AST_STMT_CONTINUE";
  case TAU_AST_STMT_RETURN:        return "TAU_AST_STMT_RETURN";
  case TAU_AST_STMT_DEFER:         return "TAU_AST_STMT_DEFER";
  case TAU_AST_STMT_BLOCK:         return "TAU_AST_STMT_BLOCK";
  case TAU_AST_STMT_EXPR:          return "TAU_AST_STMT_EXPR";
  case TAU_AST_DECL_VAR:           return "TAU_AST_DECL_VAR";
  case TAU_AST_DECL_PARAM:         return "TAU_AST_DECL_PARAM";
  case TAU_AST_DECL_FUN:           return "TAU_AST_DECL_FUN";
  case TAU_AST_DECL_STRUCT:        return "TAU_AST_DECL_STRUCT";
  case TAU_AST_DECL_UNION:         return "TAU_AST_DECL_UNION";
  case TAU_AST_DECL_ENUM:          return "TAU_AST_DECL_ENUM";
  case TAU_AST_DECL_ENUM_CONSTANT: return "TAU_AST_DECL_ENUM_CONSTANT";
  case TAU_AST_DECL_MOD:           return "TAU_AST_DECL_MOD";
  case TAU_AST_DECL_TYPE_ALIAS:    return "TAU_AST_DECL_TYPE_ALIAS";
  case TAU_AST_DECL_GENERIC_FUN:   return "TAU_AST_DECL_GENERIC_FUN";
  case TAU_AST_DECL_GENERIC_PARAM: return "TAU_AST_DECL_GENERIC_PARAM";
  case TAU_AST_PATH_SEGMENT:       return "TAU_AST_PATH_SEGMENT";
  case TAU_AST_PATH_ACCESS:        return "TAU_AST_PATH_ACCESS";
  case TAU_AST_PATH_LIST:          return "TAU_AST_PATH_LIST";
  case TAU_AST_PATH_WILDCARD:      return "TAU_AST_PATH_WILDCARD";
  case TAU_AST_PATH_ALIAS:         return "TAU_AST_PATH_ALIAS";
  case TAU_AST_USE:                return "TAU_AST_USE";
  case TAU_AST_PROG:               return "TAU_AST_PROG";
  default: TAU_UNREACHABLE();
  }

  return NULL;
}

bool tau_ast_is_type(tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_TYPE_ID:
  case TAU_AST_TYPE_MUT:
  case TAU_AST_TYPE_CONST:
  case TAU_AST_TYPE_PTR:
  case TAU_AST_TYPE_ARRAY:
  case TAU_AST_TYPE_REF:
  case TAU_AST_TYPE_OPT:
  case TAU_AST_TYPE_FUN:
  case TAU_AST_TYPE_VEC:
  case TAU_AST_TYPE_MAT:
  case TAU_AST_TYPE_PRIM_I8:
  case TAU_AST_TYPE_PRIM_I16:
  case TAU_AST_TYPE_PRIM_I32:
  case TAU_AST_TYPE_PRIM_I64:
  case TAU_AST_TYPE_PRIM_ISIZE:
  case TAU_AST_TYPE_PRIM_U8:
  case TAU_AST_TYPE_PRIM_U16:
  case TAU_AST_TYPE_PRIM_U32:
  case TAU_AST_TYPE_PRIM_U64:
  case TAU_AST_TYPE_PRIM_USIZE:
  case TAU_AST_TYPE_PRIM_F32:
  case TAU_AST_TYPE_PRIM_F64:
  case TAU_AST_TYPE_PRIM_C64:
  case TAU_AST_TYPE_PRIM_C128:
  case TAU_AST_TYPE_PRIM_CHAR:
  case TAU_AST_TYPE_PRIM_BOOL:
  case TAU_AST_TYPE_PRIM_UNIT:
  case TAU_AST_TYPE_MEMBER:
  case TAU_AST_TYPE_TYPE:
    return true;
  default:
    return false;
  }
}

bool tau_ast_is_expr(tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_EXPR_ID:
  case TAU_AST_EXPR_LIT_INT:
  case TAU_AST_EXPR_LIT_FLT:
  case TAU_AST_EXPR_LIT_STR:
  case TAU_AST_EXPR_LIT_CHAR:
  case TAU_AST_EXPR_LIT_BOOL:
  case TAU_AST_EXPR_LIT_NULL:
  case TAU_AST_EXPR_LIT_VEC:
  case TAU_AST_EXPR_LIT_MAT:
  case TAU_AST_EXPR_OP_UNARY:
  case TAU_AST_EXPR_OP_BINARY:
  case TAU_AST_EXPR_OP_CALL:
  case TAU_AST_EXPR_OP_SPEC:
    return true;
  default:
    return false;
  }
}

bool tau_ast_is_stmt(tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_STMT_IF:
  case TAU_AST_STMT_FOR:
  case TAU_AST_STMT_WHILE:
  case TAU_AST_STMT_DO_WHILE:
  case TAU_AST_STMT_LOOP:
  case TAU_AST_STMT_BREAK:
  case TAU_AST_STMT_CONTINUE:
  case TAU_AST_STMT_RETURN:
  case TAU_AST_STMT_DEFER:
  case TAU_AST_STMT_BLOCK:
  case TAU_AST_STMT_EXPR:
    return true;
  default:
    return false;
  }
}

bool tau_ast_is_decl(tau_ast_node_t* node)
{
  TAU_ASSERT(node != NULL);

  switch (node->kind)
  {
  case TAU_AST_DECL_VAR:
  case TAU_AST_DECL_PARAM:
  case TAU_AST_DECL_FUN:
  case TAU_AST_DECL_STRUCT:
  case TAU_AST_DECL_UNION:
  case TAU_AST_DECL_ENUM:
  case TAU_AST_DECL_ENUM_CONSTANT:
  case TAU_AST_DECL_MOD:
  case TAU_AST_DECL_TYPE_ALIAS:
  case TAU_AST_DECL_GENERIC_FUN:
  case TAU_AST_DECL_GENERIC_PARAM:
    return true;
  default:
    return false;
  }
}
