#include "vm.h"

#include <string.h>
#include <stdarg.h>

#include "opcode.h"
#include "register.h"

#include "util.h"
#include "memtrace.h"

#define VM_DEFAULT_STACK_SIZE (4 * (1 << 10))

void vm_init(vm_t* vm, const uint8_t* code, size_t size)
{
  memset(&vm->regs, 0, sizeof(vm->regs));

  memset(&vm->flags, 0, sizeof(vm->flags));

  vm->code.size = size;
  vm->code.data = code;

  vm->stack.size = VM_DEFAULT_STACK_SIZE;
  vm->stack.data = malloc(VM_DEFAULT_STACK_SIZE);
  assert(vm->stack.data != NULL);
  vm->stack.offset = 0;
}

void vm_free(vm_t* vm)
{
  free(vm->stack.data);
}

uint8_t vm_register_8bit_get(vm_t* vm, register_t reg)
{
  switch (reg)
  {
  case REG_AHB: return (uint8_t)((vm->regs.A >> 8) & 0xFF);
  case REG_ALB: return (uint8_t)(vm->regs.A & 0xFF);
  case REG_BHB: return (uint8_t)((vm->regs.B >> 8) & 0xFF);
  case REG_BLB: return (uint8_t)(vm->regs.B & 0xFF);
  case REG_CHB: return (uint8_t)((vm->regs.C >> 8) & 0xFF);
  case REG_CLB: return (uint8_t)(vm->regs.C & 0xFF);
  case REG_DHB: return (uint8_t)((vm->regs.D >> 8) & 0xFF);
  case REG_DLB: return (uint8_t)(vm->regs.D & 0xFF);
  case REG_EHB: return (uint8_t)((vm->regs.E >> 8) & 0xFF);
  case REG_ELB: return (uint8_t)(vm->regs.E & 0xFF);
  case REG_FHB: return (uint8_t)((vm->regs.F >> 8) & 0xFF);
  case REG_FLB: return (uint8_t)(vm->regs.F & 0xFF);
  default: unreachable();
  }

  return 0;
}

uint16_t vm_register_16bit_get(vm_t* vm, register_t reg)
{
  switch (reg)
  {
  case REG_AHW: return (uint16_t)((vm->regs.A >> 16) & 0xFFFF);
  case REG_ALW: return (uint16_t)(vm->regs.A & 0xFFFF);
  case REG_BHW: return (uint16_t)((vm->regs.B >> 16) & 0xFFFF);
  case REG_BLW: return (uint16_t)(vm->regs.B & 0xFFFF);
  case REG_CHW: return (uint16_t)((vm->regs.C >> 16) & 0xFFFF);
  case REG_CLW: return (uint16_t)(vm->regs.C & 0xFFFF);
  case REG_DHW: return (uint16_t)((vm->regs.D >> 16) & 0xFFFF);
  case REG_DLW: return (uint16_t)(vm->regs.D & 0xFFFF);
  case REG_EHW: return (uint16_t)((vm->regs.E >> 16) & 0xFFFF);
  case REG_ELW: return (uint16_t)(vm->regs.E & 0xFFFF);
  case REG_FHW: return (uint16_t)((vm->regs.F >> 16) & 0xFFFF);
  case REG_FLW: return (uint16_t)(vm->regs.F & 0xFFFF);
  default: unreachable();
  }

  return 0;
}

uint32_t vm_register_32bit_get(vm_t* vm, register_t reg)
{
  switch (reg)
  {
  case REG_AHD: return (uint32_t)((vm->regs.A >> 32) & 0xFFFFFFFF);
  case REG_ALD: return (uint32_t)(vm->regs.A & 0xFFFFFFFF);
  case REG_BHD: return (uint32_t)((vm->regs.B >> 32) & 0xFFFFFFFF);
  case REG_BLD: return (uint32_t)(vm->regs.B & 0xFFFFFFFF);
  case REG_CHD: return (uint32_t)((vm->regs.C >> 32) & 0xFFFFFFFF);
  case REG_CLD: return (uint32_t)(vm->regs.C & 0xFFFFFFFF);
  case REG_DHD: return (uint32_t)((vm->regs.D >> 32) & 0xFFFFFFFF);
  case REG_DLD: return (uint32_t)(vm->regs.D & 0xFFFFFFFF);
  case REG_EHD: return (uint32_t)((vm->regs.E >> 32) & 0xFFFFFFFF);
  case REG_ELD: return (uint32_t)(vm->regs.E & 0xFFFFFFFF);
  case REG_FHD: return (uint32_t)((vm->regs.F >> 32) & 0xFFFFFFFF);
  case REG_FLD: return (uint32_t)(vm->regs.F & 0xFFFFFFFF);
  default: unreachable();
  }

  return 0;
}

