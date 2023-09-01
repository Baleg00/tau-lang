/**
 * \file ast.h
 * 
 * \brief Abstract syntax tree library interface.
 * 
 * \details The Abstract Syntax Tree (AST) is a hierarchical representation of
 * the structure of source code. It captures the syntactic and semantic
 * relationships among the different elements of the code, such as expressions,
 * statements, and declarations. Each node in the AST corresponds to a language
 * construct, and the tree's structure reflects the code's grammar and
 * organization. The AST is used to analyze and transform the source code during
 * the compilation process. It aids in performing tasks like type checking,
 * optimization, code generation, and error detection.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_H
#define TAU_AST_H

#include <stdbool.h>
#include <stdint.h>

#include "list.h"
#include "op.h"
#include "token.h"

/**
 * \brief Flags that specify the category of an AST node.
 */
typedef enum ast_flag_e
{
  AST_FLAG_TYPE      = (1 << 0) << 8, // Type.
  AST_FLAG_TYPED     = (1 << 1) << 8, // Typed.
  AST_FLAG_EXPR      = (1 << 2) << 8, // Expression.
  AST_FLAG_STMT      = (1 << 3) << 8, // Statement.
  AST_FLAG_DECL      = (1 << 4) << 8, // Declaration.
  AST_FLAG_GENERIC   = (1 << 5) << 8, // Generic.
  AST_FLAG_COMPOSITE = (1 << 6) << 8, // Composite.
} ast_flag_t;

/**
 * \brief Enumeration of AST node kinds.
 */
typedef enum ast_kind_e
{
  AST_ID, // Identifier.

  AST_PROG, // Program.

  AST_TYPE_MUT = AST_FLAG_TYPE, // Mutable type.
  AST_TYPE_CONST, // Compile-time type.
  AST_TYPE_PTR, // Pointer type.
  AST_TYPE_ARRAY, // Array type.
  AST_TYPE_REF, // Reference type.
  AST_TYPE_OPT, // Optional type.
  AST_TYPE_FUN, // Function type.
  AST_TYPE_GEN, // Generator type.
  AST_TYPE_TYPE, // Type type.
  AST_TYPE_SELF, // Self type.
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

  AST_EXPR_LIT_INT = AST_FLAG_EXPR, // Literal integer expression.
  AST_EXPR_LIT_FLT, // Literal float expression.
  AST_EXPR_LIT_STR, // Literal string expression.
  AST_EXPR_LIT_CHAR, // Literal character expression.
  AST_EXPR_LIT_BOOL, // Literal boolean expression.
  AST_EXPR_LIT_NULL, // Literal null expression.
  AST_EXPR_OP, // Operation expression.
  
  AST_STMT_IF = AST_FLAG_STMT, // If statement.
  AST_STMT_FOR, // For statement.
  AST_STMT_WHILE, // While statement.
  AST_STMT_BREAK, // Break statement.
  AST_STMT_CONTINUE, // Continue statement.
  AST_STMT_RETURN, // Return statement.
  AST_STMT_YIELD, // Yield statement.
  AST_STMT_BLOCK, // Block statement.
  AST_STMT_EXPR, // Expression statement.

  AST_DECL_MOD = AST_FLAG_DECL, // Module declaration.
  AST_DECL_ENUM = AST_FLAG_DECL | AST_FLAG_COMPOSITE, // Enum declaration.

  AST_DECL_FUN = AST_FLAG_DECL | AST_FLAG_GENERIC, // Function declaration.
  AST_DECL_GEN, // Generator declaration.
  AST_DECL_STRUCT = AST_FLAG_DECL | AST_FLAG_GENERIC | AST_FLAG_COMPOSITE, // Struct declaration.
  AST_DECL_UNION, // Union declaration.
  
  AST_DECL_VAR = AST_FLAG_DECL | AST_FLAG_TYPED, // Variable declaration.
  AST_DECL_PARAM, // Parameter declaration.
  AST_DECL_ENUM_CONSTANT, // Enum constant declaration.
} ast_kind_t;

/**
 * \brief AST node.
 */
typedef struct ast_node_s ast_node_t;

/**
 * \brief Initializes a new AST node with the specified kind.
 * 
 * \param[in] kind The kind of the AST node.
 * \returns Pointer to the newly initialized AST node.
*/
ast_node_t* ast_node_init(ast_kind_t kind);

