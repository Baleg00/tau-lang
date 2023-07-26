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
#include <stdint.h>

#include "list.h"
#include "location.h"
#include "stack.h"
#include "typedefs.h"

/** Utility macro which expands to fields that all nodes must have. */
#define AST_NODE_HEADER\
  struct\
  {\
    ast_kind_t kind; /** Node kind. */\
    token_t* tok; /** First token in this node. */\
  }

/** Utility macro which expands to fields that all type nodes must have. */
#define AST_TYPE_HEADER\
  struct\
  {\
    typedesc_t* desc; /** Type descriptor. */\
  }

/** Utility macro which expands to fields that all expression nodes must have. */
#define AST_EXPR_HEADER\
  struct\
  {\
    typedesc_t* desc; /** Type descriptor. */\
  }

/** Utility macro which expands to fields that all operator expression nodes must have. */
#define AST_EXPR_OP_HEADER\
  struct\
  {\
    op_kind_t op_kind; /** Operator kind. */\
  }

/** Utility macro which expands to fields that all declaration nodes must have. */
#define AST_DECL_HEADER\
  struct\
  {\
    ast_node_t* id; /** Declaration identifier */\
    typedesc_t* desc; /** Type descriptor. */\
  }

/** Utility macro which expands to fields that all parameter nodes must have. */
#define AST_PARAM_HEADER\
  struct\
  {\
    ast_node_t* id; /** Parameter identifier. */\
    ast_node_t* type; /** Parameter type. */\
    typedesc_t* desc; /** Type descriptor. */\
  }

struct ast_node_s
{
  AST_NODE_HEADER;
};

struct ast_id_s
{
  AST_NODE_HEADER;
};

struct ast_type_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
};

struct ast_type_mut_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  ast_node_t* base_type; // Underlying type.
};

struct ast_type_const_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  ast_node_t* base_type; // Underlying type.
};

struct ast_type_ptr_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  ast_node_t* base_type; // Pointed type.
};

struct ast_type_ref_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  ast_node_t* base_type; // Referenced type.
};

struct ast_type_opt_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  ast_node_t* base_type; // Underlying type.
};

struct ast_type_array_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  ast_node_t* base_type; // Contained type.
  ast_node_t* size; // Size of array or null if not specified.
};

struct ast_type_fun_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  list_t* params; // List of function parameters.
  ast_node_t* return_type; // Return type.
};

struct ast_type_gen_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  list_t* params; // List of generator parameters.
  ast_node_t* yield_type; // Yield type.
};

struct ast_type_member_s
{
  AST_NODE_HEADER;
  AST_TYPE_HEADER;
  ast_node_t* owner; // Owner node.
  ast_node_t* member; // Member node.
};

struct ast_expr_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
};

struct ast_expr_lit_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
};

struct ast_expr_op_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  AST_EXPR_OP_HEADER;
};

struct ast_expr_op_bin_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* lhs; // Left-hand side parameter.
  ast_node_t* rhs; // Right-hand side parameter.
};

struct ast_expr_op_un_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* param; // Operator parameter.
};

struct ast_expr_op_call_s
{
  AST_NODE_HEADER;
  AST_EXPR_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* callee; // Expression to be called.
  list_t* params; // List of parameters.
};

struct ast_stmt_s
{
  AST_NODE_HEADER;
};

struct ast_stmt_if_s
{
  AST_NODE_HEADER;
  ast_node_t* cond; // Condition.
  ast_node_t* stmt; // Statement to execute if condition is met.
  ast_node_t* stmt_else; // Statement to execute if condition is not met or null.
};

struct ast_stmt_for_s
{
  AST_NODE_HEADER;
  ast_node_t* var; // Loop variable.
  ast_node_t* range; // Expression evaluating to a range-like type.
  ast_node_t* stmt; // Statement to execute for each iteration.
};

struct ast_stmt_while_s
{
  AST_NODE_HEADER;
  ast_node_t* cond; // Condition.
  ast_node_t* stmt; // Statement to execute for each iteration.
};

struct ast_stmt_break_s
{
  AST_NODE_HEADER;
};

struct ast_stmt_continue_s
{
  AST_NODE_HEADER;
};

struct ast_stmt_return_s
{
  AST_NODE_HEADER;
  ast_node_t* expr; // Expression to be returned or null pointer.
};