uint64_t vm_register_64bit_get(vm_t* vm, register_t reg)
{
  switch (reg)
  {
  case REG_A:  return vm->regs.A;
  case REG_B:  return vm->regs.B;
  case REG_C:  return vm->regs.C;
  case REG_D:  return vm->regs.D;
  case REG_E:  return vm->regs.E;
  case REG_F:  return vm->regs.F;
  case REG_SP: return vm->regs.SP;
  case REG_BP: return vm->regs.BP;
  case REG_IP: return vm->regs.IP;
  default: unreachable();
  }

  return 0;
}

void vm_register_get(vm_t* vm, register_t reg, void* data)
{
  if (register_is_8bit(reg))
    *((uint8_t*)data) = vm_register_8bit_get(vm, reg);
  else if (register_is_16bit(reg))
    *((uint16_t*)data) = vm_register_16bit_get(vm, reg);
  else if (register_is_32bit(reg))
    *((uint32_t*)data) = vm_register_32bit_get(vm, reg);
  else if (register_is_64bit(reg))
    *((uint64_t*)data) = vm_register_64bit_get(vm, reg);
  else
    unreachable();
}

void vm_register_8bit_set(vm_t* vm, register_t reg, uint8_t value)
{
  switch (reg)
  {
  case REG_AHB: vm->regs.A = (vm->regs.A & 0xFFFFFFFFFFFF00FFULL) | (((uint64_t)value) << 8); break;
  case REG_ALB: vm->regs.A = (vm->regs.A & 0xFFFFFFFFFFFFFF00ULL) | value; break;
  case REG_BHB: vm->regs.B = (vm->regs.B & 0xFFFFFFFFFFFF00FFULL) | (((uint64_t)value) << 8); break;
  case REG_BLB: vm->regs.B = (vm->regs.B & 0xFFFFFFFFFFFFFF00ULL) | value; break;
  case REG_CHB: vm->regs.C = (vm->regs.C & 0xFFFFFFFFFFFF00FFULL) | (((uint64_t)value) << 8); break;
  case REG_CLB: vm->regs.C = (vm->regs.C & 0xFFFFFFFFFFFFFF00ULL) | value; break;
  case REG_DHB: vm->regs.D = (vm->regs.D & 0xFFFFFFFFFFFF00FFULL) | (((uint64_t)value) << 8); break;
  case REG_DLB: vm->regs.D = (vm->regs.D & 0xFFFFFFFFFFFFFF00ULL) | value; break;
  case REG_EHB: vm->regs.E = (vm->regs.E & 0xFFFFFFFFFFFF00FFULL) | (((uint64_t)value) << 8); break;
  case REG_ELB: vm->regs.E = (vm->regs.E & 0xFFFFFFFFFFFFFF00ULL) | value; break;
  case REG_FHB: vm->regs.F = (vm->regs.F & 0xFFFFFFFFFFFF00FFULL) | (((uint64_t)value) << 8); break;
  case REG_FLB: vm->regs.F = (vm->regs.F & 0xFFFFFFFFFFFFFF00ULL) | value; break;
  default: unreachable();
  }
}

