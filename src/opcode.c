#include "opcode.h"

#include <string.h>

#include "util.h"

uint16_t opcode_encode(opcode_t opcode, opcode_param_t param, opcode_width_t width)
{
  uint16_t param_value;

  // Value of `param` needs to be remapped to fit into 3 bits.
  // When decoding, `opcode` is used to exactly determine the parameter,
  // since an operation has either 0, 1 or 2 parameters.
  switch (param)
  {
  case OPCODE_PARAM_NONE:    param_value = 0; break;
  case OPCODE_PARAM_REG:     param_value = 0; break;
  case OPCODE_PARAM_MEM:     param_value = 1; break;
  case OPCODE_PARAM_IMM:     param_value = 2; break;
  case OPCODE_PARAM_LABEL:   param_value = 3; break;
  case OPCODE_PARAM_REG_REG: param_value = 0; break;
  case OPCODE_PARAM_MEM_REG: param_value = 1; break;
  case OPCODE_PARAM_REG_MEM: param_value = 2; break;
  case OPCODE_PARAM_MEM_MEM: param_value = 3; break;
  case OPCODE_PARAM_REG_IMM: param_value = 4; break;
  case OPCODE_PARAM_MEM_IMM: param_value = 5; break;
  default: unreachable();
  }

  uint16_t width_value;

  switch (width)
  {
  case OPCODE_WIDTH_NONE:  width_value = 0; break;
  case OPCODE_WIDTH_8BIT:  width_value = 0; break;
  case OPCODE_WIDTH_16BIT: width_value = 1; break;
  case OPCODE_WIDTH_32BIT: width_value = 2; break;
  case OPCODE_WIDTH_64BIT: width_value = 3; break;
  default: unreachable();
  }

  return (((uint16_t)opcode & 0x3FF) << 6) |
         ((param_value & 0x7) << 3) |
         ((width_value & 0x3) << 1);
}

void opcode_decode(uint16_t value, opcode_t* opcode, opcode_param_t* param, opcode_width_t* width)
{
  opcode_t opcode_value = (opcode_t)((value >> 6) & 0x3FF);

  if (opcode != NULL)
    *opcode = opcode_value;
  
  opcode_param_t param_value = (opcode_param_t)((value >> 3) & 0x7);

  switch (opcode_param_count(opcode_value))
  {
  case 0:
    param_value = OPCODE_PARAM_NONE;
    break;
  case 1:
    switch (param_value)
    {
    case 0: param_value = OPCODE_PARAM_REG; break;
    case 1: param_value = OPCODE_PARAM_MEM; break;
    case 2: param_value = OPCODE_PARAM_IMM; break;
    case 3: param_value = OPCODE_PARAM_LABEL; break;
    default: unreachable();
    }
    break;
  case 2:
    switch (param_value)
    {
    case 0: param_value = OPCODE_PARAM_REG_REG; break;
    case 1: param_value = OPCODE_PARAM_MEM_REG; break;
    case 2: param_value = OPCODE_PARAM_REG_MEM; break;
    case 3: param_value = OPCODE_PARAM_MEM_MEM; break;
    case 4: param_value = OPCODE_PARAM_REG_IMM; break;
    case 5: param_value = OPCODE_PARAM_MEM_IMM; break;
    default: unreachable();
    }
    break;
  default:
    unreachable();
  }
  
  if (param != NULL)
    *param = param_value;
  
  opcode_width_t width_value = (opcode_width_t)((value >> 1) & 0x3);

  switch (width_value)
  {
  case 0: width_value = opcode_param_count(opcode_value) == 0 ? OPCODE_WIDTH_NONE : OPCODE_WIDTH_8BIT; break;
  case 1: width_value = OPCODE_WIDTH_16BIT; break;
  case 2: width_value = OPCODE_WIDTH_32BIT; break;
  case 3: width_value = OPCODE_WIDTH_64BIT; break;
  default: unreachable();
  }

  if (width != NULL)
    *width = width_value;
}

