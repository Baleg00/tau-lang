#ifndef TAU_AST_H
#define TAU_AST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "token.h"
#include "op.h"

typedef enum ast_kind_e
{
  AST_UNKNOWN = 0, // unknown node

  AST_NODE_LIST, // list of ast nodes

  AST_ID, // identifier

  AST_TYPE_MUT, // mutable type
  AST_TYPE_CONST, // compile time type
  AST_TYPE_STATIC, // static storage type
  AST_TYPE_PTR, // pointer type
  AST_TYPE_ARRAY, // array type
  AST_TYPE_REF, // reference type
  AST_TYPE_NULLABLE, // nullable type
  AST_TYPE_FUN, // function type
  AST_TYPE_GEN, // generator type
  AST_TYPE_BUILTIN_I8, // built-in type i8
  AST_TYPE_BUILTIN_I16, // built-in type i16
  AST_TYPE_BUILTIN_I32, // built-in type i32
  AST_TYPE_BUILTIN_I64, // built-in type i64
  AST_TYPE_BUILTIN_ISIZE, // built-in type isize
  AST_TYPE_BUILTIN_U8, // built-in type u8
  AST_TYPE_BUILTIN_U16, // built-in type u16
  AST_TYPE_BUILTIN_U32, // built-in type u32
  AST_TYPE_BUILTIN_U64, // built-in type u64
  AST_TYPE_BUILTIN_USIZE, // built-in type usize
  AST_TYPE_BUILTIN_F32, // built-in type f32
  AST_TYPE_BUILTIN_F64, // built-in type f64
  AST_TYPE_BUILTIN_BOOL, // built-in type bool
  AST_TYPE_BUILTIN_UNIT, // built-in type unit

  AST_EXPR_LIT_INT, // expression literal integer
  AST_EXPR_LIT_FLT, // expression literal float
  AST_EXPR_LIT_STR, // expression literal string
  AST_EXPR_LIT_CHAR, // expression literal character
  AST_EXPR_LIT_BOOL, // expression literal boolean
  AST_EXPR_LIT_NULL, // expression literal null
  AST_EXPR_OP_UNARY, // expression operation unary
  AST_EXPR_OP_BINARY, // expression operation binary
  AST_EXPR_OP_CALL, // expression operation function call
  AST_EXPR_LAMBDA_FUN, // expression lambda function
  AST_EXPR_LAMBDA_GEN, // expression lambda generator
  AST_EXPR_IF, // expression if
  AST_EXPR_FOR, // expression for
  AST_EXPR_WHILE, // expression while
  AST_EXPR_DO_WHILE, // expression do while
  AST_EXPR_WHEN, // expression when
  AST_EXPR_BREAK, // expression break
  AST_EXPR_CONTINUE, // expression continue
  AST_EXPR_RETURN, // expression return
  AST_EXPR_YIELD, // expression yield
  AST_EXPR_BLOCK, // expression block

  AST_WHEN_CASE, // when case
  
  AST_DECL_VAR, // declaration variable
  AST_DECL_FUN, // declaration function
  AST_DECL_GEN, // declaration generator
  AST_DECL_STRUCT, // declaration struct
  AST_DECL_UNION, // declaration union
  AST_DECL_ENUM, // declaration enum
  AST_DECL_MOD, // declaration module
  AST_DECL_MEMBER, // declaration member

  AST_ARG, // function/generator argument
} ast_kind_t;

typedef struct ast_node_s
{
  ast_kind_t kind;
  
  token_t* tok;

  union {
    struct {
      struct ast_node_s *node, *next;
    } node_list;

    struct {
      struct ast_node_s* base_type;
    } type_mut,
      type_const,
      type_static,
      type_ptr,
      type_ref,
      type_nullable;

    struct {
      struct ast_node_s *size, *base_type;
    } type_array;

    struct {
      struct ast_node_s *args, *ret_type;
    } type_fun,
      type_gen;

    struct {
      op_kind_t kind;
      struct ast_node_s* child;
    } op_unary;

    struct {
      op_kind_t kind;
      struct ast_node_s *lhs, *rhs;
    } op_binary;
    
    struct {
      struct ast_node_s *callee, *args;
    } op_call;

    struct {
      struct ast_node_s *args, *ret_type, *expr;
    } lambda_fun,
      lambda_gen;

    struct {
      struct ast_node_s *cond, *expr, *expr_else;
    } expr_if;

    struct {
      struct ast_node_s *var, *range, *expr;
    } expr_for;

    struct {
      struct ast_node_s *cond, *expr;
    } expr_while;

    struct {
      struct ast_node_s *cond, *expr;
    } expr_do_while;

    struct {
      struct ast_node_s *expr, *cases;
    } expr_when;

    struct {
      struct ast_node_s* expr;
    } expr_break,
      expr_return,
      expr_yield;

    struct {
      struct ast_node_s *exprs;
    } expr_block;

    struct {
      struct ast_node_s *cond, *expr;
    } when_case;

    struct {
      struct ast_node_s *id, *type, *init;
    } decl_var;

    struct {
      struct ast_node_s *id, *args, *ret_type, *expr;
    } decl_fun,
      decl_gen;

    struct {
      struct ast_node_s *id, *members;
    } decl_struct,
      decl_union,
      decl_enum,
      decl_mod;

    struct {
      struct ast_node_s *decl;
      bool is_pub;
    } decl_member;

    struct {
      struct ast_node_s *id, *type;
    } arg;
  };
} ast_node_t;

ast_node_t* ast_node_init(ast_kind_t kind);
void ast_node_free(ast_node_t* node);

const char* ast_kind_to_string(ast_kind_t kind);

void ast_json_dump(FILE* stream, ast_node_t* root);

#endif