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

#include "stages/analysis/nameres.h"
#include "stages/analysis/typecheck.h"
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

  AST_TYPE_ID, // Type identifier.
  AST_TYPE_MUT, // Mutable type.
  AST_TYPE_CONST, // Compile-time type.
  AST_TYPE_PTR, // Pointer type.
  AST_TYPE_ARRAY, // Array type.
  AST_TYPE_REF, // Reference type.
  AST_TYPE_OPT, // Optional type.
  AST_TYPE_FUN, // Function type.
  AST_TYPE_PRIM_I8, // Primitive type `i8`.
  AST_TYPE_PRIM_I16, // Primitive type `i16`.
  AST_TYPE_PRIM_I32, // Primitive type `i32`.
  AST_TYPE_PRIM_I64, // Primitive type `i64`.
  AST_TYPE_PRIM_ISIZE, // Primitive type `isize`.
  AST_TYPE_PRIM_U8, // Primitive type `u8`.
  AST_TYPE_PRIM_U16, // Primitive type `u16`.
  AST_TYPE_PRIM_U32, // Primitive type `u32`.
  AST_TYPE_PRIM_U64, // Primitive type `u64`.
  AST_TYPE_PRIM_USIZE, // Primitive type `usize`.
  AST_TYPE_PRIM_F32, // Primitive type `f32`.
  AST_TYPE_PRIM_F64, // Primitive type `f64`.
  AST_TYPE_PRIM_CHAR, // Primitive type `char`.
  AST_TYPE_PRIM_BOOL, // Primitive type `bool`.
  AST_TYPE_PRIM_UNIT, // Primitive type `unit`.
  AST_TYPE_MEMBER, // Member type.

  AST_EXPR_ID, // Identifier expression.
  AST_EXPR_LIT_INT, // Literal integer expression.
  AST_EXPR_LIT_FLT, // Literal float expression.
  AST_EXPR_LIT_STR, // Literal string expression.
  AST_EXPR_LIT_CHAR, // Literal character expression.
  AST_EXPR_LIT_BOOL, // Literal boolean expression.
  AST_EXPR_LIT_NULL, // Literal null expression.
  AST_EXPR_OP_UNARY, // Unary operation expression.
  AST_EXPR_OP_BINARY, // Binary operation expression.
  AST_EXPR_OP_CALL, // Call operation expression.
  
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
 * \brief Frees all memory allocated by an AST node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void ast_node_free(ast_node_t* node);

/**
 * \brief Performs name resolution pass on an AST node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_node_nameres(nameres_ctx_t* ctx, ast_node_t* node);

/**
 * \brief Performs type check pass on an AST node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_node_typecheck(typecheck_ctx_t* ctx, ast_node_t* node);

/**
 * \brief Writes a JSON dump of a vector of AST nodes into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] vec The vector of the AST nodes to be dumped.
 */
void ast_node_dump_json_vector(FILE* stream, vector_t* vec);

/**
 * \brief Writes a JSON dump of an AST node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
 */
void ast_node_dump_json(FILE* stream, ast_node_t* node);

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