/**
 * \brief Frees all memory allocated by AST nodes.
*/
void ast_cleanup(void);

/**
 * \brief Writes a JSON dump of an AST into a stream.
 * 
 * \param[in] stream Stream to be written to.
 * \param[in] root Root of AST to be dumped.
*/
void ast_json_dump(FILE* stream, ast_node_t* root);

/**
 * \brief Writes a JSON dump of an AST into a stream such that recursive
 * references are resolved.
 * 
 * \param[in] stream Stream to be written to.
 * \param[in] root Root of AST to be dumped.
*/
void ast_json_dump_flat(FILE* stream, ast_node_t* root);

/**
 * \brief Returns a string representation of a node kind.
 * 
 * \param[in] kind Node kind.
 * \returns String representation.
*/
const char* ast_kind_to_string(ast_kind_t kind);

/**
 * \brief Retrieves the kind of an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The kind of the AST node.
 */
ast_kind_t ast_get_kind(ast_node_t* node);

/**
 * \brief Sets the kind of an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] kind The kind to set.
 */
void ast_set_kind(ast_node_t* node, ast_kind_t kind);

/**
 * \brief Retrieves the token associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns Pointer to the token associated with the node.
 */
token_t* ast_get_token(ast_node_t* node);

/**
 * \brief Sets the token associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] tok Pointer to the token to be associated with the AST node.
 */
void ast_set_token(ast_node_t* node, token_t* tok);

/**
 * \brief Retrieves the operation kind associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The operation kind associated with the node.
 */
op_kind_t ast_get_op(ast_node_t* node);

/**
 * \brief Sets the operation kind associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] op The operation kind to be associated with the AST node.
 */
void ast_set_op(ast_node_t* node, op_kind_t op);

/**
 * \brief Retrieves the identifier associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The identifier associated with the node.
 */
ast_node_t* ast_get_id(ast_node_t* node);

/**
 * \brief Sets the identifier associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] id The identifier to be set.
 */
void ast_set_id(ast_node_t* node, ast_node_t* id);

/**
 * \brief Retrieves the type associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The type associated with the node.
 */
ast_node_t* ast_get_type(ast_node_t* node);

/**
 * \brief Sets the type associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] type The type to be set.
 */
void ast_set_type(ast_node_t* node, ast_node_t* type);

/**
 * \brief Retrieves the base type associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The base type associated with the node.
 */
ast_node_t* ast_get_base_type(ast_node_t* node);

/**
 * \brief Sets the base type associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] base_type The base type to be set.
 */
void ast_set_base_type(ast_node_t* node, ast_node_t* base_type);

/**
 * \brief Retrieves the size associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The size associated with the node.
 */
ast_node_t* ast_get_size(ast_node_t* node);

/**
 * \brief Sets the size associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] size The size to be set.
 */
void ast_set_size(ast_node_t* node, ast_node_t* size);

/**
 * \brief Retrieves the parameters associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The parameters associated with the node.
 */
list_t* ast_get_params(ast_node_t* node);

/**
 * \brief Sets the parameters associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] params The parameters to be set.
 */
void ast_set_params(ast_node_t* node, list_t* params);

/**
 * \brief Retrieves the return type associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The return type associated with the node.
 */
ast_node_t* ast_get_return_type(ast_node_t* node);

/**
 * \brief Sets the return type associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] return_type The return type to be set.
 */
void ast_set_return_type(ast_node_t* node, ast_node_t* return_type);

/**
 * \brief Retrieves the yield type associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The yield type associated with the node.
 */
ast_node_t* ast_get_yield_type(ast_node_t* node);

/**
 * \brief Sets the yield type associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] yield_type The yield type to be set.
 */
void ast_set_yield_type(ast_node_t* node, ast_node_t* yield_type);

/**
 * \brief Retrieves the owner associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The owner associated with the node.
 */
ast_node_t* ast_get_owner(ast_node_t* node);

/**
 * \brief Sets the owner associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] owner The owner to be set.
 */
void ast_set_owner(ast_node_t* node, ast_node_t* owner);

/**
 * \brief Retrieves the member associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The member associated with the node.
 */
ast_node_t* ast_get_member(ast_node_t* node);

