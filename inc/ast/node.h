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

#include "stages/analysis/ctrlflow.h"
#include "stages/analysis/nameres.h"
#include "stages/analysis/types/typecheck.h"
#include "stages/codegen/codegen.h"
#include "stages/lexer/token/token.h"
#include "utils/common.h"

/**
 * \brief Header for all AST nodes.
 */
#define TAU_AST_NODE_HEADER\
  tau_ast_kind_t kind; /** AST node kind. */\
  tau_token_t* tok /** The token associated with this node. */

TAU_EXTERN_C_BEGIN

/**
 * \brief Enumeration of AST node kinds.
 */
typedef enum tau_ast_kind_e
{
  TAU_AST_ID, ///< Identifier.

  TAU_AST_POISON, ///< Poison node.

  TAU_AST_TYPE_ID, ///< Type identifier.
  TAU_AST_TYPE_MUT, ///< Mutable type.
  TAU_AST_TYPE_CONST, ///< Compile-time type.
  TAU_AST_TYPE_PTR, ///< Pointer type.
  TAU_AST_TYPE_ARRAY, ///< Array type.
  TAU_AST_TYPE_REF, ///< Reference type.
  TAU_AST_TYPE_OPT, ///< Optional type.
  TAU_AST_TYPE_FUN, ///< Function type.
  TAU_AST_TYPE_VEC, ///< Vector type.
  TAU_AST_TYPE_MAT, ///< Matrix type.
  TAU_AST_TYPE_PRIM_I8, ///< Primitive type `i8`.
  TAU_AST_TYPE_PRIM_I16, ///< Primitive type `i16`.
  TAU_AST_TYPE_PRIM_I32, ///< Primitive type `i32`.
  TAU_AST_TYPE_PRIM_I64, ///< Primitive type `i64`.
  TAU_AST_TYPE_PRIM_ISIZE, ///< Primitive type `isize`.
  TAU_AST_TYPE_PRIM_U8, ///< Primitive type `u8`.
  TAU_AST_TYPE_PRIM_U16, ///< Primitive type `u16`.
  TAU_AST_TYPE_PRIM_U32, ///< Primitive type `u32`.
  TAU_AST_TYPE_PRIM_U64, ///< Primitive type `u64`.
  TAU_AST_TYPE_PRIM_USIZE, ///< Primitive type `usize`.
  TAU_AST_TYPE_PRIM_F32, ///< Primitive type `f32`.
  TAU_AST_TYPE_PRIM_F64, ///< Primitive type `f64`.
  TAU_AST_TYPE_PRIM_C64, ///< Primitive type `c64`.
  TAU_AST_TYPE_PRIM_C128, ///< Primitive type `c128`.
  TAU_AST_TYPE_PRIM_CHAR, ///< Primitive type `char`.
  TAU_AST_TYPE_PRIM_BOOL, ///< Primitive type `bool`.
  TAU_AST_TYPE_PRIM_UNIT, ///< Primitive type `unit`.
  TAU_AST_TYPE_MEMBER, ///< Member type.
  TAU_AST_TYPE_TYPE, ///< Type of types.

  TAU_AST_EXPR_ID, ///< Identifier expression.
  TAU_AST_EXPR_LIT_INT, ///< Literal integer expression.
  TAU_AST_EXPR_LIT_FLT, ///< Literal float expression.
  TAU_AST_EXPR_LIT_STR, ///< Literal string expression.
  TAU_AST_EXPR_LIT_CHAR, ///< Literal character expression.
  TAU_AST_EXPR_LIT_BOOL, ///< Literal boolean expression.
  TAU_AST_EXPR_LIT_NULL, ///< Literal null expression.
  TAU_AST_EXPR_LIT_VEC, ///< Literal vector expression.
  TAU_AST_EXPR_LIT_MAT, ///< Literal matrix expression.
  TAU_AST_EXPR_OP_UNARY, ///< Unary operation expression.
  TAU_AST_EXPR_OP_BINARY, ///< Binary operation expression.
  TAU_AST_EXPR_OP_CALL, ///< Call operation expression.
  TAU_AST_EXPR_OP_SPEC, ///< Generic specialization operation expression.

  TAU_AST_STMT_IF, ///< If statement.
  TAU_AST_STMT_FOR, ///< For statement.
  TAU_AST_STMT_WHILE, ///< While statement.
  TAU_AST_STMT_DO_WHILE, ///<  Do-while statement.
  TAU_AST_STMT_LOOP, ///<  Loop statement.
  TAU_AST_STMT_BREAK, ///< Break statement.
  TAU_AST_STMT_CONTINUE, ///< Continue statement.
  TAU_AST_STMT_RETURN, ///< Return statement.
  TAU_AST_STMT_DEFER, ///< Defer statement.
  TAU_AST_STMT_BLOCK, ///< Block statement.
  TAU_AST_STMT_EXPR, ///< Expression statement.

  TAU_AST_DECL_VAR, ///< Variable declaration.
  TAU_AST_DECL_PARAM, ///< Parameter declaration.
  TAU_AST_DECL_FUN, ///< Function declaration.
  TAU_AST_DECL_STRUCT, ///< Struct declaration.
  TAU_AST_DECL_UNION, ///< Union declaration.
  TAU_AST_DECL_ENUM, ///< Enum declaration.
  TAU_AST_DECL_ENUM_CONSTANT, ///< Enum constant declaration.
  TAU_AST_DECL_MOD, ///< Module declaration.
  TAU_AST_DECL_TYPE_ALIAS, ///< Type alias declaration.
  TAU_AST_DECL_GENERIC_FUN, ///< Generic function declaration.
  TAU_AST_DECL_GENERIC_PARAM, ///< Generic parameter declaration.

  TAU_AST_PATH_SEGMENT, ///< Path segment.
  TAU_AST_PATH_ACCESS, ///< Path member access.
  TAU_AST_PATH_LIST, ///< Path list.
  TAU_AST_PATH_WILDCARD, ///< Path wildcard.
  TAU_AST_PATH_ALIAS, ///< Path alias.

  TAU_AST_USE, ///< Use directive.

  TAU_AST_PROG, ///< Program.
} tau_ast_kind_t;

