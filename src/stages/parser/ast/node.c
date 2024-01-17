/**
 * \file node.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "stages/parser/ast/node.h"

#include "utils/common.h"

const char* ast_kind_to_cstr(ast_kind_t kind)
{
  switch (kind)
  {
  case AST_ID:                 return "AST_ID";
  case AST_TYPE_MUT:           return "AST_TYPE_MUT";
  case AST_TYPE_CONST:         return "AST_TYPE_CONST";
  case AST_TYPE_PTR:           return "AST_TYPE_PTR";
  case AST_TYPE_ARRAY:         return "AST_TYPE_ARRAY";
  case AST_TYPE_REF:           return "AST_TYPE_REF";
  case AST_TYPE_OPT:           return "AST_TYPE_OPT";
  case AST_TYPE_FUN:           return "AST_TYPE_FUN";
  case AST_TYPE_I8:            return "AST_TYPE_I8";
  case AST_TYPE_I16:           return "AST_TYPE_I16";
  case AST_TYPE_I32:           return "AST_TYPE_I32";
  case AST_TYPE_I64:           return "AST_TYPE_I64";
  case AST_TYPE_ISIZE:         return "AST_TYPE_ISIZE";
  case AST_TYPE_U8:            return "AST_TYPE_U8";
  case AST_TYPE_U16:           return "AST_TYPE_U16";
  case AST_TYPE_U32:           return "AST_TYPE_U32";
  case AST_TYPE_U64:           return "AST_TYPE_U64";
  case AST_TYPE_USIZE:         return "AST_TYPE_USIZE";
  case AST_TYPE_F32:           return "AST_TYPE_F32";
  case AST_TYPE_F64:           return "AST_TYPE_F64";
  case AST_TYPE_BOOL:          return "AST_TYPE_BOOL";
  case AST_TYPE_UNIT:          return "AST_TYPE_UNIT";
  case AST_TYPE_MEMBER:        return "AST_TYPE_MEMBER";
  case AST_TYPE_DECL:          return "AST_TYPE_DECL";
  case AST_EXPR_LIT_INT:       return "AST_EXPR_LIT_INT";
  case AST_EXPR_LIT_FLT:       return "AST_EXPR_LIT_FLT";
  case AST_EXPR_LIT_STR:       return "AST_EXPR_LIT_STR";
  case AST_EXPR_LIT_CHAR:      return "AST_EXPR_LIT_CHAR";
  case AST_EXPR_LIT_BOOL:      return "AST_EXPR_LIT_BOOL";
  case AST_EXPR_LIT_NULL:      return "AST_EXPR_LIT_NULL";
  case AST_EXPR_OP_UNARY:      return "AST_EXPR_OP_UNARY";
  case AST_EXPR_OP_BINARY:     return "AST_EXPR_OP_BINARY";
  case AST_EXPR_OP_CALL:       return "AST_EXPR_OP_CALL";
  case AST_EXPR_DECL:          return "AST_EXPR_DECL";
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
  case AST_TYPE_MUT:
  case AST_TYPE_CONST:
  case AST_TYPE_PTR:
  case AST_TYPE_ARRAY:
  case AST_TYPE_REF:
  case AST_TYPE_OPT:
  case AST_TYPE_FUN:
  case AST_TYPE_I8:
  case AST_TYPE_I16:
  case AST_TYPE_I32:
  case AST_TYPE_I64:
  case AST_TYPE_ISIZE:
  case AST_TYPE_U8:
  case AST_TYPE_U16:
  case AST_TYPE_U32:
  case AST_TYPE_U64:
  case AST_TYPE_USIZE:
  case AST_TYPE_F32:
  case AST_TYPE_F64:
  case AST_TYPE_BOOL:
  case AST_TYPE_UNIT:
  case AST_TYPE_MEMBER:
  case AST_TYPE_DECL:
    return true;
  default:
    return false;
  }
}

bool ast_is_expr(ast_node_t* node)
{
  switch (node->kind)
  {
  case AST_EXPR_LIT_INT:
  case AST_EXPR_LIT_FLT:
  case AST_EXPR_LIT_STR:
  case AST_EXPR_LIT_CHAR:
  case AST_EXPR_LIT_BOOL:
  case AST_EXPR_LIT_NULL:
  case AST_EXPR_OP_UNARY:
  case AST_EXPR_OP_BINARY:
  case AST_EXPR_OP_CALL:
  case AST_EXPR_DECL:
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
