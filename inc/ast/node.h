/**
 * \file
 * 
 * \brief Abstract syntax tree base node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_NODE_H
#define TAU_AST_NODE_H

#include <stdio.h>

#include "stages/analysis/ctrlflow.h"
#include "stages/analysis/nameres.h"
#include "stages/analysis/types/typecheck.h"
#include "stages/codegen/codegen.h"
#include "stages/lexer/token/token.h"

/**
 * \brief Header for all AST nodes.
 */
#define AST_NODE_HEADER\
  ast_kind_t kind; /** AST node kind. */\
  token_t* tok /** The token associated with this node. */

TAU_EXTERN_C_BEGIN

/**
 * \brief Enumeration of AST node kinds.
 */
typedef enum ast_kind_e
{
  AST_ID, ///< Identifier.

  AST_TYPE_ID, ///< Type identifier.
  AST_TYPE_MUT, ///< Mutable type.
  AST_TYPE_CONST, ///< Compile-time type.
  AST_TYPE_PTR, ///< Pointer type.
  AST_TYPE_ARRAY, ///< Array type.
  AST_TYPE_REF, ///< Reference type.
  AST_TYPE_OPT, ///< Optional type.
  AST_TYPE_FUN, ///< Function type.
  AST_TYPE_VEC, ///< Vector type.
  AST_TYPE_PRIM_I8, ///< Primitive type `i8`.
  AST_TYPE_PRIM_I16, ///< Primitive type `i16`.
  AST_TYPE_PRIM_I32, ///< Primitive type `i32`.
  AST_TYPE_PRIM_I64, ///< Primitive type `i64`.
  AST_TYPE_PRIM_ISIZE, ///< Primitive type `isize`.
  AST_TYPE_PRIM_U8, ///< Primitive type `u8`.
  AST_TYPE_PRIM_U16, ///< Primitive type `u16`.
  AST_TYPE_PRIM_U32, ///< Primitive type `u32`.
  AST_TYPE_PRIM_U64, ///< Primitive type `u64`.
  AST_TYPE_PRIM_USIZE, ///< Primitive type `usize`.
  AST_TYPE_PRIM_F32, ///< Primitive type `f32`.
  AST_TYPE_PRIM_F64, ///< Primitive type `f64`.
  AST_TYPE_PRIM_C64, ///< Primitive type `c64`.
  AST_TYPE_PRIM_C128, ///< Primitive type `c128`.
  AST_TYPE_PRIM_CHAR, ///< Primitive type `char`.
  AST_TYPE_PRIM_BOOL, ///< Primitive type `bool`.
  AST_TYPE_PRIM_UNIT, ///< Primitive type `unit`.
  AST_TYPE_MEMBER, ///< Member type.
  AST_TYPE_TYPE, ///< Type of types.

  AST_EXPR_ID, ///< Identifier expression.
  AST_EXPR_LIT_INT, ///< Literal integer expression.
  AST_EXPR_LIT_FLT, ///< Literal float expression.
  AST_EXPR_LIT_STR, ///< Literal string expression.
  AST_EXPR_LIT_CHAR, ///< Literal character expression.
  AST_EXPR_LIT_BOOL, ///< Literal boolean expression.
  AST_EXPR_LIT_NULL, ///< Literal null expression.
  AST_EXPR_OP_UNARY, ///< Unary operation expression.
  AST_EXPR_OP_BINARY, ///< Binary operation expression.
  AST_EXPR_OP_CALL, ///< Call operation expression.
  AST_EXPR_OP_SPEC, ///< Generic specialization operation expression.

  AST_STMT_IF, ///< If statement.
  AST_STMT_FOR, ///< For statement.
  AST_STMT_WHILE, ///< While statement.
  AST_STMT_DO_WHILE, ///<  Do-while statement.
  AST_STMT_LOOP, ///<  Loop statement.
  AST_STMT_BREAK, ///< Break statement.
  AST_STMT_CONTINUE, ///< Continue statement.
  AST_STMT_RETURN, ///< Return statement.
  AST_STMT_DEFER, ///< Defer statement.
  AST_STMT_BLOCK, ///< Block statement.
  AST_STMT_EXPR, ///< Expression statement.

  AST_DECL_VAR, ///< Variable declaration.
  AST_DECL_PARAM, ///< Parameter declaration.
  AST_DECL_FUN, ///< Function declaration.
  AST_DECL_STRUCT, ///< Struct declaration.
  AST_DECL_UNION, ///< Union declaration.
  AST_DECL_ENUM, ///< Enum declaration.
  AST_DECL_ENUM_CONSTANT, ///< Enum constant declaration.
  AST_DECL_MOD, ///< Module declaration.
  AST_DECL_TYPE_ALIAS, ///< Type alias declaration.
  AST_DECL_GENERIC_FUN, ///< Generic function declaration.
  AST_DECL_GENERIC_PARAM, ///< Generic parameter declaration.

  AST_PATH_SEGMENT, ///< Path segment.
  AST_PATH_ACCESS, ///< Path member access.
  AST_PATH_LIST, ///< Path list.
  AST_PATH_WILDCARD, ///< Path wildcard.
  AST_PATH_ALIAS, ///< Path alias.

  AST_USE, ///< Use directive.

  AST_PROG, ///< Program.
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
 * \brief Performs control flow analysis pass on an AST node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void ast_node_ctrlflow(ctrlflow_ctx_t* ctx, ast_node_t* node);

/**
 * \brief Performs code generation pass on an AST node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void ast_node_codegen(codegen_ctx_t* ctx, ast_node_t* node);

/**
 * \brief Writes at most `len` characters (including the null-terminator) of the
 * mangled name of an AST node into `buf`.
 * 
 * \details If `len` is zero, `buf` and `len` are ignored and nothing is written,
 * however the return value is still calculated.
 * 
 * \param[in] node Pointer to the AST node.
 * \param[in,out] buf Pointer to the buffer where the mangled name is to be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters (excluding the null-terminator) that would
 * have been written to the buffer if `len` was ignored.
 */
size_t ast_node_mangle(ast_node_t* node, char* buf, size_t len);

/**
 * \brief Writes at most `len` characters (including the null-terminator) of the
 * mangled nested name of an AST node into `buf`.
 * 
 * \details If `buf` is NULL or `len` is zero, both `buf` and `len` are ignored
 * and nothing is written, however the return value is still calculated.
 * 
 * \param[in] node Pointer to the AST node.
 * \param[in,out] buf Pointer to the buffer where the mangled nested name is to be stored.
 * \param[in] len The length of the buffer.
 * \returns The number of characters (excluding the null-terminator) that would
 * have been written to the buffer if `len` was ignored.
 */
size_t ast_node_mangle_nested_name(ast_node_t* node, char* buf, size_t len);

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

TAU_EXTERN_C_END

#endif
