#include "opcode.h"

#include "util.h"

const char* opcode_to_string(opcode_t opcode)
{
  switch (opcode)
  {
  case OPCODE_NOP:  return "NOP";
  case OPCODE_MOV:  return "MOV";
  case OPCODE_PSH:  return "PSH";
  case OPCODE_POP:  return "POP";
  case OPCODE_LEA:  return "LEA";
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
  case OPCODE_ROL:  return "ROL";
  case OPCODE_ROR:  return "ROR";
  case OPCODE_JMP:  return "JMP";
  case OPCODE_JE:   return "JE";
  case OPCODE_JNE:  return "JNE";
  case OPCODE_JL:   return "JL";
  case OPCODE_JLE:  return "JLE";
  case OPCODE_JG:   return "JG";
  case OPCODE_JGE:  return "JGE";
  case OPCODE_JZ:   return "JZ";
  case OPCODE_JNZ:  return "JNZ";
  case OPCODE_CMP:  return "CMP";
  case OPCODE_CLF:  return "CLF";
  case OPCODE_CALL: return "CALL";
  case OPCODE_RET:  return "RET";
  case OPCODE_HLT:  return "HLT";
  default: unreachable();
  }

  return NULL;
}
