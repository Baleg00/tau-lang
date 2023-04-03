/**
 * \file
 * 
 * Abstract syntax tree (AST).
*/

#ifndef TAU_AST_H
#define TAU_AST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"
#include "token.h"
#include "op.h"

/** Indicates the category of a node. */
typedef enum ast_flag_e
{
  AST_FLAG_UNKNOWN    = 0x0000000, // Unknown
  AST_FLAG_ID         = 0x0010000, // Identifier
  AST_FLAG_TYPE       = 0x0020000, // Type
  AST_FLAG_EXPR       = 0x0040000, // Expression
  AST_FLAG_STMT       = 0x0080000, // Statement
  AST_FLAG_DECL       = 0x0100000, // Declaration
  AST_FLAG_PARAM      = 0x0200000, // Parameter
  AST_FLAG_LOOP_VAR   = 0x0400000, // Loop variable
  AST_FLAG_ENUMERATOR = 0x0800000, // Enumerator
  AST_FLAG_PROG       = 0x1000000, // Program
} ast_flag_t;

typedef enum ast_kind_e
{
  AST_UNKNOWN = AST_FLAG_UNKNOWN, // Unknown node

  AST_ID = AST_FLAG_ID, // Identifier

  AST_TYPE_MUT = AST_FLAG_TYPE, // Mutable type
  AST_TYPE_CONST, // Compile-time type
  AST_TYPE_PTR, // Pointer type
  AST_TYPE_ARRAY, // Array type
  AST_TYPE_REF, // Reference type
  AST_TYPE_OPTIONAL, // Optional type
  AST_TYPE_FUN, // Function type
  AST_TYPE_GEN, // Generator type
  AST_TYPE_TYPE, // Type type
  AST_TYPE_SELF, // Self type
  AST_TYPE_BUILTIN_I8, // Built-in type i8
  AST_TYPE_BUILTIN_I16, // Built-in type i16
  AST_TYPE_BUILTIN_I32, // Built-in type i32
  AST_TYPE_BUILTIN_I64, // Built-in type i64
  AST_TYPE_BUILTIN_ISIZE, // Built-in type isize
  AST_TYPE_BUILTIN_U8, // Built-in type u8
  AST_TYPE_BUILTIN_U16, // Built-in type u16
  AST_TYPE_BUILTIN_U32, // Built-in type u32
  AST_TYPE_BUILTIN_U64, // Built-in type u64
  AST_TYPE_BUILTIN_USIZE, // Built-in type usize
  AST_TYPE_BUILTIN_F32, // Built-in type f32
  AST_TYPE_BUILTIN_F64, // Built-in type f64
  AST_TYPE_BUILTIN_BOOL, // Built-in type bool
  AST_TYPE_BUILTIN_UNIT, // Built-in type unit

  AST_EXPR_LIT_INT = AST_FLAG_EXPR, // Literal integer expression
  AST_EXPR_LIT_FLT, // Literal float expression
  AST_EXPR_LIT_STR, // Literal string expression
  AST_EXPR_LIT_CHAR, // Literal character expression
  AST_EXPR_LIT_BOOL, // Literal boolean expression
  AST_EXPR_LIT_NULL, // Literal null expression
  AST_EXPR_OP, // Operation expression
  
  AST_STMT_IF = AST_FLAG_STMT, // If-statement
  AST_STMT_FOR, // For-statement
  AST_STMT_WHILE, // While-statement
  AST_STMT_BREAK, // Break-statement
  AST_STMT_CONTINUE, // Continue-statement
  AST_STMT_RETURN, // Return-statement
  AST_STMT_YIELD, // Yield-statement
  AST_STMT_BLOCK, // Block-statement
  AST_STMT_EXPR, // Expression-statement
  
  AST_DECL_VAR = AST_FLAG_DECL, // Variable declaration
  AST_DECL_FUN, // Function declaration
  AST_DECL_GEN, // Generator declaration
  AST_DECL_STRUCT, // Struct declaration
  AST_DECL_UNION, // Union declaration
  AST_DECL_ENUM, // Enum declaration
  AST_DECL_MOD, // Module declaration
  AST_DECL_MEMBER, // Member declaration

  AST_PARAM = AST_FLAG_PARAM, // Function or generator parameter
  AST_VARIADIC_PARAM, // Variadic parameter
  AST_GENERIC_PARAM, // Generic parameter

  AST_LOOP_VAR = AST_FLAG_LOOP_VAR, // Loop variable

  AST_ENUMERATOR = AST_FLAG_ENUMERATOR, // Enumerator

  AST_PROG = AST_FLAG_PROG, // Program
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