/**
 * \brief AST node.
 */
typedef struct tau_ast_node_t
{
  TAU_AST_NODE_HEADER;
} tau_ast_node_t;

/**
 * \brief Frees all memory allocated by an AST node.
 * 
 * \param[in] node Pointer to the AST node to be freed.
 */
void tau_ast_node_free(tau_ast_node_t* node);

/**
 * \brief Performs name resolution pass on an AST node.
 * 
 * \param[in] ctx Pointer to the name resolution context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_node_nameres(tau_nameres_ctx_t* ctx, tau_ast_node_t* node);

/**
 * \brief Performs type check pass on an AST node.
 * 
 * \param[in] ctx Pointer to the type check context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_node_typecheck(tau_typecheck_ctx_t* ctx, tau_ast_node_t* node);

/**
 * \brief Performs control flow analysis pass on an AST node.
 * 
 * \param[in] ctx Pointer to the control flow analysis context.
 * \param[in,out] node Pointer to the AST node to be visited.
 */
void tau_ast_node_ctrlflow(tau_ctrlflow_ctx_t* ctx, tau_ast_node_t* node);

/**
 * \brief Performs code generation pass on an AST node.
 * 
 * \param[in] ctx Pointer to the code generation context.
 * \param[in] node Pointer to the AST node to be visited.
 */
void tau_ast_node_codegen(tau_codegen_ctx_t* ctx, tau_ast_node_t* node);

/**
 * \brief Writes a JSON dump of a vector of AST nodes into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] vec The vector of the AST nodes to be dumped.
 */
void tau_ast_node_dump_json_vector(FILE* stream, tau_vector_t* vec);

/**
 * \brief Writes a JSON dump of an AST node into a stream.
 * 
 * \param[in] stream The stream to be written to.
 * \param[in] node Pointer to the AST node to be dumped.
 */
void tau_ast_node_dump_json(FILE* stream, tau_ast_node_t* node);

/**
 * \brief Returns a C-string representation of a node kind.
 * 
 * \param[in] kind Node kind.
 * \returns C-string representation.
 */
const char* tau_ast_kind_to_cstr(tau_ast_kind_t kind);

/**
 * \brief Checks if a node is a type.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is a type, `false` otherwise.
 */
bool tau_ast_is_type(tau_ast_node_t* node);

/**
 * \brief Checks if a node is an expression.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is an expression, `false` otherwise.
 */
bool tau_ast_is_expr(tau_ast_node_t* node);

/**
 * \brief Checks if a node is a statement.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is a statement, `false` otherwise.
 */
bool tau_ast_is_stmt(tau_ast_node_t* node);

/**
 * \brief Checks if a node is a declaration.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is a declaration, `false` otherwise.
 */
bool tau_ast_is_decl(tau_ast_node_t* node);

TAU_EXTERN_C_END

#endif