void vm_register_16bit_set(vm_t* vm, register_t reg, uint16_t value)
{
  switch (reg)
  {
  case REG_AHW: vm->regs.A = (vm->regs.A & 0xFFFFFFFF0000FFFFULL) | (((uint64_t)value) << 16); break;
  case REG_ALW: vm->regs.A = (vm->regs.A & 0xFFFFFFFFFFFF0000ULL) | value; break;
  case REG_BHW: vm->regs.B = (vm->regs.B & 0xFFFFFFFF0000FFFFULL) | (((uint64_t)value) << 16); break;
  case REG_BLW: vm->regs.B = (vm->regs.B & 0xFFFFFFFFFFFF0000ULL) | value; break;
  case REG_CHW: vm->regs.C = (vm->regs.C & 0xFFFFFFFF0000FFFFULL) | (((uint64_t)value) << 16); break;
  case REG_CLW: vm->regs.C = (vm->regs.C & 0xFFFFFFFFFFFF0000ULL) | value; break;
  case REG_DHW: vm->regs.D = (vm->regs.D & 0xFFFFFFFF0000FFFFULL) | (((uint64_t)value) << 16); break;
  case REG_DLW: vm->regs.D = (vm->regs.D & 0xFFFFFFFFFFFF0000ULL) | value; break;
  case REG_EHW: vm->regs.E = (vm->regs.E & 0xFFFFFFFF0000FFFFULL) | (((uint64_t)value) << 16); break;
  case REG_ELW: vm->regs.E = (vm->regs.E & 0xFFFFFFFFFFFF0000ULL) | value; break;
  case REG_FHW: vm->regs.F = (vm->regs.F & 0xFFFFFFFF0000FFFFULL) | (((uint64_t)value) << 16); break;
  case REG_FLW: vm->regs.F = (vm->regs.F & 0xFFFFFFFFFFFF0000ULL) | value; break;
  default: unreachable();
  }
}

void vm_register_32bit_set(vm_t* vm, register_t reg, uint32_t value)
{
  switch (reg)
  {
  case REG_AHD: vm->regs.A = (vm->regs.A & 0x00000000FFFFFFFFULL) | (((uint64_t)value) << 32); break;
  case REG_ALD: vm->regs.A = (vm->regs.A & 0xFFFFFFFF00000000ULL) | value; break;
  case REG_BHD: vm->regs.B = (vm->regs.B & 0x00000000FFFFFFFFULL) | (((uint64_t)value) << 32); break;
  case REG_BLD: vm->regs.B = (vm->regs.B & 0xFFFFFFFF00000000ULL) | value; break;
  case REG_CHD: vm->regs.C = (vm->regs.C & 0x00000000FFFFFFFFULL) | (((uint64_t)value) << 32); break;
  case REG_CLD: vm->regs.C = (vm->regs.C & 0xFFFFFFFF00000000ULL) | value; break;
  case REG_DHD: vm->regs.D = (vm->regs.D & 0x00000000FFFFFFFFULL) | (((uint64_t)value) << 32); break;
  case REG_DLD: vm->regs.D = (vm->regs.D & 0xFFFFFFFF00000000ULL) | value; break;
  case REG_EHD: vm->regs.E = (vm->regs.E & 0x00000000FFFFFFFFULL) | (((uint64_t)value) << 32); break;
  case REG_ELD: vm->regs.E = (vm->regs.E & 0xFFFFFFFF00000000ULL) | value; break;
  case REG_FHD: vm->regs.F = (vm->regs.F & 0x00000000FFFFFFFFULL) | (((uint64_t)value) << 32); break;
  case REG_FLD: vm->regs.F = (vm->regs.F & 0xFFFFFFFF00000000ULL) | value; break;
  default: unreachable();
  }
}

void vm_register_64bit_set(vm_t* vm, register_t reg, uint64_t value)
{
  switch (reg)
  {
  case REG_A:  vm->regs.A  = value; break;
  case REG_B:  vm->regs.B  = value; break;
  case REG_C:  vm->regs.C  = value; break;
  case REG_D:  vm->regs.D  = value; break;
  case REG_E:  vm->regs.E  = value; break;
  case REG_F:  vm->regs.F  = value; break;
  case REG_SP: vm->regs.SP = value; break;
  case REG_BP: vm->regs.BP = value; break;
  case REG_IP: vm->regs.IP = value; break;
  default: unreachable();
  }
}

void vm_register_set(vm_t* vm, register_t reg, const void* data)
{
  if (register_is_8bit(reg))
    vm_register_8bit_set(vm, reg, *(const uint8_t*)data);
  else if (register_is_16bit(reg))
    vm_register_16bit_set(vm, reg, *(const uint16_t*)data);
  else if (register_is_32bit(reg))
    vm_register_32bit_set(vm, reg, *(const uint32_t*)data);
  else if (register_is_64bit(reg))
    vm_register_64bit_set(vm, reg, *(const uint64_t*)data);
  else
    unreachable();
}

uint8_t vm_code_next_8bit(vm_t* vm)
{
  uint8_t value = vm_mem_8bit_get(vm->code.data + vm->regs.IP);
  vm->regs.IP += 1;
  return value;
}

