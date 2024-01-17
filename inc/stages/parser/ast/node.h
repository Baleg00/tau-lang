/**
 * \file node.h
 * 
 * \brief Abstract syntax tree base node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_NODE_H
#define TAU_AST_NODE_H

#include <stdio.h>

#include "stages/lexer/token.h"

/**
 * \brief Header for all AST nodes.
 */
#define AST_NODE_HEADER\
  ast_kind_t kind; /** AST node kind. */\
  token_t* tok /** The token associated with this node. */

/**
 * \brief Enumeration of AST node kinds.
 */
typedef enum ast_kind_e
{
  AST_ID, // Identifier.

  AST_TYPE_MUT, // Mutable type.
  AST_TYPE_CONST, // Compile-time type.
  AST_TYPE_PTR, // Pointer type.
  AST_TYPE_ARRAY, // Array type.
  AST_TYPE_REF, // Reference type.
  AST_TYPE_OPT, // Optional type.
  AST_TYPE_FUN, // Function type.
  AST_TYPE_I8, // Built-in type `i8`.
  AST_TYPE_I16, // Built-in type `i16`.
  AST_TYPE_I32, // Built-in type `i32`.
  AST_TYPE_I64, // Built-in type `i64`.
  AST_TYPE_ISIZE, // Built-in type `isize`.
  AST_TYPE_U8, // Built-in type `u8`.
  AST_TYPE_U16, // Built-in type `u16`.
  AST_TYPE_U32, // Built-in type `u32`.
  AST_TYPE_U64, // Built-in type `u64`.
  AST_TYPE_USIZE, // Built-in type `usize`.
  AST_TYPE_F32, // Built-in type `f32`.
  AST_TYPE_F64, // Built-in type `f64`.
  AST_TYPE_BOOL, // Built-in type `bool`.
  AST_TYPE_UNIT, // Built-in type `unit`.
  AST_TYPE_MEMBER, // Member type.
  AST_TYPE_DECL, // Declaration type.

  AST_EXPR_LIT_INT, // Literal integer expression.
  AST_EXPR_LIT_FLT, // Literal float expression.
  AST_EXPR_LIT_STR, // Literal string expression.
  AST_EXPR_LIT_CHAR, // Literal character expression.
  AST_EXPR_LIT_BOOL, // Literal boolean expression.
  AST_EXPR_LIT_NULL, // Literal null expression.
  AST_EXPR_OP_UNARY, // Unary operation expression.
  AST_EXPR_OP_BINARY, // Binary operation expression.
  AST_EXPR_OP_CALL, // Call operation expression.
  AST_EXPR_DECL, // Declaration expression.
  
  AST_STMT_IF, // If statement.
  AST_STMT_FOR, // For statement.
  AST_STMT_WHILE, // While statement.
  AST_STMT_BREAK, // Break statement.
  AST_STMT_CONTINUE, // Continue statement.
  AST_STMT_RETURN, // Return statement.
  AST_STMT_DEFER, // Defer statement.
  AST_STMT_BLOCK, // Block statement.
  AST_STMT_EXPR, // Expression statement.

  AST_DECL_VAR, // Variable declaration.
  AST_DECL_PARAM, // Parameter declaration.
  AST_DECL_FUN, // Function declaration.
  AST_DECL_STRUCT, // Struct declaration.
  AST_DECL_UNION, // Union declaration.
  AST_DECL_ENUM, // Enum declaration.
  AST_DECL_ENUM_CONSTANT, // Enum constant declaration.
  AST_DECL_MOD, // Module declaration.

  AST_PROG, // Program.
} ast_kind_t;

/**
 * \brief AST node.
 */
typedef struct ast_node_t
{
  AST_NODE_HEADER;
} ast_node_t;

/**
 * \brief Returns a C-string representation of a node kind.
 * 
 * \param[in] kind Node kind.
 * \returns C-string representation.
*/
const char* ast_kind_to_cstr(ast_kind_t kind);

/**
 * \brief Checks if a node is a type.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is a type, `false` otherwise.
*/
bool ast_is_type(ast_node_t* node);

/**
 * \brief Checks if a node is an expression.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is an expression, `false` otherwise.
*/
bool ast_is_expr(ast_node_t* node);

/**
 * \brief Checks if a node is a statement.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is a statement, `false` otherwise.
*/
bool ast_is_stmt(ast_node_t* node);

/**
 * \brief Checks if a node is a declaration.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is a declaration, `false` otherwise.
*/
bool ast_is_decl(ast_node_t* node);

#endif