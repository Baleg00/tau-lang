#ifndef TAU_AST_H
#define TAU_AST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"
#include "token.h"
#include "op.h"

typedef enum ast_kind_e
{
  AST_UNKNOWN = 0, // unknown node

  AST_ID, // identifier

  AST_TYPE_MUT, // mutable type
  AST_TYPE_CONST, // compile time type
  AST_TYPE_PTR, // pointer type
  AST_TYPE_ARRAY, // array type
  AST_TYPE_REF, // reference type
  AST_TYPE_NULLABLE, // nullable type
  AST_TYPE_FUN, // function type
  AST_TYPE_GEN, // generator type
  AST_TYPE_TYPE, // type type
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
  AST_EXPR_OP, // expression operation
  
  AST_STMT_IF, // statement if
  AST_STMT_FOR, // statement for
  AST_STMT_WHILE, // statement while
  AST_STMT_BREAK, // statement break
  AST_STMT_CONTINUE, // statement continue
  AST_STMT_RETURN, // statement return
  AST_STMT_YIELD, // statement yield
  AST_STMT_BLOCK, // statement block
  AST_STMT_EXPR, // statement expression
  
  AST_DECL_VAR, // declaration variable
  AST_DECL_FUN, // declaration function
  AST_DECL_GEN, // declaration generator
  AST_DECL_STRUCT, // declaration struct
  AST_DECL_UNION, // declaration union
  AST_DECL_ENUM, // declaration enum
  AST_DECL_MOD, // declaration module
  AST_DECL_MEMBER, // declaration member

  AST_PARAM, // function/generator parameter
  AST_VARIADIC_PARAM, // variadic parameter
  AST_GENERIC_PARAM, // generic parameter

  AST_LOOP_VAR, // loop variable

  AST_ENUMERATOR, // enumerator

  AST_PROG, // program
} ast_kind_t;

typedef struct ast_node_s ast_node_t;

struct ast_node_s
{
  ast_kind_t kind;
  
  token_t* tok;

  union {
    struct {
      ast_node_t* base_type;
    } type_mut,
      type_const,
      type_ptr,
      type_ref,
      type_nullable;

    struct {
      ast_node_t *size, *base_type;
    } type_array;

    struct {
      list_t* params;
      ast_node_t *ret_type;
    } type_fun,
      type_gen;

    struct {
      op_kind_t kind;

      union {
        struct {
          ast_node_t *lhs, *rhs;
        } op_binary;

        struct {
          ast_node_t* arg;
        } op_unary;

        struct {
          ast_node_t* callee;
          list_t* args;
        } op_call;
      };
    } expr_op;

    struct {
      ast_node_t *cond, *stmt, *stmt_else;
    } stmt_if;

    struct {
      ast_node_t *var, *range, *stmt;
    } stmt_for;

    struct {
      ast_node_t *cond, *stmt;
    } stmt_while;

    struct {
      ast_node_t* expr;
    } stmt_return,
      stmt_yield,
      stmt_expr;

    struct {
      list_t* stmts;
    } stmt_block;

    struct {
      ast_node_t *id, *type, *init;
    } decl_var;

    struct {
      list_t* generic_params;
      list_t* params;
      ast_node_t *id, *ret_type, *stmt;
    } decl_fun,
      decl_gen;

    struct {
      list_t* generic_params;
      ast_node_t *id;
      list_t* members;
    } decl_struct;

    struct {
      ast_node_t *id;
      list_t* members;
    } decl_union,
      decl_enum,
      decl_mod;

    struct {
      ast_node_t *decl;
    } decl_member;

    struct {
      ast_node_t *id, *type, *init;
    } param;

    struct {
      ast_node_t *id, *type;
    } generic_param,
      variadic_param,
      loop_var;
    
    struct {
      ast_node_t* id;
    } enumerator;

    struct {
      list_t* decls;
    } prog;
  };
};

ast_node_t* ast_node_init(ast_kind_t kind);
void ast_node_free(ast_node_t* node);

const char* ast_kind_to_string(ast_kind_t kind);

void ast_json_dump(FILE* stream, ast_node_t* root);

#endif