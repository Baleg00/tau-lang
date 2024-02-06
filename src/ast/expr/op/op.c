/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "ast/expr/op/op.h"

#include "utils/common.h"

const char* op_kind_to_cstr(op_kind_t kind)
{
  switch (kind)
  {
  case OP_SIZEOF:           return "OP_SIZEOF";
  case OP_ALIGNOF:          return "OP_ALIGNOF";
  case OP_ARIT_INC_PRE:     return "OP_ARIT_INC_PRE";
  case OP_ARIT_INC_POST:    return "OP_ARIT_INC_POST";
  case OP_ARIT_DEC_PRE:     return "OP_ARIT_DEC_PRE";
  case OP_ARIT_DEC_POST:    return "OP_ARIT_DEC_POST";
  case OP_ARIT_POS:         return "OP_ARIT_POS";
  case OP_ARIT_NEG:         return "OP_ARIT_NEG";
  case OP_ARIT_ADD:         return "OP_ARIT_ADD";
  case OP_ARIT_SUB:         return "OP_ARIT_SUB";
  case OP_ARIT_MUL:         return "OP_ARIT_MUL";
  case OP_ARIT_DIV:         return "OP_ARIT_DIV";
  case OP_ARIT_MOD:         return "OP_ARIT_MOD";
  case OP_BIT_AND:          return "OP_BIT_AND";
  case OP_BIT_OR:           return "OP_BIT_OR";
  case OP_BIT_XOR:          return "OP_BIT_XOR";
  case OP_BIT_NOT:          return "OP_BIT_NOT";
  case OP_BIT_LSH:          return "OP_BIT_LSH";
  case OP_BIT_RSH:          return "OP_BIT_RSH";
  case OP_LOGIC_AND:        return "OP_LOGIC_AND";
  case OP_LOGIC_OR:         return "OP_LOGIC_OR";
  case OP_LOGIC_NOT:        return "OP_LOGIC_NOT";
  case OP_CMP_EQ:           return "OP_CMP_EQ";
  case OP_CMP_NE:           return "OP_CMP_NE";
  case OP_CMP_LT:           return "OP_CMP_LT";
  case OP_CMP_LE:           return "OP_CMP_LE";
  case OP_CMP_GT:           return "OP_CMP_GT";
  case OP_CMP_GE:           return "OP_CMP_GE";
  case OP_ASSIGN:           return "OP_ASSIGN";
  case OP_ASSIGN_ARIT_ADD:  return "OP_ASSIGN_ARIT_ADD";
  case OP_ASSIGN_ARIT_SUB:  return "OP_ASSIGN_ARIT_SUB";
  case OP_ASSIGN_ARIT_MUL:  return "OP_ASSIGN_ARIT_MUL";
  case OP_ASSIGN_ARIT_DIV:  return "OP_ASSIGN_ARIT_DIV";
  case OP_ASSIGN_ARIT_MOD:  return "OP_ASSIGN_ARIT_MOD";
  case OP_ASSIGN_BIT_AND:   return "OP_ASSIGN_BIT_AND";
  case OP_ASSIGN_BIT_OR:    return "OP_ASSIGN_BIT_OR";
  case OP_ASSIGN_BIT_XOR:   return "OP_ASSIGN_BIT_XOR";
  case OP_ASSIGN_BIT_LSH:   return "OP_ASSIGN_BIT_LSH";
  case OP_ASSIGN_BIT_RSH:   return "OP_ASSIGN_BIT_RSH";
  case OP_SUBS:             return "OP_SUBS";
  case OP_IND:              return "OP_IND";
  case OP_ADDR:             return "OP_ADDR";
  case OP_ACCESS:           return "OP_ACCESS";
  case OP_ACCESS_IND:       return "OP_ACCESS_IND";
  case OP_ACCESS_OPT:       return "OP_ACCESS_OPT";
  case OP_RANGE:            return "OP_RANGE";
  case OP_CALL:             return "OP_CALL";
  default: unreachable();
  }

  return NULL;
}

int op_precedence(op_kind_t kind)
{
  switch (kind)
  {
  case OP_ACCESS:
  case OP_ACCESS_IND:
  case OP_ACCESS_OPT:
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
  case OP_SUBS:
  case OP_CALL:
    return 0;
  
  case OP_SIZEOF:
  case OP_ALIGNOF:
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
  case OP_LOGIC_NOT:
  case OP_IND:
  case OP_ADDR:
    return 1;
  
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
    return 2;

  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
    return 3;

  case OP_RANGE:
    return 4;

  case OP_BIT_LSH:
  case OP_BIT_RSH:
    return 5;

  case OP_CMP_LT:
  case OP_CMP_LE:
  case OP_CMP_GT:
  case OP_CMP_GE:
    return 6;

  case OP_CMP_EQ:
  case OP_CMP_NE:
    return 7;

  case OP_BIT_AND:
    return 8;

  case OP_BIT_XOR:
    return 9;

  case OP_BIT_OR:
    return 10;

  case OP_LOGIC_AND:
    return 11;

  case OP_LOGIC_OR:
    return 12;

  case OP_ASSIGN:
  case OP_ASSIGN_ARIT_ADD:
  case OP_ASSIGN_ARIT_SUB:
  case OP_ASSIGN_ARIT_MUL:
  case OP_ASSIGN_ARIT_DIV:
  case OP_ASSIGN_ARIT_MOD:
  case OP_ASSIGN_BIT_AND:
  case OP_ASSIGN_BIT_OR:
  case OP_ASSIGN_BIT_XOR:
  case OP_ASSIGN_BIT_LSH:
  case OP_ASSIGN_BIT_RSH:
    return 13;

  default:
    unreachable();
  }

  return -1;
}

