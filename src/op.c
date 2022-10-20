#include "op.h"

const char* op_kind_to_string(op_kind_t kind)
{
  switch (kind)
  {
  case OP_IS: return "OP_IS";
  case OP_AS: return "OP_AS";
  case OP_SIZEOF: return "OP_SIZEOF";
  case OP_ALIGNOF: return "OP_ALIGNOF";
  case OP_TYPEOF: return "OP_TYPEOF";
  case OP_IN: return "OP_IN";
  case OP_ARIT_ADD: return "OP_ARIT_ADD";
  case OP_ARIT_SUB: return "OP_ARIT_SUB";
  case OP_ARIT_MUL: return "OP_ARIT_MUL";
  case OP_ARIT_DIV: return "OP_ARIT_DIV";
  case OP_ARIT_MOD: return "OP_ARIT_MOD";
  case OP_BIT_AND: return "OP_BIT_AND";
  case OP_BIT_OR: return "OP_BIT_OR";
  case OP_BIT_XOR: return "OP_BIT_XOR";
  case OP_BIT_NOT: return "OP_BIT_NOT";
  case OP_BIT_LSH: return "OP_BIT_LSH";
  case OP_BIT_RSH: return "OP_BIT_RSH";
  case OP_LOGIC_AND: return "OP_LOGIC_AND";
  case OP_LOGIC_OR: return "OP_LOGIC_OR";
  case OP_LOGIC_NOT: return "OP_LOGIC_NOT";
  case OP_COMP_EQ: return "OP_COMP_EQ";
  case OP_COMP_NE: return "OP_COMP_NE";
  case OP_COMP_LT: return "OP_COMP_LT";
  case OP_COMP_LE: return "OP_COMP_LE";
  case OP_COMP_GT: return "OP_COMP_GT";
  case OP_COMP_GE: return "OP_COMP_GE";
  case OP_ASSIGN: return "OP_ASSIGN";
  case OP_ARIT_ADD_ASSIGN: return "OP_ARIT_ADD_ASSIGN";
  case OP_ARIT_SUB_ASSIGN: return "OP_ARIT_SUB_ASSIGN";
  case OP_ARIT_MUL_ASSIGN: return "OP_ARIT_MUL_ASSIGN";
  case OP_ARIT_DIV_ASSIGN: return "OP_ARIT_DIV_ASSIGN";
  case OP_ARIT_MOD_ASSIGN: return "OP_ARIT_MOD_ASSIGN";
  case OP_BIT_AND_ASSIGN: return "OP_BIT_AND_ASSIGN";
  case OP_BIT_OR_ASSIGN: return "OP_BIT_OR_ASSIGN";
  case OP_BIT_XOR_ASSIGN: return "OP_BIT_XOR_ASSIGN";
  case OP_BIT_LSH_ASSIGN: return "OP_BIT_LSH_ASSIGN";
  case OP_BIT_RSH_ASSIGN: return "OP_BIT_RSH_ASSIGN";
  case OP_SUBS: return "OP_SUBS";
  case OP_IND: return "OP_IND";
  case OP_ADDR: return "OP_ADDR";
  case OP_MEMBER: return "OP_MEMBER";
  case OP_IND_MEMBER: return "OP_IND_MEMBER";
  case OP_SAFE_IND_MEMBER: return "OP_SAFE_IND_MEMBER";
  case OP_RANGE: return "OP_RANGE";
  case OP_COMMA: return "OP_COMMA";
  default: return "";
  }
}