uint16_t vm_code_next_16bit(vm_t* vm)
{
  uint16_t value = vm_mem_16bit_get(vm->code.data + vm->regs.IP);
  vm->regs.IP += 2;
  return value;
}

uint32_t vm_code_next_32bit(vm_t* vm)
{
  uint32_t value = vm_mem_32bit_get(vm->code.data + vm->regs.IP);
  vm->regs.IP += 4;
  return value;
}

uint64_t vm_code_next_64bit(vm_t* vm)
{
  uint64_t value = vm_mem_64bit_get(vm->code.data + vm->regs.IP);
  vm->regs.IP += 8;
  return value;
}

uint8_t vm_mem_8bit_get(const void* mem)
{
  return *(uint8_t*)mem;
}

uint16_t vm_mem_16bit_get(const void* mem)
{
  uint8_t low = ((const uint8_t*)mem)[0];
  uint8_t high = ((const uint8_t*)mem)[1];
  return ((uint16_t)high << 8) |
         ((uint16_t)low);
}

uint32_t vm_mem_32bit_get(const void* mem)
{
  uint8_t byte1 = ((const uint8_t*)mem)[0];
  uint8_t byte2 = ((const uint8_t*)mem)[1];
  uint8_t byte3 = ((const uint8_t*)mem)[2];
  uint8_t byte4 = ((const uint8_t*)mem)[3];
  return (((uint32_t)byte4) << 24) |
         (((uint32_t)byte3) << 16) |
         (((uint32_t)byte2) << 8) |
         (((uint32_t)byte1));
}

uint64_t vm_mem_64bit_get(const void* mem)
{
  uint8_t byte1 = ((const uint8_t*)mem)[0];
  uint8_t byte2 = ((const uint8_t*)mem)[1];
  uint8_t byte3 = ((const uint8_t*)mem)[2];
  uint8_t byte4 = ((const uint8_t*)mem)[3];
  uint8_t byte5 = ((const uint8_t*)mem)[4];
  uint8_t byte6 = ((const uint8_t*)mem)[5];
  uint8_t byte7 = ((const uint8_t*)mem)[6];
  uint8_t byte8 = ((const uint8_t*)mem)[7];
  return (((uint64_t)byte8) << 56) |
         (((uint64_t)byte7) << 48) |
         (((uint64_t)byte6) << 40) |
         (((uint64_t)byte5) << 32) |
         (((uint64_t)byte4) << 24) |
         (((uint64_t)byte3) << 16) |
         (((uint64_t)byte2) << 8) |
         (((uint64_t)byte1));
}

void vm_mem_8bit_set(void* mem, uint8_t value)
{
  *(uint8_t*)mem = value;
}

void vm_mem_16bit_set(void* mem, uint16_t value)
{
  ((uint8_t*)mem)[0] = (uint8_t)(value & 0xFF);
  ((uint8_t*)mem)[1] = (uint8_t)((value >> 8) & 0xFF);
}

void vm_mem_32bit_set(void* mem, uint32_t value)
{
  ((uint8_t*)mem)[0] = (uint8_t)(value & 0xFF);
  ((uint8_t*)mem)[1] = (uint8_t)((value >> 8) & 0xFF);
  ((uint8_t*)mem)[2] = (uint8_t)((value >> 16) & 0xFF);
  ((uint8_t*)mem)[3] = (uint8_t)((value >> 24) & 0xFF);
}

void vm_mem_64bit_set(void* mem, uint64_t value)
{
  ((uint8_t*)mem)[0] = (uint8_t)(value & 0xFF);
  ((uint8_t*)mem)[1] = (uint8_t)((value >> 8) & 0xFF);
  ((uint8_t*)mem)[2] = (uint8_t)((value >> 16) & 0xFF);
  ((uint8_t*)mem)[3] = (uint8_t)((value >> 24) & 0xFF);
  ((uint8_t*)mem)[4] = (uint8_t)((value >> 32) & 0xFF);
  ((uint8_t*)mem)[5] = (uint8_t)((value >> 40) & 0xFF);
  ((uint8_t*)mem)[6] = (uint8_t)((value >> 48) & 0xFF);
  ((uint8_t*)mem)[7] = (uint8_t)((value >> 56) & 0xFF);
}

void vm_fetch(vm_t* vm)
{
  uint8_t low = vm->code.data[vm->regs.IP];
  uint8_t high = vm->code.data[vm->regs.IP + 1];

  vm->inst.raw = (((uint16_t)high) << 8) | ((uint16_t)low);
  vm->regs.IP += 2;
}