/**
 * \brief Sets the member associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] member The member to be set.
 */
void ast_set_member(ast_node_t* node, ast_node_t* member);

/**
 * \brief Retrieves the left-hand side node associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The left-hand side node associated with the node.
 */
ast_node_t* ast_get_lhs(ast_node_t* node);

/**
 * \brief Sets the left-hand side node associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] lhs The left-hand side node to be set.
 */
void ast_set_lhs(ast_node_t* node, ast_node_t* lhs);

/**
 * \brief Retrieves the right-hand side node associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The right-hand side node associated with the node.
 */
ast_node_t* ast_get_rhs(ast_node_t* node);

/**
 * \brief Sets the right-hand side node associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] rhs The right-hand side node to be set.
 */
void ast_set_rhs(ast_node_t* node, ast_node_t* rhs);

/**
 * \brief Retrieves the callee associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The callee associated with the node.
 */
ast_node_t* ast_get_callee(ast_node_t* node);

/**
 * \brief Sets the callee associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] callee The callee to be set.
 */
void ast_set_callee(ast_node_t* node, ast_node_t* callee);

/**
 * \brief Retrieves the condition associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The condition associated with the node.
 */
ast_node_t* ast_get_cond(ast_node_t* node);

/**
 * \brief Sets the condition associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] cond The condition to be set.
 */
void ast_set_cond(ast_node_t* node, ast_node_t* cond);

/**
 * \brief Retrieves the statement associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The statement associated with the node.
 */
ast_node_t* ast_get_stmt(ast_node_t* node);

/**
 * \brief Sets the statement associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] stmt The statement to be set.
 */
void ast_set_stmt(ast_node_t* node, ast_node_t* stmt);

/**
 * \brief Retrieves the else statement associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The else statement associated with the node.
 */
ast_node_t* ast_get_stmt_else(ast_node_t* node);

/**
 * \brief Sets the else statement associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] else_stmt The else statement to be set.
 */
void ast_set_stmt_else(ast_node_t* node, ast_node_t* stmt_else);

/**
 * \brief Retrieves the variable associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The variable associated with the node.
 */
ast_node_t* ast_get_var(ast_node_t* node);

/**
 * \brief Sets the variable associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] var The variable to be set.
 */
void ast_set_var(ast_node_t* node, ast_node_t* var);

/**
 * \brief Retrieves the range associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The range associated with the node.
 */
ast_node_t* ast_get_range(ast_node_t* node);

/**
 * \brief Sets the range associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] range The range to be set.
 */
void ast_set_range(ast_node_t* node, ast_node_t* range);

/**
 * \brief Retrieves the expression associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The expression associated with the node.
 */
ast_node_t* ast_get_expr(ast_node_t* node);

/**
 * \brief Sets the expression associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] expr The expression to be set.
 */
void ast_set_expr(ast_node_t* node, ast_node_t* expr);

/**
 * \brief Retrieves the statements associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The statements associated with the node.
 */
list_t* ast_get_stmts(ast_node_t* node);

/**
 * \brief Sets the statements associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] stmts The statements to be set.
 */
void ast_set_stmts(ast_node_t* node, list_t* stmts);

/**
 * \brief Retrieves the members associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The members associated with the node.
 */
list_t* ast_get_members(ast_node_t* node);

/**
 * \brief Sets the members associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] members The members to be set.
 */
void ast_set_members(ast_node_t* node, list_t* members);

/**
 * \brief Retrieves the declarations associated with an AST node.
 *
 * \param[in] node Pointer to the AST node.
 * \returns The declarations associated with the node.
 */
list_t* ast_get_decls(ast_node_t* node);

/**
 * \brief Sets the declarations associated with an AST node.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] decls The declarations to be set.
 */
void ast_set_decls(ast_node_t* node, list_t* decls);

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

/**
 * \brief Checks if a node is variadic.
 * 
 * \param[in] node Node to be checked.
 * \returns `true` if node is variadic, `false` otherwise.
*/
bool ast_is_variadic(ast_node_t* node);

/**
 * \brief Sets wether an AST node is variadic.
 *
 * \param[in,out] node Pointer to the AST node.
 * \param[in] is_variadic The value to be set.
 */
void ast_set_variadic(ast_node_t* node, bool is_variadic);

#endif