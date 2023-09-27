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

#include <llvm-c/Core.h>

#include "list.h"
#include "op.h"
#include "token.h"

/**
 * \brief Utility macro which expands to fields that all AST nodes must have.
 */
#define AST_NODE_HEADER\
  struct\
  {\
    ast_kind_t kind; /** AST node kind. */\
    token_t* tok; /** Pointer to the token associated with this AST node. */\
  }

/**
 * \brief Utility macro which expands to fields that all type AST nodes must have.
 */
#define AST_TYPE_HEADER\
  struct\
  {\
    LLVMTypeRef llvm_type;\
  }

/**
 * \brief Utility macro which expands to fields that all type modifier AST nodes
 * must have.
 */
#define AST_MODIFIER_HEADER\
  struct\
  {\
    ast_node_t* base_type; /** Type modifier base type. */\
  }

/**
 * \brief Utility macro which expands to fields that all typed AST nodes must
 * have.
 */
#define AST_TYPED_HEADER\
  struct\
  {\
    ast_node_t* type; /** Type. */\
  }

/**
 * \brief Utility macro which expands to fields that all expression AST nodes
 * must have.
 */
#define AST_EXPR_HEADER\
  struct\
  {\
    LLVMTypeRef llvm_type;\
    LLVMValueRef llvm_value;\
  }

/**
 * \brief Utility macro which expands to fields that all operator expression AST
 * nodes must have.
 */
#define AST_EXPR_OP_HEADER\
  struct\
  {\
    op_kind_t op_kind; /** Operation kind. */\
  }

/**
 * \brief Utility macro which expands to fields that all declaration AST nodes
 * must have.
 */
#define AST_DECL_HEADER\
  struct\
  {\
    ast_node_t* id; /** Identifier. */\
    list_t* attrs; /** Attributes. */\
  }

/**
 * \brief Utility macro which expands to fields that all composite AST nodes
 * must have.
 */
#define AST_COMPOSITE_HEADER\
  struct\
  {\
    list_t* members; /** Members. */\
  }

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
  AST_STMT_YIELD, // Yield statement.
  AST_STMT_BLOCK, // Block statement.
  AST_STMT_EXPR, // Expression statement.

  AST_DECL_VAR, // Variable declaration.
  AST_DECL_PARAM, // Parameter declaration.
  AST_DECL_FUN, // Function declaration.
  AST_DECL_GEN, // Generator declaration.
  AST_DECL_STRUCT, // Struct declaration.
  AST_DECL_UNION, // Union declaration.
  AST_DECL_ENUM, // Enum declaration.
  AST_DECL_ENUM_CONSTANT, // Enum constant declaration.
  AST_DECL_MOD, // Module declaration.

  AST_ATTR, // Attribute.

  AST_PROG, // Program.
} ast_kind_t;

/**
 * \brief AST node.
 */
typedef struct ast_node_s
{
  AST_NODE_HEADER;
} ast_node_t;

/**
 * \brief AST identifier node.
 */
typedef struct ast_id_s
{
  AST_NODE_HEADER;
} ast_id_t;

/**
 * \brief AST type node.
 */
typedef struct ast_type_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
} ast_type_t;

/**
 * \brief AST type modifier node.
 */
typedef struct ast_type_modifier_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  AST_MODIFIER_HEADER;
} ast_type_modifier_t;

/**
 * \brief AST mutable type node.
 */
typedef struct ast_type_mut_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  AST_MODIFIER_HEADER;
} ast_type_mut_t;

/**
 * \brief AST constant type node.
 */
typedef struct ast_type_const_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  AST_MODIFIER_HEADER;
} ast_type_const_t;

/**
 * \brief AST pointer type node.
 */
typedef struct ast_type_ptr_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  AST_MODIFIER_HEADER;
} ast_type_ptr_t;

/**
 * \brief AST reference type node.
 */
typedef struct ast_type_ref_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  AST_MODIFIER_HEADER;
} ast_type_ref_t;

/**
 * \brief AST optional type node.
 */
typedef struct ast_type_opt_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  AST_MODIFIER_HEADER;
} ast_type_opt_t;

/**
 * \brief AST array type node.
 */
typedef struct ast_type_array_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  AST_MODIFIER_HEADER;
  ast_node_t* size;
} ast_type_array_t;

/**
 * \brief AST function type node.
 */
typedef struct ast_type_fun_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  list_t* params; // List of parameter types.
  ast_node_t* return_type; // Return type.
} ast_type_fun_t;

/**
 * \brief AST generator type node.
 */
typedef struct ast_type_gen_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  list_t* params; // List of parameter types.
  ast_node_t* yield_type; // Yield type.
} ast_type_gen_t;

/**
 * \brief AST member type node.
 */
typedef struct ast_type_member_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  ast_node_t* owner; // Owner type.
  ast_node_t* member; // Member type.
} ast_type_member_t;

/**
 * \brief AST expression node.
 */
typedef struct ast_expr_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
} ast_expr_t;

/**
 * \brief AST literal expression node.
 */
typedef struct ast_expr_lit_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
} ast_expr_lit_t;

/**
 * \brief AST operation expression node.
 */
typedef struct ast_expr_op_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  AST_EXPR_OP_HEADER;
} ast_expr_op_t;

/**
 * \brief AST binary operation expression node.
 */
typedef struct ast_expr_op_bin_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* lhs; // Left-hand side expression.
  ast_node_t* rhs; // Right-hand side expression.
} ast_expr_op_bin_t;

/**
 * \brief AST unary operation expression node.
 */
typedef struct ast_expr_op_un_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* expr; // Expression.
} ast_expr_op_un_t;