void vm_decode(vm_t* vm)
{
  vm->inst.opcode = (opcode_t)((vm->inst.raw >> 6) & 0x3FF);
  vm->inst.param = (opcode_param_t)((vm->inst.raw >> 3) & 0x7);
  vm->inst.width = (opcode_width_t)((vm->inst.raw >> 1) & 0x3);
}

static inline void vm_execute_MOV(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_PSH(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_POP(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_LEA(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_IADD(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_ISUB(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_IMUL(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_IDIV(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_IMOD(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_INEG(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_IINC(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_IDEC(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_FADD(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_FSUB(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_FMUL(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_FDIV(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_FMOD(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_FNEG(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_AND(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_OR(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_XOR(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_NOT(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_SHL(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_SHR(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_ROL(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_ROR(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JMP(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JE(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JNE(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JL(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JLE(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JG(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JGE(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JZ(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_JNZ(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_CMP(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_CLF(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_CALL(vm_t* vm)
{
  unused(vm);
}

static inline void vm_execute_RET(vm_t* vm)
{
  unused(vm);
}

void vm_execute(vm_t* vm)
{
  switch (vm->inst.opcode)
  {
  case OPCODE_NOP:  break;
  case OPCODE_MOV:  vm_execute_MOV(vm); break;
  case OPCODE_PSH:  vm_execute_PSH(vm); break;
  case OPCODE_POP:  vm_execute_POP(vm); break;
  case OPCODE_LEA:  vm_execute_LEA(vm); break;
  case OPCODE_IADD: vm_execute_IADD(vm); break;
  case OPCODE_ISUB: vm_execute_ISUB(vm); break;
  case OPCODE_IMUL: vm_execute_IMUL(vm); break;
  case OPCODE_IDIV: vm_execute_IDIV(vm); break;
  case OPCODE_IMOD: vm_execute_IMOD(vm); break;
  case OPCODE_INEG: vm_execute_INEG(vm); break;
  case OPCODE_IINC: vm_execute_IINC(vm); break;
  case OPCODE_IDEC: vm_execute_IDEC(vm); break;
  case OPCODE_FADD: vm_execute_FADD(vm); break;
  case OPCODE_FSUB: vm_execute_FSUB(vm); break;
  case OPCODE_FMUL: vm_execute_FMUL(vm); break;
  case OPCODE_FDIV: vm_execute_FDIV(vm); break;
  case OPCODE_FMOD: vm_execute_FMOD(vm); break;
  case OPCODE_FNEG: vm_execute_FNEG(vm); break;
  case OPCODE_AND:  vm_execute_AND(vm); break;
  case OPCODE_OR:   vm_execute_OR(vm); break;
  case OPCODE_XOR:  vm_execute_XOR(vm); break;
  case OPCODE_NOT:  vm_execute_NOT(vm); break;
  case OPCODE_SHL:  vm_execute_SHL(vm); break;
  case OPCODE_SHR:  vm_execute_SHR(vm); break;
  case OPCODE_ROL:  vm_execute_ROL(vm); break;
  case OPCODE_ROR:  vm_execute_ROR(vm); break;
  case OPCODE_JMP:  vm_execute_JMP(vm); break;
  case OPCODE_JE:   vm_execute_JE(vm); break;
  case OPCODE_JNE:  vm_execute_JNE(vm); break;
  case OPCODE_JL:   vm_execute_JL(vm); break;
  case OPCODE_JLE:  vm_execute_JLE(vm); break;
  case OPCODE_JG:   vm_execute_JG(vm); break;
  case OPCODE_JGE:  vm_execute_JGE(vm); break;
  case OPCODE_JZ:   vm_execute_JZ(vm); break;
  case OPCODE_JNZ:  vm_execute_JNZ(vm); break;
  case OPCODE_CMP:  vm_execute_CMP(vm); break;
  case OPCODE_CLF:  vm_execute_CLF(vm); break;
  case OPCODE_CALL: vm_execute_CALL(vm); break;
  case OPCODE_RET:  vm_execute_RET(vm); break;
  case OPCODE_HLT:  break;
  default: unreachable();
  }
}

void vm_run(vm_t* vm)
{
  while (vm->inst.opcode != OPCODE_HLT)
  {
    vm_fetch(vm);
    vm_decode(vm);
    vm_execute(vm);
  }
}