bool op_is_binary(op_kind_t kind)
{
  switch (kind)
  {
  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
  case OP_BIT_AND:
  case OP_BIT_OR:
  case OP_BIT_XOR:
  case OP_BIT_LSH:
  case OP_BIT_RSH:
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
  case OP_CMP_EQ:
  case OP_CMP_NE:
  case OP_CMP_LT:
  case OP_CMP_LE:
  case OP_CMP_GT:
  case OP_CMP_GE:
  case OP_ASSIGN:
  case OP_ASSIGN_ARIT_ADD:
  case OP_ASSIGN_ARIT_SUB:
  case OP_ASSIGN_ARIT_MUL:
  case OP_ASSIGN_ARIT_DIV:
  case OP_ASSIGN_ARIT_MOD:
  case OP_ASSIGN_BIT_AND:
  case OP_ASSIGN_BIT_OR:
  case OP_ASSIGN_BIT_XOR:
  case OP_ASSIGN_BIT_LSH:
  case OP_ASSIGN_BIT_RSH:
  case OP_SUBS:
  case OP_ACCESS:
  case OP_ACCESS_IND:
  case OP_ACCESS_OPT:
  case OP_RANGE:
    return true;
  default:
    return false;
  }
}

bool op_is_unary(op_kind_t kind)
{
  switch (kind)
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
  case OP_ARIT_INC_PRE:
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_PRE:
  case OP_ARIT_DEC_POST:
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_LOGIC_NOT:
  case OP_BIT_NOT:
  case OP_IND:
  case OP_ADDR:
    return true;
  default:
    return false;
  }
}

bool op_is_arithmetic(op_kind_t kind)
{
  switch (kind)
  {
  case OP_ARIT_INC_PRE:
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_PRE:
  case OP_ARIT_DEC_POST:
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
  case OP_ASSIGN_ARIT_ADD:
  case OP_ASSIGN_ARIT_SUB:
  case OP_ASSIGN_ARIT_MUL:
  case OP_ASSIGN_ARIT_DIV:
  case OP_ASSIGN_ARIT_MOD:
    return true;
  default:
    return false;
  }
}

bool op_is_bitwise(op_kind_t kind)
{
  switch (kind)
  {
  case OP_BIT_AND:
  case OP_BIT_OR:
  case OP_BIT_XOR:
  case OP_BIT_NOT:
  case OP_BIT_LSH:
  case OP_BIT_RSH:
    return true;
  default:
    return false;
  }
}

bool op_is_logical(op_kind_t kind)
{
  switch (kind)
  {
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
  case OP_LOGIC_NOT:
    return true;
  default:
    return false;
  }
}

bool op_is_comparison(op_kind_t kind)
{
  switch (kind)
  {
  case OP_CMP_EQ:
  case OP_CMP_NE:
  case OP_CMP_LT:
  case OP_CMP_LE:
  case OP_CMP_GT:
  case OP_CMP_GE:
    return true;
  default:
    return false;
  }
}

bool op_is_assignment(op_kind_t kind)
{
  switch (kind)
  {
  case OP_ASSIGN:
  case OP_ASSIGN_ARIT_ADD:
  case OP_ASSIGN_ARIT_SUB:
  case OP_ASSIGN_ARIT_MUL:
  case OP_ASSIGN_ARIT_DIV:
  case OP_ASSIGN_ARIT_MOD:
  case OP_ASSIGN_BIT_AND:
  case OP_ASSIGN_BIT_OR:
  case OP_ASSIGN_BIT_XOR:
  case OP_ASSIGN_BIT_LSH:
  case OP_ASSIGN_BIT_RSH:
    return true;
  default:
    return false;
  }
}

bool op_is_left_assoc(op_kind_t kind)
{
  switch (kind)
  {
  case OP_ARIT_INC_POST:
  case OP_ARIT_DEC_POST:
  case OP_ARIT_ADD:
  case OP_ARIT_SUB:
  case OP_ARIT_MUL:
  case OP_ARIT_DIV:
  case OP_ARIT_MOD:
  case OP_BIT_AND:
  case OP_BIT_OR:
  case OP_BIT_XOR:
  case OP_BIT_LSH:
  case OP_BIT_RSH:
  case OP_LOGIC_AND:
  case OP_LOGIC_OR:
  case OP_CMP_EQ:
  case OP_CMP_NE:
  case OP_CMP_LT:
  case OP_CMP_LE:
  case OP_CMP_GT:
  case OP_CMP_GE:
  case OP_SUBS:
  case OP_ACCESS:
  case OP_ACCESS_IND:
  case OP_ACCESS_OPT:
  case OP_RANGE:
  case OP_CALL:
    return true;
  default:
    return false;
  }
}

bool op_is_right_assoc(op_kind_t kind)
{
  switch (kind)
  {
  case OP_SIZEOF:
  case OP_ALIGNOF:
  case OP_ARIT_INC_PRE:
  case OP_ARIT_DEC_PRE:
  case OP_ARIT_POS:
  case OP_ARIT_NEG:
  case OP_BIT_NOT:
  case OP_LOGIC_NOT:
  case OP_ASSIGN:
  case OP_ASSIGN_ARIT_ADD:
  case OP_ASSIGN_ARIT_SUB:
  case OP_ASSIGN_ARIT_MUL:
  case OP_ASSIGN_ARIT_DIV:
  case OP_ASSIGN_ARIT_MOD:
  case OP_ASSIGN_BIT_AND:
  case OP_ASSIGN_BIT_OR:
  case OP_ASSIGN_BIT_XOR:
  case OP_ASSIGN_BIT_LSH:
  case OP_ASSIGN_BIT_RSH:
  case OP_IND:
  case OP_ADDR:
    return true;
  default:
    return false;
  }
}
