/**
 * \file op.h
 * 
 * \brief Operator interface.
 * 
 * \details Operators are symbols or keywords that represent specific operations
 * or actions to be performed on data, typically variables or values. These
 * operations include arithmetic calculations (e.g., addition, subtraction),
 * logical comparisons (e.g., equality, greater than), assignment of values to
 * variables, and more.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_OP_H
#define TAU_OP_H

#include <stdbool.h>

/**
 * \brief Enumeration of operator kinds.
 */
typedef enum op_kind_e
{
  OP_UNKNOWN, // Unknown operator

  OP_IS, // "is" type check
  OP_AS, // "as" type cast
  OP_SIZEOF, // "sizeof" size of expression type
  OP_ALIGNOF, // "alignof" align of expression type
  OP_TYPEOF, // "typeof" type of expression
  OP_IN, // "in" in range
  
  OP_ARIT_INC_PRE, // "++" arithmetic pre increment
  OP_ARIT_INC_POST, // "++" arithmetic post increment
  OP_ARIT_DEC_PRE, // "--" arithmetic pre decrement
  OP_ARIT_DEC_POST, // "--" arithmetic post decrement
  OP_ARIT_POS, // "+" arithmetic positive
  OP_ARIT_NEG, // "-" arithmetic negative
  OP_ARIT_ADD, // "+" arithmetic add
  OP_ARIT_SUB, // "-" arithmetic subtract
  OP_ARIT_MUL, // "*" arithmetic multiply
  OP_ARIT_DIV, // "/" arithmetic divide
  OP_ARIT_MOD, // "%" arithmetic modulo
  
  OP_BIT_AND, // "&" bitwise and
  OP_BIT_OR, // "|" bitwise or
  OP_BIT_XOR, // "^" bitwise xor
  OP_BIT_NOT, // "~" bitwise not
  OP_BIT_LSH, // "<<" bitwise left shift
  OP_BIT_RSH, // ">>" bitwise right shift

  OP_LOGIC_AND, // "&&" logical and
  OP_LOGIC_OR, // "||" logical or
  OP_LOGIC_NOT, // "!" logical not

  OP_COMP_EQ, // "==" comparison equal
  OP_COMP_NE, // "!=" comparison not equal
  OP_COMP_LT, // "<" comparison less than
  OP_COMP_LE, // "<=" comparison less equal
  OP_COMP_GT, // ">" comparison greater than
  OP_COMP_GE, // ">=" comparison greater equal

  OP_ASSIGN, // "=" assign
  OP_ARIT_ADD_ASSIGN, // "+=" arithmetic add assign
  OP_ARIT_SUB_ASSIGN, // "-=" arithmetic subtract assign
  OP_ARIT_MUL_ASSIGN, // "*=" arithmetic multiply assign
  OP_ARIT_DIV_ASSIGN, // "/=" arithmetic divide assign
  OP_ARIT_MOD_ASSIGN, // "%=" arithmetic modulo assign
  OP_BIT_AND_ASSIGN, // "&=" bitwise and assign
  OP_BIT_OR_ASSIGN, // "|=" bitwise or assign
  OP_BIT_XOR_ASSIGN, // "^=" bitwise xor assign
  OP_BIT_LSH_ASSIGN, // "<<=" bitwise left shift assign
  OP_BIT_RSH_ASSIGN, // ">>=" bitwise right shift assign

  OP_SUBS, // "[]" subscript
  OP_IND, // "*" indirection
  OP_ADDR, // "&" address of
  OP_MEMBER, // "." member access
  OP_IND_MEMBER, // "*." indirect member access
  OP_NULL_SAFE_MEMBER, // "?." null-safe member access
  OP_RANGE, // ".." generate range
  OP_CALL, // "()" function call
} op_kind_t;

/**
 * \brief Returns a string representation of an operator kind.
 * 
 * \param[in] kind Operator kind.
 * \returns String representation.
*/
const char* op_kind_to_string(op_kind_t kind);

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