struct ast_stmt_yield_s
{
  AST_NODE_HEADER;
  ast_node_t* expr; // Expression to be yielded or null pointer.
};

struct ast_stmt_expr_s
{
  AST_NODE_HEADER;
  ast_node_t* expr; // Expression.
};

struct ast_stmt_block_s
{
  AST_NODE_HEADER;
  list_t* stmts; // List of statements in block.
};

struct ast_decl_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
};

struct ast_decl_var_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  ast_node_t* type; // Type of variable.
  ast_node_t* init; // Initializer expression or null pointer.
};

struct ast_decl_loop_var_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  ast_node_t* type; // Variable type.
};

struct ast_decl_fun_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* params; // List of function parameters.
  ast_node_t* return_type; // Return type.
  ast_node_t* stmt; // Statement.
};

struct ast_decl_gen_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* params; // List of generator parameters.
  ast_node_t* yield_type; // Yield type.
  ast_node_t* stmt; // Statement.
};

struct ast_decl_struct_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* members; // List of member variables.
};

struct ast_decl_union_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* members; // List of member variables.
};

struct ast_decl_enum_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* values; // List of enumerator values.
};

struct ast_decl_mod_s
{
  AST_NODE_HEADER;
  AST_DECL_HEADER;
  list_t* decls; // List of declarations.
};

struct ast_decl_generic_s
{
  AST_NODE_HEADER;
  list_t* params; // List of generic parameters.
  ast_node_t* decl; // Declaration.
};

struct ast_param_s
{
  AST_NODE_HEADER;
  AST_PARAM_HEADER;
};

struct ast_param_default_s
{
  AST_NODE_HEADER;
  AST_PARAM_HEADER;
  ast_node_t* init; // Initializer expression.
};

struct ast_param_variadic_s
{
  AST_NODE_HEADER;
  AST_PARAM_HEADER;
};

struct ast_param_generic_s
{
  AST_NODE_HEADER;
  AST_PARAM_HEADER;
};

struct ast_enumerator_s
{
  AST_NODE_HEADER;
  ast_node_t* id; // Enumerator identifier.
  typedesc_t* desc; // Type descriptor.
};

struct ast_prog_s
{
  AST_NODE_HEADER;
  list_t* decls; // List of declarations.
};

#undef AST_PARAM_HEADER
#undef AST_DECL_HEADER
#undef AST_EXPR_OP_HEADER
#undef AST_NODE_HEADER

/**
 * \brief Initializes an AST node instance.
 * 
 * \param[out] node Node instance to be initialized.
 * \param[in] kind Node kind.
 * \param[in] tok First token of node.
*/
void ast_node_init(ast_node_t* node, ast_kind_t kind, token_t* tok);

#define ast_id_ptr(NODE) (location_get_ptr(((ast_id_t*)(NODE))->tok->loc))
#define ast_id_len(NODE) (location_get_len(((ast_id_t*)(NODE))->tok->loc))

/**
 * \brief Destroys an AST node and all of its children.
 * 
 * \param[in] node Node to be destroyed.
*/
void ast_node_free(ast_node_t* node);

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
 * \brief Checks if a node is a type.
 * 
 * \param[in] node Node to be checked.
 * \returns True if node is a type, false otherwise.
*/
bool ast_is_type(ast_node_t* node);

/**
 * \brief Checks if a node is an expression.
 * 
 * \param[in] node Node to be checked.
 * \returns True if node is an expression, false otherwise.
*/
bool ast_is_expr(ast_node_t* node);

/**
 * \brief Checks if a node is a statement.
 * 
 * \param[in] node Node to be checked.
 * \returns True if node is a statement, false otherwise.
*/
bool ast_is_stmt(ast_node_t* node);

/**
 * \brief Checks if a node is a declaration.
 * 
 * \param[in] node Node to be checked.
 * \returns True if node is a declaration, false otherwise.
*/
bool ast_is_decl(ast_node_t* node);

/**
 * \brief Checks if a node is a parameter.
 * 
 * \param[in] node Node to be checked.
 * \returns True if node is a parameter, false otherwise.
*/
bool ast_is_param(ast_node_t* node);

/**
 * \brief Queries the type descriptor of a node.
 *  
 * \param[in] node Node whose type descriptor is to be queried.
 * \returns Type descriptor of node.
 */
typedesc_t* ast_desc_of(ast_node_t* node);

#endif