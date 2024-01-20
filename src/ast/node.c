/**
 * \file node.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/node.h"

#include "ast/ast.h"
#include "utils/common.h"

void ast_node_free(ast_node_t* node)
{
  if (node == NULL)
    return;

  switch (node->kind)
  {
  case AST_ID:                 ast_id_free                ((ast_id_t*                )node); break;
  case AST_TYPE_ID:            ast_type_id_free           ((ast_type_id_t*           )node); break;
  case AST_TYPE_MUT:           ast_type_mut_free          ((ast_type_mut_t*          )node); break;
  case AST_TYPE_CONST:         ast_type_const_free        ((ast_type_const_t*        )node); break;
  case AST_TYPE_PTR:           ast_type_ptr_free          ((ast_type_ptr_t*          )node); break;
  case AST_TYPE_ARRAY:         ast_type_array_free        ((ast_type_array_t*        )node); break;
  case AST_TYPE_REF:           ast_type_ref_free          ((ast_type_ref_t*          )node); break;
  case AST_TYPE_OPT:           ast_type_opt_free          ((ast_type_opt_t*          )node); break;
  case AST_TYPE_FUN:           ast_type_fun_free          ((ast_type_fun_t*          )node); break;
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
  case AST_TYPE_PRIM_UNIT:     ast_type_prim_free         ((ast_type_prim_t*         )node); break;
  case AST_TYPE_MEMBER:        ast_type_mbr_free          ((ast_type_mbr_t*          )node); break;
  case AST_EXPR_ID:            ast_expr_id_free           ((ast_expr_id_t*           )node); break;
  case AST_EXPR_LIT_INT:       ast_expr_lit_int_free      ((ast_expr_lit_int_t*      )node); break;
  case AST_EXPR_LIT_FLT:       ast_expr_lit_flt_free      ((ast_expr_lit_flt_t*      )node); break;
  case AST_EXPR_LIT_STR:       ast_expr_lit_str_free      ((ast_expr_lit_str_t*      )node); break;
  case AST_EXPR_LIT_CHAR:      ast_expr_lit_char_free     ((ast_expr_lit_char_t*     )node); break;
  case AST_EXPR_LIT_BOOL:      ast_expr_lit_bool_free     ((ast_expr_lit_bool_t*     )node); break;
  case AST_EXPR_LIT_NULL:      ast_expr_lit_null_free     ((ast_expr_lit_null_t*     )node); break;
  case AST_EXPR_OP_UNARY:      ast_expr_op_un_free        ((ast_expr_op_un_t*        )node); break;
  case AST_EXPR_OP_BINARY:     ast_expr_op_bin_free       ((ast_expr_op_bin_t*       )node); break;
  case AST_EXPR_OP_CALL:       ast_expr_op_call_free      ((ast_expr_op_call_t*      )node); break;
  case AST_STMT_IF:            ast_stmt_if_free           ((ast_stmt_if_t*           )node); break;
  case AST_STMT_FOR:           ast_stmt_for_free          ((ast_stmt_for_t*          )node); break;
  case AST_STMT_WHILE:         ast_stmt_while_free        ((ast_stmt_while_t*        )node); break;
  case AST_STMT_BREAK:         ast_stmt_break_free        ((ast_stmt_break_t*        )node); break;
  case AST_STMT_CONTINUE:      ast_stmt_continue_free     ((ast_stmt_continue_t*     )node); break;
  case AST_STMT_RETURN:        ast_stmt_return_free       ((ast_stmt_return_t*       )node); break;
  case AST_STMT_DEFER:         ast_stmt_defer_free        ((ast_stmt_defer_t*        )node); break;
  case AST_STMT_BLOCK:         ast_stmt_block_free        ((ast_stmt_block_t*        )node); break;
  case AST_STMT_EXPR:          ast_stmt_expr_free         ((ast_stmt_expr_t*         )node); break;
  case AST_DECL_VAR:           ast_decl_var_free          ((ast_decl_var_t*          )node); break;
  case AST_DECL_FUN:           ast_decl_fun_free          ((ast_decl_fun_t*          )node); break;
  case AST_DECL_STRUCT:        ast_decl_struct_free       ((ast_decl_struct_t*       )node); break;
  case AST_DECL_UNION:         ast_decl_union_free        ((ast_decl_union_t*        )node); break;
  case AST_DECL_ENUM:          ast_decl_enum_free         ((ast_decl_enum_t*         )node); break;
  case AST_DECL_MOD:           ast_decl_mod_free          ((ast_decl_mod_t*          )node); break;
  case AST_DECL_PARAM:         ast_decl_param_free        ((ast_decl_param_t*        )node); break;
  case AST_DECL_ENUM_CONSTANT: ast_decl_enum_constant_free((ast_decl_enum_constant_t*)node); break;
  case AST_PROG:               ast_prog_free              ((ast_prog_t*              )node); break;
  default: unreachable();
  }
}

void ast_node_nameres(nameres_ctx_t* ctx, ast_node_t* node)
{
  if (node == NULL)
    return;

  switch (node->kind)
  {
  case AST_TYPE_ID:            ast_type_id_nameres           (ctx, (ast_type_id_t*           )node); break;
  case AST_TYPE_MUT:           ast_type_mut_nameres          (ctx, (ast_type_mut_t*          )node); break;
  case AST_TYPE_CONST:         ast_type_const_nameres        (ctx, (ast_type_const_t*        )node); break;
  case AST_TYPE_PTR:           ast_type_ptr_nameres          (ctx, (ast_type_ptr_t*          )node); break;
  case AST_TYPE_ARRAY:         ast_type_array_nameres        (ctx, (ast_type_array_t*        )node); break;
  case AST_TYPE_REF:           ast_type_ref_nameres          (ctx, (ast_type_ref_t*          )node); break;
  case AST_TYPE_OPT:           ast_type_opt_nameres          (ctx, (ast_type_opt_t*          )node); break;
  case AST_TYPE_FUN:           ast_type_fun_nameres          (ctx, (ast_type_fun_t*          )node); break;
  case AST_TYPE_MEMBER:        ast_type_mbr_nameres          (ctx, (ast_type_mbr_t*          )node); break;
  case AST_EXPR_ID:            ast_expr_id_nameres           (ctx, (ast_expr_id_t*           )node); break;
  case AST_EXPR_OP_UNARY:      ast_expr_op_un_nameres        (ctx, (ast_expr_op_un_t*        )node); break;
  case AST_EXPR_OP_BINARY:     ast_expr_op_bin_nameres       (ctx, (ast_expr_op_bin_t*       )node); break;
  case AST_EXPR_OP_CALL:       ast_expr_op_call_nameres      (ctx, (ast_expr_op_call_t*      )node); break;
  case AST_STMT_IF:            ast_stmt_if_nameres           (ctx, (ast_stmt_if_t*           )node); break;
  case AST_STMT_FOR:           ast_stmt_for_nameres          (ctx, (ast_stmt_for_t*          )node); break;
  case AST_STMT_WHILE:         ast_stmt_while_nameres        (ctx, (ast_stmt_while_t*        )node); break;
  case AST_STMT_RETURN:        ast_stmt_return_nameres       (ctx, (ast_stmt_return_t*       )node); break;
  case AST_STMT_DEFER:         ast_stmt_defer_nameres        (ctx, (ast_stmt_defer_t*        )node); break;
  case AST_STMT_BLOCK:         ast_stmt_block_nameres        (ctx, (ast_stmt_block_t*        )node); break;
  case AST_STMT_EXPR:          ast_stmt_expr_nameres         (ctx, (ast_stmt_expr_t*         )node); break;
  case AST_DECL_VAR:           ast_decl_var_nameres          (ctx, (ast_decl_var_t*          )node); break;
  case AST_DECL_PARAM:         ast_decl_param_nameres        (ctx, (ast_decl_param_t*        )node); break;
  case AST_DECL_FUN:           ast_decl_fun_nameres          (ctx, (ast_decl_fun_t*          )node); break;
  case AST_DECL_STRUCT:        ast_decl_struct_nameres       (ctx, (ast_decl_struct_t*       )node); break;
  case AST_DECL_UNION:         ast_decl_union_nameres        (ctx, (ast_decl_union_t*        )node); break;
  case AST_DECL_ENUM:          ast_decl_enum_nameres         (ctx, (ast_decl_enum_t*         )node); break;
  case AST_DECL_ENUM_CONSTANT: ast_decl_enum_constant_nameres(ctx, (ast_decl_enum_constant_t*)node); break;
  case AST_DECL_MOD:           ast_decl_mod_nameres          (ctx, (ast_decl_mod_t*          )node); break;
  case AST_PROG:               ast_prog_nameres              (ctx, (ast_prog_t*              )node); break;
  case AST_ID:
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
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
  case AST_STMT_BREAK:
  case AST_STMT_CONTINUE: break;
  default: unreachable();
  }
}

void ast_node_dump_json_vector(FILE* stream, vector_t* vec)
{
  if (vec == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  fputc('[', stream);
  
  VECTOR_FOR_LOOP(i, vec)
  {
    ast_node_dump_json(stream, (ast_node_t*)vector_get(vec, i));

    if (i + 1 < vector_size(vec))
      fputc(',', stream);
  }

  fputc(']', stream);
}

void ast_node_dump_json(FILE* stream, ast_node_t* node)
{
  if (node == NULL)
  {
    fprintf(stream, "null");
    return;
  }

  switch (node->kind)
  {
  case AST_ID:                 ast_id_dump_json                (stream, (ast_id_t*                )node); break;
  case AST_TYPE_ID:            ast_type_id_dump_json           (stream, (ast_type_id_t*           )node); break;
  case AST_TYPE_MUT:           ast_type_mut_dump_json          (stream, (ast_type_mut_t*          )node); break;
  case AST_TYPE_CONST:         ast_type_const_dump_json        (stream, (ast_type_const_t*        )node); break;
  case AST_TYPE_PTR:           ast_type_ptr_dump_json          (stream, (ast_type_ptr_t*          )node); break;
  case AST_TYPE_ARRAY:         ast_type_array_dump_json        (stream, (ast_type_array_t*        )node); break;
  case AST_TYPE_REF:           ast_type_ref_dump_json          (stream, (ast_type_ref_t*          )node); break;
  case AST_TYPE_OPT:           ast_type_opt_dump_json          (stream, (ast_type_opt_t*          )node); break;
  case AST_TYPE_FUN:           ast_type_fun_dump_json          (stream, (ast_type_fun_t*          )node); break;
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
  case AST_TYPE_PRIM_UNIT:     ast_type_prim_dump_json         (stream, (ast_type_prim_t*         )node); break;
  case AST_TYPE_MEMBER:        ast_type_mbr_dump_json          (stream, (ast_type_mbr_t*          )node); break;
  case AST_EXPR_ID:            ast_expr_id_dump_json           (stream, (ast_expr_id_t*           )node); break;
  case AST_EXPR_LIT_INT:       ast_expr_lit_int_dump_json      (stream, (ast_expr_lit_int_t*      )node); break;
  case AST_EXPR_LIT_FLT:       ast_expr_lit_flt_dump_json      (stream, (ast_expr_lit_flt_t*      )node); break;
  case AST_EXPR_LIT_STR:       ast_expr_lit_str_dump_json      (stream, (ast_expr_lit_str_t*      )node); break;
  case AST_EXPR_LIT_CHAR:      ast_expr_lit_char_dump_json     (stream, (ast_expr_lit_char_t*     )node); break;
  case AST_EXPR_LIT_BOOL:      ast_expr_lit_bool_dump_json     (stream, (ast_expr_lit_bool_t*     )node); break;
  case AST_EXPR_LIT_NULL:      ast_expr_lit_null_dump_json     (stream, (ast_expr_lit_null_t*     )node); break;
  case AST_EXPR_OP_UNARY:      ast_expr_op_un_dump_json        (stream, (ast_expr_op_un_t*        )node); break;
  case AST_EXPR_OP_BINARY:     ast_expr_op_bin_dump_json       (stream, (ast_expr_op_bin_t*       )node); break;
  case AST_EXPR_OP_CALL:       ast_expr_op_call_dump_json      (stream, (ast_expr_op_call_t*      )node); break;
  case AST_STMT_IF:            ast_stmt_if_dump_json           (stream, (ast_stmt_if_t*           )node); break;
  case AST_STMT_FOR:           ast_stmt_for_dump_json          (stream, (ast_stmt_for_t*          )node); break;
  case AST_STMT_WHILE:         ast_stmt_while_dump_json        (stream, (ast_stmt_while_t*        )node); break;
  case AST_STMT_BREAK:         ast_stmt_break_dump_json        (stream, (ast_stmt_break_t*        )node); break;
  case AST_STMT_CONTINUE:      ast_stmt_continue_dump_json     (stream, (ast_stmt_continue_t*     )node); break;
  case AST_STMT_RETURN:        ast_stmt_return_dump_json       (stream, (ast_stmt_return_t*       )node); break;
  case AST_STMT_DEFER:         ast_stmt_defer_dump_json        (stream, (ast_stmt_defer_t*        )node); break;
  case AST_STMT_BLOCK:         ast_stmt_block_dump_json        (stream, (ast_stmt_block_t*        )node); break;
  case AST_STMT_EXPR:          ast_stmt_expr_dump_json         (stream, (ast_stmt_expr_t*         )node); break;
  case AST_DECL_VAR:           ast_decl_var_dump_json          (stream, (ast_decl_var_t*          )node); break;
  case AST_DECL_FUN:           ast_decl_fun_dump_json          (stream, (ast_decl_fun_t*          )node); break;
  case AST_DECL_STRUCT:        ast_decl_struct_dump_json       (stream, (ast_decl_struct_t*       )node); break;
  case AST_DECL_UNION:         ast_decl_union_dump_json        (stream, (ast_decl_union_t*        )node); break;
  case AST_DECL_ENUM:          ast_decl_enum_dump_json         (stream, (ast_decl_enum_t*         )node); break;
  case AST_DECL_MOD:           ast_decl_mod_dump_json          (stream, (ast_decl_mod_t*          )node); break;
  case AST_DECL_PARAM:         ast_decl_param_dump_json        (stream, (ast_decl_param_t*        )node); break;
  case AST_DECL_ENUM_CONSTANT: ast_decl_enum_constant_dump_json(stream, (ast_decl_enum_constant_t*)node); break;
  case AST_PROG:               ast_prog_dump_json              (stream, (ast_prog_t*              )node); break;
  default: unreachable();
  }
}

const char* ast_kind_to_cstr(ast_kind_t kind)
{
  switch (kind)
  {
  case AST_ID:                 return "AST_ID";
  case AST_TYPE_ID:            return "AST_TYPE_ID";
  case AST_TYPE_MUT:           return "AST_TYPE_MUT";
  case AST_TYPE_CONST:         return "AST_TYPE_CONST";
  case AST_TYPE_PTR:           return "AST_TYPE_PTR";
  case AST_TYPE_ARRAY:         return "AST_TYPE_ARRAY";
  case AST_TYPE_REF:           return "AST_TYPE_REF";
  case AST_TYPE_OPT:           return "AST_TYPE_OPT";
  case AST_TYPE_FUN:           return "AST_TYPE_FUN";
  case AST_TYPE_PRIM_I8:       return "AST_TYPE_PRIM_I8";
  case AST_TYPE_PRIM_I16:      return "AST_TYPE_PRIM_I16";
  case AST_TYPE_PRIM_I32:      return "AST_TYPE_PRIM_I32";
  case AST_TYPE_PRIM_I64:      return "AST_TYPE_PRIM_I64";
  case AST_TYPE_PRIM_ISIZE:    return "AST_TYPE_PRIM_ISIZE";
  case AST_TYPE_PRIM_U8:       return "AST_TYPE_PRIM_U8";
  case AST_TYPE_PRIM_U16:      return "AST_TYPE_PRIM_U16";
  case AST_TYPE_PRIM_U32:      return "AST_TYPE_PRIM_U32";
  case AST_TYPE_PRIM_U64:      return "AST_TYPE_PRIM_U64";
  case AST_TYPE_PRIM_USIZE:    return "AST_TYPE_PRIM_USIZE";
  case AST_TYPE_PRIM_F32:      return "AST_TYPE_PRIM_F32";
  case AST_TYPE_PRIM_F64:      return "AST_TYPE_PRIM_F64";
  case AST_TYPE_PRIM_CHAR:     return "AST_TYPE_PRIM_CHAR";
  case AST_TYPE_PRIM_BOOL:     return "AST_TYPE_PRIM_BOOL";
  case AST_TYPE_PRIM_UNIT:     return "AST_TYPE_PRIM_UNIT";
  case AST_TYPE_MEMBER:        return "AST_TYPE_MEMBER";
  case AST_EXPR_ID:            return "AST_EXPR_ID";
  case AST_EXPR_LIT_INT:       return "AST_EXPR_LIT_INT";
  case AST_EXPR_LIT_FLT:       return "AST_EXPR_LIT_FLT";
  case AST_EXPR_LIT_STR:       return "AST_EXPR_LIT_STR";
  case AST_EXPR_LIT_CHAR:      return "AST_EXPR_LIT_CHAR";
  case AST_EXPR_LIT_BOOL:      return "AST_EXPR_LIT_BOOL";
  case AST_EXPR_LIT_NULL:      return "AST_EXPR_LIT_NULL";
  case AST_EXPR_OP_UNARY:      return "AST_EXPR_OP_UNARY";
  case AST_EXPR_OP_BINARY:     return "AST_EXPR_OP_BINARY";
  case AST_EXPR_OP_CALL:       return "AST_EXPR_OP_CALL";
  case AST_STMT_IF:            return "AST_STMT_IF";
  case AST_STMT_FOR:           return "AST_STMT_FOR";
  case AST_STMT_WHILE:         return "AST_STMT_WHILE";
  case AST_STMT_BREAK:         return "AST_STMT_BREAK";
  case AST_STMT_CONTINUE:      return "AST_STMT_CONTINUE";
  case AST_STMT_RETURN:        return "AST_STMT_RETURN";
  case AST_STMT_DEFER:         return "AST_STMT_DEFER";
  case AST_STMT_BLOCK:         return "AST_STMT_BLOCK";
  case AST_STMT_EXPR:          return "AST_STMT_EXPR";
  case AST_DECL_VAR:           return "AST_DECL_VAR";
  case AST_DECL_FUN:           return "AST_DECL_FUN";
  case AST_DECL_STRUCT:        return "AST_DECL_STRUCT";
  case AST_DECL_UNION:         return "AST_DECL_UNION";
  case AST_DECL_ENUM:          return "AST_DECL_ENUM";
  case AST_DECL_MOD:           return "AST_DECL_MOD";
  case AST_DECL_PARAM:         return "AST_DECL_PARAM";
  case AST_DECL_ENUM_CONSTANT: return "AST_DECL_ENUM_CONSTANT";
  case AST_PROG:               return "AST_PROG";
  default: unreachable();
  }

  return NULL;
}

bool ast_is_type(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_TYPE_ID:
  case AST_TYPE_MUT:
  case AST_TYPE_CONST:
  case AST_TYPE_PTR:
  case AST_TYPE_ARRAY:
  case AST_TYPE_REF:
  case AST_TYPE_OPT:
  case AST_TYPE_FUN:
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
  case AST_TYPE_PRIM_CHAR:
  case AST_TYPE_PRIM_BOOL:
  case AST_TYPE_PRIM_UNIT:
  case AST_TYPE_MEMBER:
    return true;
  default:
    return false;
  }
}

bool ast_is_expr(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_EXPR_ID:
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
  case AST_EXPR_OP_UNARY:
  case AST_EXPR_OP_BINARY:
  case AST_EXPR_OP_CALL:
    return true;
  default:
    return false;
  }
}

bool ast_is_stmt(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_STMT_IF:
  case AST_STMT_FOR:
  case AST_STMT_WHILE:
  case AST_STMT_BREAK:
  case AST_STMT_CONTINUE:
  case AST_STMT_RETURN:
  case AST_STMT_DEFER:
  case AST_STMT_BLOCK:
  case AST_STMT_EXPR:
    return true;
  default:
    return false;
  }
}

bool ast_is_decl(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_DECL_VAR:
  case AST_DECL_PARAM:
  case AST_DECL_FUN:
  case AST_DECL_STRUCT:
  case AST_DECL_UNION:
  case AST_DECL_ENUM:
  case AST_DECL_ENUM_CONSTANT:
  case AST_DECL_MOD:
    return true;
  default:
    return false;
  }
}