/**
 * \brief AST call operation expression node.
 */
typedef struct ast_expr_op_call_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* callee; // Callee expression.
  list_t* params; // List of parameter expressions.
} ast_expr_op_call_t;

/**
 * \brief AST variable expression node.
 */
typedef struct ast_expr_decl_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  ast_node_t* decl; // Declaration.
} ast_expr_decl_t;

/**
 * \brief AST statement node.
 */
typedef struct ast_stmt_s
{
  AST_NODE_HEADER;
} ast_stmt_t;

/**
 * \brief AST if statement node.
 */
typedef struct ast_stmt_if_s
{
  AST_NODE_HEADER;
  ast_node_t* cond; // Condition expression.
  ast_node_t* stmt; // Main body statement.
  ast_node_t* stmt_else; // Else statement.

  LLVMValueRef llvm_value;

  LLVMBasicBlockRef llvm_then;
  LLVMBasicBlockRef llvm_else;
  LLVMBasicBlockRef llvm_end;
} ast_stmt_if_t;

/**
 * \brief AST for statement node.
 */
typedef struct ast_stmt_for_s
{
  AST_NODE_HEADER;
  ast_node_t* var; // Loop variable declaration.
  ast_node_t* range; // Range expression.
  ast_node_t* stmt; // Main body statement.
} ast_stmt_for_t;

/**
 * \brief AST while statement node.
 */
typedef struct ast_stmt_while_s
{
  AST_NODE_HEADER;
  ast_node_t* cond; // Condition expression.
  ast_node_t* stmt; // Main body statement.
} ast_stmt_while_t;

/**
 * \brief AST break statement node.
 */
typedef struct ast_stmt_break_s
{
  AST_NODE_HEADER;
} ast_stmt_break_t;

/**
 * \brief AST continue statement node.
 */
typedef struct ast_stmt_continue_s
{
  AST_NODE_HEADER;
} ast_stmt_continue_t;

/**
 * \brief AST return statement node.
 */
typedef struct ast_stmt_return_s
{
  AST_NODE_HEADER;
  ast_node_t* expr; // Expression.
} ast_stmt_return_t;

/**
 * \brief AST yield statement node.
 */
typedef struct ast_stmt_yield_s
{
  AST_NODE_HEADER;
  ast_node_t* expr; // Expression.
} ast_stmt_yield_t;

/**
 * \brief AST expression statement node.
 */
typedef struct ast_stmt_expr_s
{
  AST_NODE_HEADER;
  ast_node_t* expr; // Expression.
} ast_stmt_expr_t;

/**
 * \brief AST block statement node.
 */
typedef struct ast_stmt_block_s
{
  AST_NODE_HEADER;
  list_t* stmts; // List of statements.
} ast_stmt_block_t;

/**
 * \brief AST declaration node.
 */
typedef struct ast_decl_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
} ast_decl_t;

/**
 * \brief AST variable declaration node.
 */
typedef struct ast_decl_var_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_TYPED_HEADER;
  ast_node_t* expr; // Initializer expression.

  LLVMTypeRef llvm_type;
  LLVMValueRef llvm_value;
} ast_decl_var_t;

/**
 * \brief AST function declaration node.
 */
typedef struct ast_decl_fun_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* params; // List of parameter declarations.
  ast_node_t* return_type; // Return type.
  ast_node_t* stmt; // Body statement.
  bool is_extern; // External flag.

  LLVMTypeRef llvm_type;
  LLVMValueRef llvm_value;

  LLVMBasicBlockRef llvm_entry;
  LLVMBasicBlockRef llvm_end;
} ast_decl_fun_t;

/**
 * \brief AST generator declaration node.
 */
typedef struct ast_decl_gen_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* params; // List of parameter declarations.
  ast_node_t* yield_type; // Yield type.
  ast_node_t* stmt; // Body statement.
} ast_decl_gen_t;

/**
 * \brief AST structure declaration node.
 */
typedef struct ast_decl_struct_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_COMPOSITE_HEADER;
} ast_decl_struct_t;

/**
 * \brief AST union declaration node.
 */
typedef struct ast_decl_union_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_COMPOSITE_HEADER;
} ast_decl_union_t;

/**
 * \brief AST enumerator declaration node.
 */
typedef struct ast_decl_enum_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_COMPOSITE_HEADER;
} ast_decl_enum_t;

/**
 * \brief AST module declaration node.
 */
typedef struct ast_decl_mod_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* decls; // List of declarations.
} ast_decl_mod_t;

/**
 * \brief AST parameter declaration node.
 */
typedef struct ast_decl_param_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_TYPED_HEADER;
  ast_node_t* expr; // Default value expression.
  bool is_variadic; // Variadic flag.

  LLVMTypeRef llvm_type;
  LLVMValueRef llvm_value;
} ast_decl_param_t;

/**
 * \brief AST enumeration constant declaration node.
 */
typedef struct ast_decl_enum_constant_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  AST_TYPED_HEADER;
} ast_decl_enum_constant_t;

/**
 * \brief AST attribute node.
 */
typedef struct ast_attr_s
{
  AST_NODE_HEADER;
  ast_node_t* id; // Identifier.
  list_t* params; // List of parameter expressions.
} ast_attr_t;

/**
 * \brief AST program node.
 */
typedef struct ast_prog_s
{
  AST_NODE_HEADER;
  list_t* decls; // List of declarations.
} ast_prog_t;

#undef AST_COMPOSITE_HEADER
#undef AST_DECL_HEADER
#undef AST_EXPR_OP_HEADER
#undef AST_TYPED_HEADER
#undef AST_NODE_HEADER

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