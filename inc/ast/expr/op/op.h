/**
 * \file op.h
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
  op_kind_t op_kind

/**
 * \brief Enumeration of operator kinds.
 */
typedef enum op_kind_t
{
  OP_SIZEOF, // Size-of operator `sizeof`
  OP_ALIGNOF, // Alignment-of operator `alignof`
  OP_TYPEOF, // Type-of operator `typeof`
  OP_IN, // In-range operator `in`
  
  OP_ARIT_INC_PRE, // Arithmetic pre-increment operator `++`
  OP_ARIT_INC_POST, // Arithmetic post-increment operator `++`
  OP_ARIT_DEC_PRE, // Arithmetic pre-decrement operator `--`
  OP_ARIT_DEC_POST, // Arithmetic post-decrement operator `--`
  OP_ARIT_POS, // Arithmetic positive operator `+`
  OP_ARIT_NEG, // Arithmetic negative operator `-`
  OP_ARIT_ADD, // Arithmetic add operator `+`
  OP_ARIT_SUB, // Arithmetic subtract operator `-`
  OP_ARIT_MUL, // Arithmetic multiply operator `*`
  OP_ARIT_DIV, // Arithmetic divide operator `/`
  OP_ARIT_MOD, // Arithmetic modulo operator `%`
  
  OP_BIT_AND, // Bitwise and operator `&`
  OP_BIT_OR, // Bitwise or operator `|`
  OP_BIT_XOR, // Bitwise xor operator `^`
  OP_BIT_NOT, // Bitwise not operator `~`
  OP_BIT_LSH, // Bitwise left-shift operator `<<`
  OP_BIT_RSH, // Bitwise right-shift operator `>>`

  OP_LOGIC_AND, // Logical and operator `&&`
  OP_LOGIC_OR, // Logical or operator `||`
  OP_LOGIC_NOT, // Logical not operator `!`

  OP_COMP_EQ, // Comparison equal operator `==`
  OP_COMP_NE, // Comparison not-equal operator `!=`
  OP_COMP_LT, // Comparison less-than operator `<`
  OP_COMP_LE, // Comparison less-equal operator `<=`
  OP_COMP_GT, // Comparison greater-than operator `>`
  OP_COMP_GE, // Comparison greater-equal operator `>=`

  OP_ASSIGN, // Assignment operator `=`
  OP_ARIT_ADD_ASSIGN, // Arithmetic add-assign operator `+=`
  OP_ARIT_SUB_ASSIGN, // Arithmetic subtract-assign operator `-=`
  OP_ARIT_MUL_ASSIGN, // Arithmetic multiply-assign operator `*=`
  OP_ARIT_DIV_ASSIGN, // Arithmetic divide-assign operator `/=`
  OP_ARIT_MOD_ASSIGN, // Arithmetic modulo-assign operator `%=`
  OP_BIT_AND_ASSIGN, // Bitwise and-assign operator `&=`
  OP_BIT_OR_ASSIGN, // Bitwise or-assign operator `|=`
  OP_BIT_XOR_ASSIGN, // Bitwise xor-assign operator `^=`
  OP_BIT_LSH_ASSIGN, // Bitwise left-shift-assign operator `<<=`
  OP_BIT_RSH_ASSIGN, // Bitwise right-shift-assign operator `>>=`

  OP_SUBS, // Subscript operator `[]`
  OP_IND, // Indirection operator `*`
  OP_ADDR, // Address-of operator `&`
  OP_ACCESS, // Access operator `.`
  OP_IND_ACCESS, // Indirect-access operator `*.`
  OP_NULL_SAFE_ACCESS, // Null-safe-access operator `?.`
  OP_RANGE, // Range operator `..`
  OP_CALL, // Call operator `()`
  OP_AWAIT, // Await operator `await`
} op_kind_t;

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

#endif