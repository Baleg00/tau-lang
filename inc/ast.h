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

#include "typedefs.h"

/** Utility macro which expands to fields that all nodes must have. */
#define AST_NODE_HEADER\
  struct\
  {\
    ast_kind_t kind; /** Node kind. */\
    token_t* tok; /** First token in this node. */\
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
  }

/** Utility macro which expands to fields that all parameter nodes must have. */
#define AST_PARAM_HEADER\
  struct\
  {\
    ast_node_t* id; /** Parameter identifier. */\
    ast_node_t* type; /** Parameter type. */\
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
};

struct ast_type_mut_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type; // Underlying type.
};

struct ast_type_const_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type; // Underlying type.
};

struct ast_type_ptr_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type; // Pointed type.
};

struct ast_type_ref_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type; // Referenced type.
};

struct ast_type_opt_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type; // Underlying type.
};

struct ast_type_array_s
{
  AST_NODE_HEADER;
  ast_node_t* base_type; // Contained type.
  ast_node_t* size; // Size of array or null if not specified.
};

struct ast_type_fun_s
{
  AST_NODE_HEADER;
  list_t* params; // List of function parameters.
  ast_node_t* return_type; // Return type.
};

struct ast_type_gen_s
{
  AST_NODE_HEADER;
  list_t* params; // List of generator parameters.
  ast_node_t* yield_type; // Yield type.
};

struct ast_type_member_s
{
  AST_NODE_HEADER;
  ast_node_t* owner; // Owner node.
  ast_node_t* member; // Member node.
};

struct ast_expr_s
{
  AST_NODE_HEADER;
};

struct ast_expr_op_s
{
  AST_NODE_HEADER;
  AST_EXPR_OP_HEADER;
};

struct ast_expr_op_bin_s
{
  AST_NODE_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* lhs; // Left-hand side parameter.
  ast_node_t* rhs; // Right-hand side parameter.
};

struct ast_expr_op_un_s
{
  AST_NODE_HEADER;
  AST_EXPR_OP_HEADER;
  ast_node_t* param; // Operator parameter.
};

