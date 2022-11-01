#ifndef TAU_OP_H
#define TAU_OP_H

typedef enum op_kind_e
{
  OP_IS, // "is" operation type check
  OP_AS, // "as" operation type cast
  OP_SIZEOF, // "sizeof" operation size of expression type
  OP_ALIGNOF, // "alignof" operation align of expression type
  OP_TYPEOF, // "typeof" operation type of expression
  OP_IN, // "in" operation in range
  
  OP_ARIT_INC_PRE, // "++" operation arithmetic pre increment
  OP_ARIT_INC_POST, // "++" operation arithmetic post increment
  OP_ARIT_DEC_PRE, // "--" operation arithmetic pre decrement
  OP_ARIT_DEC_POST, // "--" operation arithmetic post decrement
  OP_ARIT_POS, // "+" operation arithmetic positive
  OP_ARIT_NEG, // "-" operation arithmetic negative
  OP_ARIT_ADD, // "+" operation arithmetic add
  OP_ARIT_SUB, // "-" operation arithmetic subtract
  OP_ARIT_MUL, // "*" operation arithmetic multiply
  OP_ARIT_DIV, // "/" operation arithmetic divide
  OP_ARIT_MOD, // "%" operation arithmetic modulo
  
  OP_BIT_AND, // "&" operation bitwise and
  OP_BIT_OR, // "|" operation bitwise or
  OP_BIT_XOR, // "^" operation bitwise xor
  OP_BIT_NOT, // "~" operation bitwise not
  OP_BIT_LSH, // "<<" operation bitwise left shift
  OP_BIT_RSH, // ">>" operation bitwise right shift

  OP_LOGIC_AND, // "&&" operation logical and
  OP_LOGIC_OR, // "||" operation logical or
  OP_LOGIC_NOT, // "!" operation logical not

  OP_COMP_EQ, // "==" operation comparison equal
  OP_COMP_NE, // "!=" operation comparison not equal
  OP_COMP_LT, // "<" operation comparison less than
  OP_COMP_LE, // "<=" operation comparison less equal
  OP_COMP_GT, // ">" operation comparison greater than
  OP_COMP_GE, // ">=" operation comparison greater equal

  OP_ASSIGN, // "=" operation assign
  OP_ARIT_ADD_ASSIGN, // "+=" operation arithmetic add assign
  OP_ARIT_SUB_ASSIGN, // "-=" operation arithmetic subtract assign
  OP_ARIT_MUL_ASSIGN, // "*=" operation arithmetic multiply assign
  OP_ARIT_DIV_ASSIGN, // "/=" operation arithmetic divide assign
  OP_ARIT_MOD_ASSIGN, // "%=" operation arithmetic modulo assign
  OP_BIT_AND_ASSIGN, // "&=" operation bitwise and assign
  OP_BIT_OR_ASSIGN, // "|=" operation bitwise or assign
  OP_BIT_XOR_ASSIGN, // "^=" operation bitwise xor assign
  OP_BIT_LSH_ASSIGN, // "<<=" operation bitwise left shift assign
  OP_BIT_RSH_ASSIGN, // ">>=" operation bitwise right shift assign

  OP_SUBS, // "[]" operation subscript
  OP_IND, // "*" operation indirection
  OP_ADDR, // "&" operation address of
  OP_MEMBER, // "." operation member access
  OP_IND_MEMBER, // "*." operation indirect member access
  OP_SAFE_IND_MEMBER, // "?." operation safe indirect member access
  OP_RANGE, // ".." operation generate range
  OP_COMMA, // "," operation comma
  OP_CALL, // "()" operation function call
} op_kind_t;

const char* op_kind_to_string(op_kind_t kind);
int op_precedence(op_kind_t kind);

#endif