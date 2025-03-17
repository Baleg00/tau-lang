/**
 * \file
 * 
 * \brief Abstract syntax tree operation expression node interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_AST_EXPR_OP_H
#define TAU_AST_EXPR_OP_H

#include "ast/expr/expr.h"

/**
 * \brief Header for all AST operation expression nodes.
 */
#define AST_EXPR_OP_HEADER\
  AST_EXPR_HEADER;\
  op_kind_t op_kind;\
  op_subkind_t op_subkind

TAU_EXTERN_C_BEGIN

/**
 * \brief Enumeration of operator kinds.
 */
typedef enum op_kind_t
{
  OP_AS, ///< Type conversion operator `as`
  OP_SIZEOF, ///< Size-of operator `sizeof`
  OP_ALIGNOF, ///< Alignment-of operator `alignof`

  OP_ARIT_INC_PRE, ///< Arithmetic pre-increment operator `++`
  OP_ARIT_INC_POST, ///< Arithmetic post-increment operator `++`
  OP_ARIT_DEC_PRE, ///< Arithmetic pre-decrement operator `--`
  OP_ARIT_DEC_POST, ///< Arithmetic post-decrement operator `--`
  OP_ARIT_POS, ///< Arithmetic positive operator `+`
  OP_ARIT_NEG, ///< Arithmetic negative operator `-`
  OP_ARIT_ADD, ///< Arithmetic add operator `+`
  OP_ARIT_SUB, ///< Arithmetic subtract operator `-`
  OP_ARIT_MUL, ///< Arithmetic multiply operator `*`
  OP_ARIT_DIV, ///< Arithmetic divide operator `/`
  OP_ARIT_MOD, ///< Arithmetic modulo operator `%`

  OP_BIT_AND, ///< Bitwise and operator `&`
  OP_BIT_OR, ///< Bitwise or operator `|`
  OP_BIT_XOR, ///< Bitwise xor operator `^`
  OP_BIT_NOT, ///< Bitwise not operator `~`
  OP_BIT_LSH, ///< Bitwise left-shift operator `<<`
  OP_BIT_RSH, ///< Bitwise right-shift operator `>>`

  OP_LOGIC_AND, ///< Logical and operator `&&`
  OP_LOGIC_OR, ///< Logical or operator `||`
  OP_LOGIC_NOT, ///< Logical not operator `!`

  OP_CMP_EQ, ///< Comparison equal operator `==`
  OP_CMP_NE, ///< Comparison not-equal operator `!=`
  OP_CMP_LT, ///< Comparison less-than operator `<`
  OP_CMP_LE, ///< Comparison less-equal operator `<=`
  OP_CMP_GT, ///< Comparison greater-than operator `>`
  OP_CMP_GE, ///< Comparison greater-equal operator `>=`

  OP_ASSIGN, ///< Assignment operator `=`
  OP_ASSIGN_ARIT_ADD, ///< Arithmetic add-assign operator `+=`
  OP_ASSIGN_ARIT_SUB, ///< Arithmetic subtract-assign operator `-=`
  OP_ASSIGN_ARIT_MUL, ///< Arithmetic multiply-assign operator `*=`
  OP_ASSIGN_ARIT_DIV, ///< Arithmetic divide-assign operator `/=`
  OP_ASSIGN_ARIT_MOD, ///< Arithmetic modulo-assign operator `%=`
  OP_ASSIGN_BIT_AND, ///< Bitwise and-assign operator `&=`
  OP_ASSIGN_BIT_OR, ///< Bitwise or-assign operator `|=`
  OP_ASSIGN_BIT_XOR, ///< Bitwise xor-assign operator `^=`
  OP_ASSIGN_BIT_LSH, ///< Bitwise left-shift-assign operator `<<=`
  OP_ASSIGN_BIT_RSH, ///< Bitwise right-shift-assign operator `>>=`

  OP_SUBS, ///< Subscript operator `[]`
  OP_IND, ///< Indirection operator `*`
  OP_ADDR, ///< Address-of operator `&`
  OP_ACCESS_DIRECT, ///< Direct-access operator `.`
  OP_ACCESS_IND, ///< Indirect-access operator `*.`
  OP_ACCESS_OPT, ///< Optional-access operator `?.`
  OP_UNWRAP_SAFE, ///< Safe optional unwrap operator `?`
  OP_UNWRAP_UNSAFE, ///< Unsafe optional unwrap operator `!`
  OP_RANGE, ///< Range operator `..`
  OP_CALL, ///< Call operator `()`
  OP_SPEC, ///< Generic specialization operator `.<>`
} op_kind_t;