struct ast_expr_op_call_s
{
  AST_NODE_HEADER;
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
 * \brief Initializes a new AST node.
 * 
 * \param[in] kind Node kind.
 * \param[in] tok First token of node.
 * \param[in] size Size of node in bytes.
 * \returns New node.
*/
ast_node_t* ast_node_init(ast_kind_t kind, token_t* tok, size_t size);

#define ast_id_init(TOK)             ((ast_id_t*)             ast_node_init(AST_ID,             (TOK), sizeof(ast_id_t)))

#define ast_type_mut_init(TOK)       ((ast_type_mut_t*)       ast_node_init(AST_TYPE_MUT,       (TOK), sizeof(ast_type_mut_t)))
#define ast_type_const_init(TOK)     ((ast_type_const_t*)     ast_node_init(AST_TYPE_CONST,     (TOK), sizeof(ast_type_const_t)))
#define ast_type_ptr_init(TOK)       ((ast_type_ptr_t*)       ast_node_init(AST_TYPE_PTR,       (TOK), sizeof(ast_type_ptr_t)))
#define ast_type_array_init(TOK)     ((ast_type_array_t*)     ast_node_init(AST_TYPE_ARRAY,     (TOK), sizeof(ast_type_array_t)))
#define ast_type_ref_init(TOK)       ((ast_type_ref_t*)       ast_node_init(AST_TYPE_REF,       (TOK), sizeof(ast_type_ref_t)))
#define ast_type_opt_init(TOK)       ((ast_type_opt_t*)       ast_node_init(AST_TYPE_OPT,       (TOK), sizeof(ast_type_opt_t)))
#define ast_type_fun_init(TOK)       ((ast_type_fun_t*)       ast_node_init(AST_TYPE_FUN,       (TOK), sizeof(ast_type_fun_t)))
#define ast_type_gen_init(TOK)       ((ast_type_gen_t*)       ast_node_init(AST_TYPE_GEN,       (TOK), sizeof(ast_type_gen_t)))
#define ast_type_type_init(TOK)      ((ast_type_t*)           ast_node_init(AST_TYPE_TYPE,      (TOK), sizeof(ast_type_t)))
#define ast_type_self_init(TOK)      ((ast_type_t*)           ast_node_init(AST_TYPE_SELF,      (TOK), sizeof(ast_type_t)))
#define ast_type_i8_init(TOK)        ((ast_type_t*)           ast_node_init(AST_TYPE_I8,        (TOK), sizeof(ast_type_t)))
#define ast_type_i16_init(TOK)       ((ast_type_t*)           ast_node_init(AST_TYPE_I16,       (TOK), sizeof(ast_type_t)))
#define ast_type_i32_init(TOK)       ((ast_type_t*)           ast_node_init(AST_TYPE_I32,       (TOK), sizeof(ast_type_t)))
#define ast_type_i64_init(TOK)       ((ast_type_t*)           ast_node_init(AST_TYPE_I64,       (TOK), sizeof(ast_type_t)))
#define ast_type_isize_init(TOK)     ((ast_type_t*)           ast_node_init(AST_TYPE_ISIZE,     (TOK), sizeof(ast_type_t)))
#define ast_type_u8_init(TOK)        ((ast_type_t*)           ast_node_init(AST_TYPE_U8,        (TOK), sizeof(ast_type_t)))
#define ast_type_u16_init(TOK)       ((ast_type_t*)           ast_node_init(AST_TYPE_U16,       (TOK), sizeof(ast_type_t)))
#define ast_type_u32_init(TOK)       ((ast_type_t*)           ast_node_init(AST_TYPE_U32,       (TOK), sizeof(ast_type_t)))
#define ast_type_u64_init(TOK)       ((ast_type_t*)           ast_node_init(AST_TYPE_U64,       (TOK), sizeof(ast_type_t)))
#define ast_type_usize_init(TOK)     ((ast_type_t*)           ast_node_init(AST_TYPE_USIZE,     (TOK), sizeof(ast_type_t)))
#define ast_type_f32_init(TOK)       ((ast_type_t*)           ast_node_init(AST_TYPE_F32,       (TOK), sizeof(ast_type_t)))
#define ast_type_f64_init(TOK)       ((ast_type_t*)           ast_node_init(AST_TYPE_F64,       (TOK), sizeof(ast_type_t)))
#define ast_type_bool_init(TOK)      ((ast_type_t*)           ast_node_init(AST_TYPE_BOOL,      (TOK), sizeof(ast_type_t)))
#define ast_type_unit_init(TOK)      ((ast_type_t*)           ast_node_init(AST_TYPE_UNIT,      (TOK), sizeof(ast_type_t)))
#define ast_type_member_init(TOK)    ((ast_type_member_t*)    ast_node_init(AST_TYPE_MEMBER,    (TOK), sizeof(ast_type_member_t)))

#define ast_expr_lit_int_init(TOK)   ((ast_expr_t*)           ast_node_init(AST_EXPR_LIT_INT,   (TOK), sizeof(ast_expr_t)))
#define ast_expr_lit_flt_init(TOK)   ((ast_expr_t*)           ast_node_init(AST_EXPR_LIT_FLT,   (TOK), sizeof(ast_expr_t)))
#define ast_expr_lit_str_init(TOK)   ((ast_expr_t*)           ast_node_init(AST_EXPR_LIT_STR,   (TOK), sizeof(ast_expr_t)))
#define ast_expr_lit_char_init(TOK)  ((ast_expr_t*)           ast_node_init(AST_EXPR_LIT_CHAR,  (TOK), sizeof(ast_expr_t)))
#define ast_expr_lit_bool_init(TOK)  ((ast_expr_t*)           ast_node_init(AST_EXPR_LIT_BOOL,  (TOK), sizeof(ast_expr_t)))
#define ast_expr_lit_null_init(TOK)  ((ast_expr_t*)           ast_node_init(AST_EXPR_LIT_NULL,  (TOK), sizeof(ast_expr_t)))
#define ast_expr_op_bin_init(TOK)    ((ast_expr_op_bin_t*)    ast_node_init(AST_EXPR_OP,        (TOK), sizeof(ast_expr_op_bin_t)))
#define ast_expr_op_un_init(TOK)     ((ast_expr_op_un_t*)     ast_node_init(AST_EXPR_OP,        (TOK), sizeof(ast_expr_op_un_t)))
#define ast_expr_op_call_init(TOK)   ((ast_expr_op_call_t*)   ast_node_init(AST_EXPR_OP,        (TOK), sizeof(ast_expr_op_call_t)))

#define ast_stmt_if_init(TOK)        ((ast_stmt_if_t*)        ast_node_init(AST_STMT_IF,        (TOK), sizeof(ast_stmt_if_t)))
#define ast_stmt_for_init(TOK)       ((ast_stmt_for_t*)       ast_node_init(AST_STMT_FOR,       (TOK), sizeof(ast_stmt_for_t)))
#define ast_stmt_while_init(TOK)     ((ast_stmt_while_t*)     ast_node_init(AST_STMT_WHILE,     (TOK), sizeof(ast_stmt_while_t)))
#define ast_stmt_break_init(TOK)     ((ast_stmt_break_t*)     ast_node_init(AST_STMT_BREAK,     (TOK), sizeof(ast_stmt_break_t)))
#define ast_stmt_continue_init(TOK)  ((ast_stmt_continue_t*)  ast_node_init(AST_STMT_CONTINUE,  (TOK), sizeof(ast_stmt_continue_t)))
#define ast_stmt_return_init(TOK)    ((ast_stmt_return_t*)    ast_node_init(AST_STMT_RETURN,    (TOK), sizeof(ast_stmt_return_t)))
#define ast_stmt_yield_init(TOK)     ((ast_stmt_yield_t*)     ast_node_init(AST_STMT_YIELD,     (TOK), sizeof(ast_stmt_yield_t)))
#define ast_stmt_block_init(TOK)     ((ast_stmt_block_t*)     ast_node_init(AST_STMT_BLOCK,     (TOK), sizeof(ast_stmt_block_t)))
#define ast_stmt_expr_init(TOK)      ((ast_stmt_expr_t*)      ast_node_init(AST_STMT_EXPR,      (TOK), sizeof(ast_stmt_expr_t)))

#define ast_decl_var_init(TOK)       ((ast_decl_var_t*)       ast_node_init(AST_DECL_VAR,       (TOK), sizeof(ast_decl_var_t)))
#define ast_decl_loop_var_init(TOK)  ((ast_decl_loop_var_t*)  ast_node_init(AST_DECL_LOOP_VAR,  (TOK), sizeof(ast_decl_loop_var_t)))
#define ast_decl_fun_init(TOK)       ((ast_decl_fun_t*)       ast_node_init(AST_DECL_FUN,       (TOK), sizeof(ast_decl_fun_t)))
#define ast_decl_gen_init(TOK)       ((ast_decl_gen_t*)       ast_node_init(AST_DECL_GEN,       (TOK), sizeof(ast_decl_gen_t)))
#define ast_decl_struct_init(TOK)    ((ast_decl_struct_t*)    ast_node_init(AST_DECL_STRUCT,    (TOK), sizeof(ast_decl_struct_t)))
#define ast_decl_union_init(TOK)     ((ast_decl_union_t*)     ast_node_init(AST_DECL_UNION,     (TOK), sizeof(ast_decl_union_t)))
#define ast_decl_enum_init(TOK)      ((ast_decl_enum_t*)      ast_node_init(AST_DECL_ENUM,      (TOK), sizeof(ast_decl_enum_t)))
#define ast_decl_mod_init(TOK)       ((ast_decl_mod_t*)       ast_node_init(AST_DECL_MOD,       (TOK), sizeof(ast_decl_mod_t)))
#define ast_decl_generic_init(TOK)   ((ast_decl_generic_t*)   ast_node_init(AST_DECL_GENERIC,   (TOK), sizeof(ast_decl_generic_t)))

#define ast_param_init(TOK)          ((ast_param_t*)          ast_node_init(AST_PARAM,          (TOK), sizeof(ast_param_t)))
#define ast_param_default_init(TOK)  ((ast_param_default_t*)  ast_node_init(AST_PARAM_DEFAULT,  (TOK), sizeof(ast_param_default_t)))
#define ast_param_variadic_init(TOK) ((ast_param_variadic_t*) ast_node_init(AST_PARAM_VARIADIC, (TOK), sizeof(ast_param_variadic_t)))
#define ast_param_generic_init(TOK)  ((ast_param_generic_t*)  ast_node_init(AST_PARAM_GENERIC,  (TOK), sizeof(ast_param_generic_t)))

#define ast_enumerator_init(TOK)     ((ast_enumerator_t*)     ast_node_init(AST_ENUMERATOR,     (TOK), sizeof(ast_enumerator_t)))

#define ast_prog_init(TOK)           ((ast_prog_t*)           ast_node_init(AST_PROG,           (TOK), sizeof(ast_prog_t)))

#define ast_id_ptr(NODE) (((ast_id_t*)(NODE))->tok->loc->cur)
#define ast_id_len(NODE) (((ast_id_t*)(NODE))->tok->loc->len)

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
 * \brief Determines the size of a type in bytes.
 * 
 * \param[in] node Type node whose size is to be determined.
 * \returns Size of type in bytes.
 */
size_t ast_size_of(ast_node_t* node);

/**
 * \brief Determines the alignment of a type in bytes.
 * 
 * \param[in] node Type node whose alignment is to be determined.
 * \returns Alignment of type in bytes.
 */
size_t ast_align_of(ast_node_t* node);

#endif