const char* opcode_to_string(opcode_t opcode)
{
  switch (opcode)
  {
  case OPCODE_NOP:  return "NOP";
  case OPCODE_MOV:  return "MOV";
  case OPCODE_PSH:  return "PSH";
  case OPCODE_POP:  return "POP";
  case OPCODE_LEA:  return "LEA";
  case OPCODE_ADD:  return "ADD";
  case OPCODE_SUB:  return "SUB";
  case OPCODE_MUL:  return "MUL";
  case OPCODE_DIV:  return "DIV";
  case OPCODE_MOD:  return "MOD";
  case OPCODE_INC:  return "INC";
  case OPCODE_IADD: return "IADD";
  case OPCODE_ISUB: return "ISUB";
  case OPCODE_IMUL: return "IMUL";
  case OPCODE_IDIV: return "IDIV";
  case OPCODE_IMOD: return "IMOD";
  case OPCODE_INEG: return "INEG";
  case OPCODE_IINC: return "IINC";
  case OPCODE_IDEC: return "IDEC";
  case OPCODE_FADD: return "FADD";
  case OPCODE_FSUB: return "FSUB";
  case OPCODE_FMUL: return "FMUL";
  case OPCODE_FDIV: return "FDIV";
  case OPCODE_FMOD: return "FMOD";
  case OPCODE_FNEG: return "FNEG";
  case OPCODE_AND:  return "AND";
  case OPCODE_OR:   return "OR";
  case OPCODE_XOR:  return "XOR";
  case OPCODE_NOT:  return "NOT";
  case OPCODE_SHL:  return "SHL";
  case OPCODE_SHR:  return "SHR";
  case OPCODE_JMP:  return "JMP";
  case OPCODE_JE:   return "JE";
  case OPCODE_JNE:  return "JNE";
  case OPCODE_JL:   return "JL";
  case OPCODE_JLE:  return "JLE";
  case OPCODE_JG:   return "JG";
  case OPCODE_JGE:  return "JGE";
  case OPCODE_CMP:  return "CMP";
  case OPCODE_CLF:  return "CLF";
  case OPCODE_CALL: return "CALL";
  case OPCODE_RET:  return "RET";
  case OPCODE_HLT:  return "HLT";
  default: unreachable();
  }

  return NULL;
}

size_t opcode_param_count(opcode_t opcode)
{
  switch (opcode)
  {
  case OPCODE_NOP:
  case OPCODE_CLF:
  case OPCODE_RET:
  case OPCODE_HLT:
    return 0;
  case OPCODE_PSH:
  case OPCODE_POP:
  case OPCODE_LEA:
  case OPCODE_INEG:
  case OPCODE_IINC:
  case OPCODE_IDEC:
  case OPCODE_FNEG:
  case OPCODE_NOT:
  case OPCODE_JMP:
  case OPCODE_JE:
  case OPCODE_JNE:
  case OPCODE_JL:
  case OPCODE_JLE:
  case OPCODE_JG:
  case OPCODE_JGE:
  case OPCODE_CALL:
    return 1;
  case OPCODE_MOV:
  case OPCODE_ADD:
  case OPCODE_SUB:
  case OPCODE_MUL:
  case OPCODE_DIV:
  case OPCODE_MOD:
  case OPCODE_IADD:
  case OPCODE_ISUB:
  case OPCODE_IMUL:
  case OPCODE_IDIV:
  case OPCODE_IMOD:
  case OPCODE_FADD:
  case OPCODE_FSUB:
  case OPCODE_FMUL:
  case OPCODE_FDIV:
  case OPCODE_FMOD:
  case OPCODE_AND:
  case OPCODE_OR:
  case OPCODE_XOR:
  case OPCODE_SHL:
  case OPCODE_SHR:
  case OPCODE_CMP:
    return 2;
  default:
    unreachable();
  }

  return -1;
}