/**
 * \brief Enumeration of operator sub-kinds.
 */
typedef enum op_subkind_t
{
  OP_ARIT_ADD_INTEGER, ///< Arithmetic integer addition
  OP_ARIT_ADD_FLOAT, ///< Arithmetic float addition
  OP_ARIT_ADD_COMPLEX, ///< Arithmetic complex addition
  OP_ARIT_ADD_VECTOR, ///< Arithmetic vector addition
  OP_ARIT_ADD_MATRIX, ///< Arithmetic matrix addition
  OP_ARIT_SUB_INTEGER, ///< Arithmetic integer subtraction
  OP_ARIT_SUB_FLOAT, ///< Arithmetic float subtraction
  OP_ARIT_SUB_COMPLEX, ///< Arithmetic complex subtraction
  OP_ARIT_SUB_VECTOR, ///< Arithmetic vector subtraction
  OP_ARIT_SUB_MATRIX, ///< Arithmetic matrix subtraction
  OP_ARIT_MUL_INTEGER, ///< Arithmetic integer multiplication
  OP_ARIT_MUL_FLOAT, ///< Arithmetic float multiplication
  OP_ARIT_MUL_COMPLEX, ///< Arithmetic complex multiplication
  OP_ARIT_MUL_VECTOR_SCALAR, ///< Arithmetic vector-scalar multiplication
  OP_ARIT_MUL_MATRIX_SCALAR, ///< Arithmetic matrix-scalar multiplication
  OP_CMP_EQ_INTEGER, ///< Integer equality comparison
  OP_CMP_EQ_FLOAT, ///< Float equality comparison
  OP_CMP_EQ_COMPLEX, ///< Complex equality comparison
  OP_CMP_EQ_VECTOR, ///< Vector equality comparison
  OP_CMP_NE_INTEGER, ///< Integer inequality comparison
  OP_CMP_NE_FLOAT, ///< Float inequality comparison
  OP_CMP_NE_COMPLEX, ///< Complex inequality comparison
  OP_CMP_NE_VECTOR, ///< Vector inequality comparison
} op_subkind_t;

/**
 * \brief AST operation expression node.
 */
typedef struct ast_expr_op_t
{
  AST_EXPR_OP_HEADER;
} ast_expr_op_t;

/**
 * \brief Returns a C-string representation of an operator kind.
 * 
 * \param[in] kind Operator kind.
 * \returns C-string representation.
*/
const char* op_kind_to_cstr(op_kind_t kind);

/**
 * \brief Returns the precedence of an operator.
 * 
 * \param kind Operator kind.
 * \returns The precedence of the operator.
 */
int op_precedence(op_kind_t kind);

/**
 * \brief Checks if an operator is binary.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is binary, `false` otherwise.
 */
bool op_is_binary(op_kind_t kind);

/**
 * \brief Checks if an operator is unary.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is unary, `false` otherwise.
 */
bool op_is_unary(op_kind_t kind);

/**
 * \brief Checks if an operator is arithmetic.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is arithmetic, `false` otherwise.
 */
bool op_is_arithmetic(op_kind_t kind);

/**
 * \brief Checks if an operator is bitwise.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is bitwise, `false` otherwise.
 */
bool op_is_bitwise(op_kind_t kind);

/**
 * \brief Checks if an operator is logical.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is logical, `false` otherwise.
 */
bool op_is_logical(op_kind_t kind);

/**
 * \brief Checks if an operator is a comparison.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is a comparison, `false` otherwise.
 */
bool op_is_comparison(op_kind_t kind);

/**
 * \brief Checks if an operator is an assignment.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is an assignment, `false` otherwise.
 */
bool op_is_assignment(op_kind_t kind);

/**
 * \brief Checks if an operator is left associative.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is left associative, `false` otherwise.
 */
bool op_is_left_assoc(op_kind_t kind);

/**
 * \brief Checks if an operator is right associative.
 * 
 * \param kind Operator kind.
 * \returns `true` if the operator is right associative, `false` otherwise.
 */
bool op_is_right_assoc(op_kind_t kind);

TAU_EXTERN_C_END

#endif
