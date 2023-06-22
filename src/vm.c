#include "vm.h"

#include <string.h>
#include <stdarg.h>
#include <limits.h>

#include "opcode.h"
#include "register.h"
#include "tasm.h"

#include "util.h"
#include "memtrace.h"

#define VM_STACK_DEFAULT_SIZE ((size_t)(8 * (1 << 10)))

void vm_init(vm_t* vm, const uint8_t* code, size_t size)
{
  memset(&vm->regs, 0, sizeof(vm->regs));

  vm->code.size = size;
  vm->code.data = code;

  vm->stack.size = VM_STACK_DEFAULT_SIZE;
  vm->stack.data = malloc(VM_STACK_DEFAULT_SIZE);
  assert(vm->stack.data != NULL);

  vm->regs.SP = (uint64_t)(vm->stack.data + VM_STACK_DEFAULT_SIZE);

  memset(&vm->inst, 0, sizeof(vm->inst));
}

void vm_free(vm_t* vm)
{
  free(vm->stack.data);
}

uint8_t vm_register_u8_get(vm_t* vm, register_t reg)
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

uint16_t vm_register_u16_get(vm_t* vm, register_t reg)
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

uint32_t vm_register_u32_get(vm_t* vm, register_t reg)
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

uint64_t vm_register_u64_get(vm_t* vm, register_t reg)
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

int8_t  vm_register_i8_get (vm_t* vm, register_t reg) { return (int8_t) vm_register_u8_get (vm, reg); }
int16_t vm_register_i16_get(vm_t* vm, register_t reg) { return (int16_t)vm_register_u16_get(vm, reg); }
int32_t vm_register_i32_get(vm_t* vm, register_t reg) { return (int32_t)vm_register_u32_get(vm, reg); }
int64_t vm_register_i64_get(vm_t* vm, register_t reg) { return (int64_t)vm_register_u64_get(vm, reg); }

float vm_register_f32_get(vm_t* vm, register_t reg)
{
  switch (reg)
  {
  case REG_AHD: return ((float*)&vm->regs.A)[1];
  case REG_ALD: return ((float*)&vm->regs.A)[0];
  case REG_BHD: return ((float*)&vm->regs.B)[1];
  case REG_BLD: return ((float*)&vm->regs.B)[0];
  case REG_CHD: return ((float*)&vm->regs.C)[1];
  case REG_CLD: return ((float*)&vm->regs.C)[0];
  case REG_DHD: return ((float*)&vm->regs.D)[1];
  case REG_DLD: return ((float*)&vm->regs.D)[0];
  case REG_EHD: return ((float*)&vm->regs.E)[1];
  case REG_ELD: return ((float*)&vm->regs.E)[0];
  case REG_FHD: return ((float*)&vm->regs.F)[1];
  case REG_FLD: return ((float*)&vm->regs.F)[0];
  default: unreachable();
  }

  return 0.0f;
}

double vm_register_f64_get(vm_t* vm, register_t reg)
{
  switch (reg)
  {
  case REG_A: return *(double*)&vm->regs.A;
  case REG_B: return *(double*)&vm->regs.B;
  case REG_C: return *(double*)&vm->regs.C;
  case REG_D: return *(double*)&vm->regs.D;
  case REG_E: return *(double*)&vm->regs.E;
  case REG_F: return *(double*)&vm->regs.F;
  default: unreachable();
  }

  return 0.0;
}

void vm_register_u8_set(vm_t* vm, register_t reg, uint8_t value)
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

void vm_register_u16_set(vm_t* vm, register_t reg, uint16_t value)
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

void vm_register_u32_set(vm_t* vm, register_t reg, uint32_t value)
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

void vm_register_u64_set(vm_t* vm, register_t reg, uint64_t value)
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

void vm_register_i8_set (vm_t* vm, register_t reg, int8_t  value) { vm_register_u8_set (vm, reg, (uint8_t) value); }
void vm_register_i16_set(vm_t* vm, register_t reg, int16_t value) { vm_register_u16_set(vm, reg, (uint16_t)value); }
void vm_register_i32_set(vm_t* vm, register_t reg, int32_t value) { vm_register_u32_set(vm, reg, (uint32_t)value); }
void vm_register_i64_set(vm_t* vm, register_t reg, int64_t value) { vm_register_u64_set(vm, reg, (uint64_t)value); }

void vm_register_f32_set(vm_t* vm, register_t reg, float value)
{
  switch (reg)
  {
  case REG_AHD: ((float*)&vm->regs.A)[1] = value; break;
  case REG_ALD: ((float*)&vm->regs.A)[0] = value; break;
  case REG_BHD: ((float*)&vm->regs.B)[1] = value; break;
  case REG_BLD: ((float*)&vm->regs.B)[0] = value; break;
  case REG_CHD: ((float*)&vm->regs.C)[1] = value; break;
  case REG_CLD: ((float*)&vm->regs.C)[0] = value; break;
  case REG_DHD: ((float*)&vm->regs.D)[1] = value; break;
  case REG_DLD: ((float*)&vm->regs.D)[0] = value; break;
  case REG_EHD: ((float*)&vm->regs.E)[1] = value; break;
  case REG_ELD: ((float*)&vm->regs.E)[0] = value; break;
  case REG_FHD: ((float*)&vm->regs.F)[1] = value; break;
  case REG_FLD: ((float*)&vm->regs.F)[0] = value; break;
  default: unreachable();
  }
}

void vm_register_f64_set(vm_t* vm, register_t reg, double value)
{
  switch (reg)
  {
  case REG_A: *(double*)&vm->regs.A = value; break;
  case REG_B: *(double*)&vm->regs.B = value; break;
  case REG_C: *(double*)&vm->regs.C = value; break;
  case REG_D: *(double*)&vm->regs.D = value; break;
  case REG_E: *(double*)&vm->regs.E = value; break;
  case REG_F: *(double*)&vm->regs.F = value; break;
  default: unreachable();
  }
}

#define template_impl_vm_code_next(TYPE)\
  do {\
    TYPE value;\
    memcpy(&value, (const void*)(vm->code.data + vm->regs.IP), sizeof(TYPE));\
    vm->regs.IP += sizeof(TYPE);\
    return value;\
  } while (0)\

uint8_t  vm_code_next_u8 (vm_t* vm) { template_impl_vm_code_next(uint8_t);  }
uint16_t vm_code_next_u16(vm_t* vm) { template_impl_vm_code_next(uint16_t); }
uint32_t vm_code_next_u32(vm_t* vm) { template_impl_vm_code_next(uint32_t); }
uint64_t vm_code_next_u64(vm_t* vm) { template_impl_vm_code_next(uint64_t); }
int8_t   vm_code_next_i8 (vm_t* vm) { template_impl_vm_code_next(int8_t);   }
int16_t  vm_code_next_i16(vm_t* vm) { template_impl_vm_code_next(int16_t);  }
int32_t  vm_code_next_i32(vm_t* vm) { template_impl_vm_code_next(int32_t);  }
int64_t  vm_code_next_i64(vm_t* vm) { template_impl_vm_code_next(int64_t);  }
float    vm_code_next_f32(vm_t* vm) { template_impl_vm_code_next(float);    }
double   vm_code_next_f64(vm_t* vm) { template_impl_vm_code_next(double);   }

void* vm_code_next_addr(vm_t* vm)
{
  addr_mode_t mode;
  register_t base;
  register_t index;
  int32_t scale;
  int64_t offset;

  size_t size = tasm_addr_decode((const void*)(vm->code.data + vm->regs.IP), &mode, &base, &index, &scale, &offset);
  vm->regs.IP += size;

  return vm_addr_effective(vm, mode, base, index, scale, offset);
}

#define template_impl_vm_mem_get(TYPE)\
  do {\
    unused(vm);\
    TYPE value;\
    memcpy(&value, mem, sizeof(TYPE));\
    return value;\
  } while (0)\

uint8_t  vm_mem_u8_get (vm_t* vm, const void* mem) { template_impl_vm_mem_get(uint8_t);  }
uint16_t vm_mem_u16_get(vm_t* vm, const void* mem) { template_impl_vm_mem_get(uint16_t); }
uint32_t vm_mem_u32_get(vm_t* vm, const void* mem) { template_impl_vm_mem_get(uint32_t); }
uint64_t vm_mem_u64_get(vm_t* vm, const void* mem) { template_impl_vm_mem_get(uint64_t); }
int8_t   vm_mem_i8_get (vm_t* vm, const void* mem) { template_impl_vm_mem_get(int8_t);   }
int16_t  vm_mem_i16_get(vm_t* vm, const void* mem) { template_impl_vm_mem_get(int16_t);  }
int32_t  vm_mem_i32_get(vm_t* vm, const void* mem) { template_impl_vm_mem_get(int32_t);  }
int64_t  vm_mem_i64_get(vm_t* vm, const void* mem) { template_impl_vm_mem_get(int64_t);  }
float    vm_mem_f32_get(vm_t* vm, const void* mem) { template_impl_vm_mem_get(float);    }
double   vm_mem_f64_get(vm_t* vm, const void* mem) { template_impl_vm_mem_get(double);   }

#define template_impl_vm_mem_set(TYPE)\
  do {\
    unused(vm);\
    memcpy(mem, &value, sizeof(TYPE));\
  } while (0)\

void vm_mem_u8_set (vm_t* vm, void* mem, uint8_t  value) { template_impl_vm_mem_set(uint8_t);  }
void vm_mem_u16_set(vm_t* vm, void* mem, uint16_t value) { template_impl_vm_mem_set(uint16_t); }
void vm_mem_u32_set(vm_t* vm, void* mem, uint32_t value) { template_impl_vm_mem_set(uint32_t); }
void vm_mem_u64_set(vm_t* vm, void* mem, uint64_t value) { template_impl_vm_mem_set(uint64_t); }
void vm_mem_i8_set (vm_t* vm, void* mem, int8_t   value) { template_impl_vm_mem_set(int8_t);   }
void vm_mem_i16_set(vm_t* vm, void* mem, int16_t  value) { template_impl_vm_mem_set(int16_t);  }
void vm_mem_i32_set(vm_t* vm, void* mem, int32_t  value) { template_impl_vm_mem_set(int32_t);  }
void vm_mem_i64_set(vm_t* vm, void* mem, int64_t  value) { template_impl_vm_mem_set(int64_t);  }
void vm_mem_f32_set(vm_t* vm, void* mem, float    value) { template_impl_vm_mem_set(float);    }
void vm_mem_f64_set(vm_t* vm, void* mem, double   value) { template_impl_vm_mem_set(double);   }

void* vm_addr_effective(vm_t* vm, addr_mode_t mode, register_t base, register_t index, int32_t scale, int64_t offset)
{
  switch (mode)
  {
  case ADDR_MODE_OFFSET:                  return (void*)(                                                                         offset);
  case ADDR_MODE_BASE:                    return (void*)(vm_register_u64_get(vm, base)                                                  );
  case ADDR_MODE_BASE_OFFSET:             return (void*)(vm_register_u64_get(vm, base)                                          + offset);
  case ADDR_MODE_BASE_INDEX:              return (void*)(vm_register_u64_get(vm, base) + vm_register_u64_get(vm, index)                 );
  case ADDR_MODE_BASE_INDEX_OFFSET:       return (void*)(vm_register_u64_get(vm, base) + vm_register_u64_get(vm, index)         + offset);
  case ADDR_MODE_BASE_INDEX_SCALE:        return (void*)(vm_register_u64_get(vm, base) + vm_register_u64_get(vm, index) * scale         );
  case ADDR_MODE_INDEX_SCALE_OFFSET:      return (void*)(                                vm_register_u64_get(vm, index) * scale + offset);
  case ADDR_MODE_BASE_INDEX_SCALE_OFFSET: return (void*)(vm_register_u64_get(vm, base) + vm_register_u64_get(vm, index) * scale + offset);
  default: unreachable();
  }

  return NULL;
}

#define template_impl_vm_stack_push(TYPE)\
  do {\
    assert((uint8_t*)vm->regs.SP - sizeof(TYPE) >= vm->stack.data);\
    vm->regs.SP -= sizeof(TYPE);\
    memcpy((void*)vm->regs.SP, &value, sizeof(TYPE));\
  } while (0)\

void vm_stack_u8_push (vm_t* vm, uint8_t  value) { template_impl_vm_stack_push(uint8_t);  }
void vm_stack_u16_push(vm_t* vm, uint16_t value) { template_impl_vm_stack_push(uint16_t); }
void vm_stack_u32_push(vm_t* vm, uint32_t value) { template_impl_vm_stack_push(uint32_t); }
void vm_stack_u64_push(vm_t* vm, uint64_t value) { template_impl_vm_stack_push(uint64_t); }
void vm_stack_i8_push (vm_t* vm, int8_t   value) { template_impl_vm_stack_push(int8_t);   }
void vm_stack_i16_push(vm_t* vm, int16_t  value) { template_impl_vm_stack_push(int16_t);  }
void vm_stack_i32_push(vm_t* vm, int32_t  value) { template_impl_vm_stack_push(int32_t);  }
void vm_stack_i64_push(vm_t* vm, int64_t  value) { template_impl_vm_stack_push(int64_t);  }
void vm_stack_f32_push(vm_t* vm, float    value) { template_impl_vm_stack_push(float);    }
void vm_stack_f64_push(vm_t* vm, double   value) { template_impl_vm_stack_push(double);   }

#define template_impl_vm_stack_pop(TYPE)\
  do {\
    assert((uint8_t*)vm->regs.SP + sizeof(TYPE) <= vm->stack.data + VM_STACK_DEFAULT_SIZE);\
    TYPE value;\
    memcpy(&value, (void*)vm->regs.SP, sizeof(TYPE));\
    vm->regs.SP += sizeof(TYPE);\
    return value;\
  } while (0)\

uint8_t  vm_stack_u8_pop (vm_t* vm) { template_impl_vm_stack_pop(uint8_t);  }
uint16_t vm_stack_u16_pop(vm_t* vm) { template_impl_vm_stack_pop(uint16_t); }
uint32_t vm_stack_u32_pop(vm_t* vm) { template_impl_vm_stack_pop(uint32_t); }
uint64_t vm_stack_u64_pop(vm_t* vm) { template_impl_vm_stack_pop(uint64_t); }
int8_t   vm_stack_i8_pop (vm_t* vm) { template_impl_vm_stack_pop(int8_t);   }
int16_t  vm_stack_i16_pop(vm_t* vm) { template_impl_vm_stack_pop(int16_t);  }
int32_t  vm_stack_i32_pop(vm_t* vm) { template_impl_vm_stack_pop(int32_t);  }
int64_t  vm_stack_i64_pop(vm_t* vm) { template_impl_vm_stack_pop(int64_t);  }
float    vm_stack_f32_pop(vm_t* vm) { template_impl_vm_stack_pop(float);    }
double   vm_stack_f64_pop(vm_t* vm) { template_impl_vm_stack_pop(double);   }

void vm_fetch(vm_t* vm)
{
  vm->inst.raw = vm_code_next_u16(vm);
}

void vm_decode(vm_t* vm)
{
  opcode_decode(vm->inst.raw, &vm->inst.opcode, &vm->inst.param, &vm->inst.width);
}

static inline void vm_execute_MOV_reg_reg(vm_t* vm)
{
  uint8_t encoded_regs = vm_code_next_u8(vm);
  register_t reg_dest = register_decode((encoded_regs >> 4) & 0xF, vm->inst.width);
  register_t reg_src = register_decode(encoded_regs & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, vm_register_u8_get (vm, reg_src)); break;
  case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, vm_register_u16_get(vm, reg_src)); break;
  case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, vm_register_u32_get(vm, reg_src)); break;
  case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, vm_register_u64_get(vm, reg_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_MOV_mem_reg(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_src = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, vm_register_u8_get (vm, reg_src)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, vm_register_u16_get(vm, reg_src)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, vm_register_u32_get(vm, reg_src)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, vm_register_u64_get(vm, reg_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_MOV_reg_mem(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  void* mem_src = vm_code_next_addr(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, vm_mem_u8_get (vm, mem_src)); break;
  case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, vm_mem_u16_get(vm, mem_src)); break;
  case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, vm_mem_u32_get(vm, mem_src)); break;
  case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, vm_mem_u64_get(vm, mem_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_MOV_mem_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);
  void* mem_src = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, vm_mem_u8_get (vm, mem_src)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, vm_mem_u16_get(vm, mem_src)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, vm_mem_u32_get(vm, mem_src)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, vm_mem_u64_get(vm, mem_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_MOV_reg_imm(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, vm_code_next_u8 (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, vm_code_next_u16(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, vm_code_next_u32(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, vm_code_next_u64(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_MOV_mem_imm(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, vm_code_next_u8 (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, vm_code_next_u16(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, vm_code_next_u32(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, vm_code_next_u64(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_MOV(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG_REG: vm_execute_MOV_reg_reg(vm); break;
  case OPCODE_PARAM_MEM_REG: vm_execute_MOV_mem_reg(vm); break;
  case OPCODE_PARAM_REG_MEM: vm_execute_MOV_reg_mem(vm); break;
  case OPCODE_PARAM_MEM_MEM: vm_execute_MOV_mem_mem(vm); break;
  case OPCODE_PARAM_REG_IMM: vm_execute_MOV_reg_imm(vm); break;
  case OPCODE_PARAM_MEM_IMM: vm_execute_MOV_mem_imm(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_PSH_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_src = register_decode(encoded_reg, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_stack_u8_push (vm, vm_register_u8_get (vm, reg_src)); break;
  case OPCODE_WIDTH_16BIT: vm_stack_u16_push(vm, vm_register_u16_get(vm, reg_src)); break;
  case OPCODE_WIDTH_32BIT: vm_stack_u32_push(vm, vm_register_u32_get(vm, reg_src)); break;
  case OPCODE_WIDTH_64BIT: vm_stack_u64_push(vm, vm_register_u64_get(vm, reg_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_PSH_mem(vm_t* vm)
{
  void* addr_src = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_stack_u8_push (vm, vm_mem_u8_get (vm, addr_src)); break;
  case OPCODE_WIDTH_16BIT: vm_stack_u16_push(vm, vm_mem_u16_get(vm, addr_src)); break;
  case OPCODE_WIDTH_32BIT: vm_stack_u32_push(vm, vm_mem_u32_get(vm, addr_src)); break;
  case OPCODE_WIDTH_64BIT: vm_stack_u64_push(vm, vm_mem_u64_get(vm, addr_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_PSH_imm(vm_t* vm)
{
  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_stack_u8_push (vm, vm_code_next_u8 (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_stack_u16_push(vm, vm_code_next_u16(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_stack_u32_push(vm, vm_code_next_u32(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_stack_u64_push(vm, vm_code_next_u64(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_PSH(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_PSH_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_PSH_mem(vm); break;
  case OPCODE_PARAM_IMM: vm_execute_PSH_imm(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_POP_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, vm_stack_u8_pop (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, vm_stack_u16_pop(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, vm_stack_u32_pop(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, vm_stack_u64_pop(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_POP_mem(vm_t* vm)
{
  void* addr_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, addr_dest, vm_stack_u8_pop (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, addr_dest, vm_stack_u16_pop(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, addr_dest, vm_stack_u32_pop(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, addr_dest, vm_stack_u64_pop(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_POP(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_POP_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_POP_mem(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_LEA(vm_t* vm)
{
  assert(vm->inst.param == OPCODE_PARAM_REG_MEM);
  assert(vm->inst.width == OPCODE_WIDTH_64BIT);

  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg, OPCODE_WIDTH_64BIT);
  void* addr = vm_code_next_addr(vm);

  vm_register_u64_set(vm, reg_dest, (uint64_t)addr);
}

#define template_impl_vm_parity(TYPE)\
  do {\
    TYPE parity = 0;\
    while (value)\
    {\
      parity ^= value & 1;\
      value >>= 1;\
    }\
    return parity;\
  } while (0)\

static inline uint8_t  vm_parity_u8 (uint8_t  value) { template_impl_vm_parity(uint8_t ); }
static inline uint16_t vm_parity_u16(uint16_t value) { template_impl_vm_parity(uint16_t); }
static inline uint32_t vm_parity_u32(uint32_t value) { template_impl_vm_parity(uint32_t); }
static inline uint64_t vm_parity_u64(uint64_t value) { template_impl_vm_parity(uint64_t); }

#define template_impl_vm_execute_UNSIGNED_BIN_OP_reg_reg(OP)\
  do {\
    uint8_t encoded_regs = vm_code_next_u8(vm);\
    register_t reg_dest = register_decode((encoded_regs >> 4) & 0xF, vm->inst.width);\
    register_t reg_src = register_decode(encoded_regs & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, (vm_##OP##8 )(vm, vm_register_u8_get (vm, reg_dest), vm_register_u8_get (vm, reg_src))); break;\
    case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, (vm_##OP##16)(vm, vm_register_u16_get(vm, reg_dest), vm_register_u16_get(vm, reg_src))); break;\
    case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, (vm_##OP##32)(vm, vm_register_u32_get(vm, reg_dest), vm_register_u32_get(vm, reg_src))); break;\
    case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, (vm_##OP##64)(vm, vm_register_u64_get(vm, reg_dest), vm_register_u64_get(vm, reg_src))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_UNSIGNED_BIN_OP_mem_reg(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    register_t reg_src = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, (vm_##OP##8 )(vm, vm_mem_u8_get (vm, mem_dest), vm_register_u8_get (vm, reg_src))); break;\
    case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, (vm_##OP##16)(vm, vm_mem_u16_get(vm, mem_dest), vm_register_u16_get(vm, reg_src))); break;\
    case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, (vm_##OP##32)(vm, vm_mem_u32_get(vm, mem_dest), vm_register_u32_get(vm, reg_src))); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, (vm_##OP##64)(vm, vm_mem_u64_get(vm, mem_dest), vm_register_u64_get(vm, reg_src))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_UNSIGNED_BIN_OP_reg_mem(OP)\
  do {\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    void* mem_src = vm_code_next_addr(vm);\
    register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, (vm_##OP##8 )(vm, vm_register_u8_get (vm, reg_dest), vm_mem_u8_get (vm, mem_src))); break;\
    case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, (vm_##OP##16)(vm, vm_register_u16_get(vm, reg_dest), vm_mem_u16_get(vm, mem_src))); break;\
    case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, (vm_##OP##32)(vm, vm_register_u32_get(vm, reg_dest), vm_mem_u32_get(vm, mem_src))); break;\
    case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, (vm_##OP##64)(vm, vm_register_u64_get(vm, reg_dest), vm_mem_u64_get(vm, mem_src))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_UNSIGNED_BIN_OP_mem_mem(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    void* mem_src = vm_code_next_addr(vm);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, (vm_##OP##8 )(vm, vm_mem_u8_get (vm, mem_dest), vm_mem_u8_get (vm, mem_src))); break;\
    case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, (vm_##OP##16)(vm, vm_mem_u16_get(vm, mem_dest), vm_mem_u16_get(vm, mem_src))); break;\
    case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, (vm_##OP##32)(vm, vm_mem_u32_get(vm, mem_dest), vm_mem_u32_get(vm, mem_src))); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, (vm_##OP##64)(vm, vm_mem_u64_get(vm, mem_dest), vm_mem_u64_get(vm, mem_src))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_UNSIGNED_BIN_OP_reg_imm(OP)\
  do {\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, (vm_##OP##8 )(vm, vm_register_u8_get (vm, reg_dest), vm_code_next_u8 (vm))); break;\
    case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, (vm_##OP##16)(vm, vm_register_u16_get(vm, reg_dest), vm_code_next_u16(vm))); break;\
    case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, (vm_##OP##32)(vm, vm_register_u32_get(vm, reg_dest), vm_code_next_u32(vm))); break;\
    case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, (vm_##OP##64)(vm, vm_register_u64_get(vm, reg_dest), vm_code_next_u64(vm))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_UNSIGNED_BIN_OP_mem_imm(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, (vm_##OP##8 )(vm, vm_mem_u8_get (vm, mem_dest), vm_code_next_u8 (vm))); break;\
    case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, (vm_##OP##16)(vm, vm_mem_u16_get(vm, mem_dest), vm_code_next_u16(vm))); break;\
    case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, (vm_##OP##32)(vm, vm_mem_u32_get(vm, mem_dest), vm_code_next_u32(vm))); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, (vm_##OP##64)(vm, vm_mem_u64_get(vm, mem_dest), vm_code_next_u64(vm))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_decls_vm_execute_BIN_OP(OP, SIGN)\
  static inline void vm_execute_##OP##_reg_reg(vm_t* vm) { template_impl_vm_execute_##SIGN##_BIN_OP_reg_reg(OP); }\
  static inline void vm_execute_##OP##_mem_reg(vm_t* vm) { template_impl_vm_execute_##SIGN##_BIN_OP_mem_reg(OP); }\
  static inline void vm_execute_##OP##_reg_mem(vm_t* vm) { template_impl_vm_execute_##SIGN##_BIN_OP_reg_mem(OP); }\
  static inline void vm_execute_##OP##_mem_mem(vm_t* vm) { template_impl_vm_execute_##SIGN##_BIN_OP_mem_mem(OP); }\
  static inline void vm_execute_##OP##_reg_imm(vm_t* vm) { template_impl_vm_execute_##SIGN##_BIN_OP_reg_imm(OP); }\
  static inline void vm_execute_##OP##_mem_imm(vm_t* vm) { template_impl_vm_execute_##SIGN##_BIN_OP_mem_imm(OP); }\
  static inline void vm_execute_##OP(vm_t* vm)\
  {\
    switch (vm->inst.param)\
    {\
    case OPCODE_PARAM_REG_REG: vm_execute_##OP##_reg_reg(vm); break;\
    case OPCODE_PARAM_MEM_REG: vm_execute_##OP##_mem_reg(vm); break;\
    case OPCODE_PARAM_REG_MEM: vm_execute_##OP##_reg_mem(vm); break;\
    case OPCODE_PARAM_MEM_MEM: vm_execute_##OP##_mem_mem(vm); break;\
    case OPCODE_PARAM_REG_IMM: vm_execute_##OP##_reg_imm(vm); break;\
    case OPCODE_PARAM_MEM_IMM: vm_execute_##OP##_mem_imm(vm); break;\
    default: unreachable();\
    }\
  }\

#define template_decls_vm_UNSIGNED_BIN_OP(OP)\
  static inline uint8_t  vm_##OP##8 (vm_t* vm, uint8_t  lhs, uint8_t  rhs) { template_impl_vm_##OP(0, UINT8_MAX,  vm_parity_u8 ); }\
  static inline uint16_t vm_##OP##16(vm_t* vm, uint16_t lhs, uint16_t rhs) { template_impl_vm_##OP(0, UINT16_MAX, vm_parity_u16); }\
  static inline uint32_t vm_##OP##32(vm_t* vm, uint32_t lhs, uint32_t rhs) { template_impl_vm_##OP(0, UINT32_MAX, vm_parity_u32); }\
  static inline uint64_t vm_##OP##64(vm_t* vm, uint64_t lhs, uint64_t rhs) { template_impl_vm_##OP(0, UINT64_MAX, vm_parity_u64); }\

#define template_decls_vm_SIGNED_BIN_OP(OP)\
  static inline int8_t  vm_##OP##8 (vm_t* vm, int8_t  lhs, int8_t  rhs) { template_impl_vm_##OP(INT8_MIN,  INT8_MAX,  vm_parity_u8 ); }\
  static inline int16_t vm_##OP##16(vm_t* vm, int16_t lhs, int16_t rhs) { template_impl_vm_##OP(INT16_MIN, INT16_MAX, vm_parity_u16); }\
  static inline int32_t vm_##OP##32(vm_t* vm, int32_t lhs, int32_t rhs) { template_impl_vm_##OP(INT32_MIN, INT32_MAX, vm_parity_u32); }\
  static inline int64_t vm_##OP##64(vm_t* vm, int64_t lhs, int64_t rhs) { template_impl_vm_##OP(INT64_MIN, INT64_MAX, vm_parity_u64); }\

#define template_decls_vm_BIN_OP(OP, SIGN)\
  template_decls_vm_##SIGN##_BIN_OP(OP)\

#define template_impl_vm_ADD(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.carry = (MAX) - lhs < rhs;\
    vm->regs.FLAGS.parity = (PARITY)(lhs + rhs);\
    return lhs + rhs;\
  } while (0)\

template_decls_vm_BIN_OP(ADD, UNSIGNED)
template_decls_vm_execute_BIN_OP(ADD, UNSIGNED)

#define template_impl_vm_SUB(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs == rhs;\
    vm->regs.FLAGS.negative = (lhs - rhs) >> (sizeof(lhs) * 8 - 1);\
    vm->regs.FLAGS.carry = lhs < rhs;\
    vm->regs.FLAGS.parity = (PARITY)(lhs - rhs);\
    return lhs - rhs;\
  } while (0)\

template_decls_vm_BIN_OP(SUB, UNSIGNED)
template_decls_vm_execute_BIN_OP(SUB, UNSIGNED)

#define template_impl_vm_MUL(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs * rhs == 0;\
    vm->regs.FLAGS.carry = rhs != 0 && lhs > (MAX) / rhs;\
    vm->regs.FLAGS.parity = (PARITY)(lhs * rhs);\
    return lhs * rhs;\
  } while (0)\

template_decls_vm_BIN_OP(MUL, UNSIGNED)
template_decls_vm_execute_BIN_OP(MUL, UNSIGNED)

#define template_impl_vm_DIV(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs == 0;\
    vm->regs.FLAGS.carry = lhs % rhs != 0;\
    vm->regs.FLAGS.parity = (PARITY)(lhs / rhs);\
    return lhs / rhs;\
  } while (0)\

template_decls_vm_BIN_OP(DIV, UNSIGNED)
template_decls_vm_execute_BIN_OP(DIV, UNSIGNED)

#define template_impl_vm_MOD(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs % rhs == 0;\
    vm->regs.FLAGS.carry = lhs % rhs != 0;\
    vm->regs.FLAGS.parity = (PARITY)(lhs % rhs);\
    return lhs % rhs;\
  } while (0)\

template_decls_vm_BIN_OP(MOD, UNSIGNED)
template_decls_vm_execute_BIN_OP(MOD, UNSIGNED)

static inline void vm_execute_INC_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, vm_ADD8 (vm, vm_register_u8_get (vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, vm_ADD16(vm, vm_register_u16_get(vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, vm_ADD32(vm, vm_register_u32_get(vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, vm_ADD64(vm, vm_register_u64_get(vm, reg_dest), 1)); break;
  default: unreachable();
  }
}

static inline void vm_execute_INC_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, vm_ADD8 (vm, vm_mem_u8_get (vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, vm_ADD16(vm, vm_mem_u16_get(vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, vm_ADD32(vm, vm_mem_u32_get(vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, vm_ADD64(vm, vm_mem_u64_get(vm, mem_dest), 1)); break;
  default: unreachable();
  }
}

static inline void vm_execute_INC(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_INC_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_INC_mem(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_DEC_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, vm_SUB8 (vm, vm_register_u8_get (vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, vm_SUB16(vm, vm_register_u16_get(vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, vm_SUB32(vm, vm_register_u32_get(vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, vm_SUB64(vm, vm_register_u64_get(vm, reg_dest), 1)); break;
  default: unreachable();
  }
}

static inline void vm_execute_DEC_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, vm_SUB8 (vm, vm_mem_u8_get (vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, vm_SUB16(vm, vm_mem_u16_get(vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, vm_SUB32(vm, vm_mem_u32_get(vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, vm_SUB64(vm, vm_mem_u64_get(vm, mem_dest), 1)); break;
  default: unreachable();
  }
}

static inline void vm_execute_DEC(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_DEC_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_DEC_mem(vm); break;
  default: unreachable();
  }
}

#define template_impl_vm_execute_SIGNED_BIN_OP_reg_reg(OP)\
  do {\
    uint8_t encoded_regs = vm_code_next_u8(vm);\
    register_t reg_dest = register_decode((encoded_regs >> 4) & 0xF, vm->inst.width);\
    register_t reg_src = register_decode(encoded_regs & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_register_i8_set (vm, reg_dest, (vm_##OP##8 )(vm, vm_register_i8_get (vm, reg_dest), vm_register_i8_get (vm, reg_src))); break;\
    case OPCODE_WIDTH_16BIT: vm_register_i16_set(vm, reg_dest, (vm_##OP##16)(vm, vm_register_i16_get(vm, reg_dest), vm_register_i16_get(vm, reg_src))); break;\
    case OPCODE_WIDTH_32BIT: vm_register_i32_set(vm, reg_dest, (vm_##OP##32)(vm, vm_register_i32_get(vm, reg_dest), vm_register_i32_get(vm, reg_src))); break;\
    case OPCODE_WIDTH_64BIT: vm_register_i64_set(vm, reg_dest, (vm_##OP##64)(vm, vm_register_i64_get(vm, reg_dest), vm_register_i64_get(vm, reg_src))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_SIGNED_BIN_OP_mem_reg(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    register_t reg_src = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_mem_i8_set (vm, mem_dest, (vm_##OP##8 )(vm, vm_mem_i8_get (vm, mem_dest), vm_register_i8_get (vm, reg_src))); break;\
    case OPCODE_WIDTH_16BIT: vm_mem_i16_set(vm, mem_dest, (vm_##OP##16)(vm, vm_mem_i16_get(vm, mem_dest), vm_register_i16_get(vm, reg_src))); break;\
    case OPCODE_WIDTH_32BIT: vm_mem_i32_set(vm, mem_dest, (vm_##OP##32)(vm, vm_mem_i32_get(vm, mem_dest), vm_register_i32_get(vm, reg_src))); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_i64_set(vm, mem_dest, (vm_##OP##64)(vm, vm_mem_i64_get(vm, mem_dest), vm_register_i64_get(vm, reg_src))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_SIGNED_BIN_OP_reg_mem(OP)\
  do {\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    void* mem_src = vm_code_next_addr(vm);\
    register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_register_i8_set (vm, reg_dest, (vm_##OP## 8)(vm, vm_register_i8_get (vm, reg_dest), vm_mem_i8_get (vm, mem_src))); break;\
    case OPCODE_WIDTH_16BIT: vm_register_i16_set(vm, reg_dest, (vm_##OP##16)(vm, vm_register_i16_get(vm, reg_dest), vm_mem_i16_get(vm, mem_src))); break;\
    case OPCODE_WIDTH_32BIT: vm_register_i32_set(vm, reg_dest, (vm_##OP##32)(vm, vm_register_i32_get(vm, reg_dest), vm_mem_i32_get(vm, mem_src))); break;\
    case OPCODE_WIDTH_64BIT: vm_register_i64_set(vm, reg_dest, (vm_##OP##64)(vm, vm_register_i64_get(vm, reg_dest), vm_mem_i64_get(vm, mem_src))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_SIGNED_BIN_OP_mem_mem(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    void* mem_src = vm_code_next_addr(vm);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_mem_i8_set (vm, mem_dest, (vm_##OP##8 )(vm, vm_mem_i8_get (vm, mem_dest), vm_mem_i8_get (vm, mem_src))); break;\
    case OPCODE_WIDTH_16BIT: vm_mem_i16_set(vm, mem_dest, (vm_##OP##16)(vm, vm_mem_i16_get(vm, mem_dest), vm_mem_i16_get(vm, mem_src))); break;\
    case OPCODE_WIDTH_32BIT: vm_mem_i32_set(vm, mem_dest, (vm_##OP##32)(vm, vm_mem_i32_get(vm, mem_dest), vm_mem_i32_get(vm, mem_src))); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_i64_set(vm, mem_dest, (vm_##OP##64)(vm, vm_mem_i64_get(vm, mem_dest), vm_mem_i64_get(vm, mem_src))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_SIGNED_BIN_OP_reg_imm(OP)\
  do {\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_register_i8_set (vm, reg_dest, (vm_##OP##8 )(vm, vm_register_i8_get (vm, reg_dest), vm_code_next_i8 (vm))); break;\
    case OPCODE_WIDTH_16BIT: vm_register_i16_set(vm, reg_dest, (vm_##OP##16)(vm, vm_register_i16_get(vm, reg_dest), vm_code_next_i16(vm))); break;\
    case OPCODE_WIDTH_32BIT: vm_register_i32_set(vm, reg_dest, (vm_##OP##32)(vm, vm_register_i32_get(vm, reg_dest), vm_code_next_i32(vm))); break;\
    case OPCODE_WIDTH_64BIT: vm_register_i64_set(vm, reg_dest, (vm_##OP##64)(vm, vm_register_i64_get(vm, reg_dest), vm_code_next_i64(vm))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_SIGNED_BIN_OP_mem_imm(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_8BIT:  vm_mem_i8_set (vm, mem_dest, (vm_##OP##8 )(vm, vm_mem_i8_get (vm, mem_dest), vm_code_next_i8 (vm))); break;\
    case OPCODE_WIDTH_16BIT: vm_mem_i16_set(vm, mem_dest, (vm_##OP##16)(vm, vm_mem_i16_get(vm, mem_dest), vm_code_next_i16(vm))); break;\
    case OPCODE_WIDTH_32BIT: vm_mem_i32_set(vm, mem_dest, (vm_##OP##32)(vm, vm_mem_i32_get(vm, mem_dest), vm_code_next_i32(vm))); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_i64_set(vm, mem_dest, (vm_##OP##64)(vm, vm_mem_i64_get(vm, mem_dest), vm_code_next_i64(vm))); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_IADD(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs + rhs == 0;\
    vm->regs.FLAGS.negative = lhs + rhs < 0;\
    vm->regs.FLAGS.overflow = (rhs > 0 && lhs > (MAX) - rhs) || (rhs < 0 && lhs < (MIN) - rhs);\
    vm->regs.FLAGS.parity = (PARITY)(lhs + rhs);\
    return lhs + rhs;\
  } while (0)\

template_decls_vm_BIN_OP(IADD, SIGNED)
template_decls_vm_execute_BIN_OP(IADD, SIGNED)

#define template_impl_vm_ISUB(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs - rhs == 0;\
    vm->regs.FLAGS.negative = lhs - rhs < 0;\
    vm->regs.FLAGS.overflow = (rhs > 0 && lhs < (MIN) + rhs) || (rhs < 0 && lhs > (MAX) + rhs);\
    vm->regs.FLAGS.parity = (PARITY)(lhs - rhs);\
    return lhs - rhs;\
  } while (0)\

template_decls_vm_BIN_OP(ISUB, SIGNED)
template_decls_vm_execute_BIN_OP(ISUB, SIGNED)

#define template_impl_vm_IMUL(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs * rhs == 0;\
    vm->regs.FLAGS.negative = lhs * rhs < 0;\
    vm->regs.FLAGS.overflow = (lhs > 0 && ((rhs > 0 && lhs > (MAX) / rhs) || (rhs < 0 && rhs < (MIN) / lhs))) ||\
                              (lhs < 0 && ((rhs > 0 && lhs < (MIN) / rhs) || (rhs < 0 && (lhs != -1 || rhs != (MIN)) && rhs < (MAX) / lhs)));\
    vm->regs.FLAGS.parity = (PARITY)(lhs * rhs);\
    return lhs * rhs;\
  } while (0)\

template_decls_vm_BIN_OP(IMUL, SIGNED)
template_decls_vm_execute_BIN_OP(IMUL, SIGNED)

#define template_impl_vm_IDIV(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs == 0;\
    vm->regs.FLAGS.negative = (lhs < 0) != (rhs < 0);\
    vm->regs.FLAGS.overflow = rhs == 0 || (lhs == (MIN) && rhs == -1);\
    vm->regs.FLAGS.parity = (PARITY)(lhs / rhs);\
    return lhs / rhs;\
  } while (0)\

template_decls_vm_BIN_OP(IDIV, SIGNED)
template_decls_vm_execute_BIN_OP(IDIV, SIGNED)

#define template_impl_vm_IMOD(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = lhs % rhs == 0;\
    vm->regs.FLAGS.negative = lhs % rhs < 0;\
    vm->regs.FLAGS.overflow = lhs == (MIN) && rhs == -1;\
    vm->regs.FLAGS.parity = (PARITY)(lhs % rhs);\
    return lhs % rhs;\
  } while (0)\

template_decls_vm_BIN_OP(IMOD, SIGNED)
template_decls_vm_execute_BIN_OP(IMOD, SIGNED)

#define template_impl_vm_INEG(PARITY)\
  do {\
    vm->regs.FLAGS.zero = value == 0;\
    vm->regs.FLAGS.negative = value > 0;\
    vm->regs.FLAGS.parity = (PARITY)(-value);\
    return -value;\
  } while (0)\

static inline int8_t  vm_INEG8 (vm_t* vm, int8_t  value) { template_impl_vm_INEG(vm_parity_u8 ); }
static inline int16_t vm_INEG16(vm_t* vm, int16_t value) { template_impl_vm_INEG(vm_parity_u16); }
static inline int32_t vm_INEG32(vm_t* vm, int32_t value) { template_impl_vm_INEG(vm_parity_u32); }
static inline int64_t vm_INEG64(vm_t* vm, int64_t value) { template_impl_vm_INEG(vm_parity_u64); }

static inline void vm_execute_INEG_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_i8_set (vm, reg_dest, -vm_register_i8_get (vm, reg_dest)); break;
  case OPCODE_WIDTH_16BIT: vm_register_i16_set(vm, reg_dest, -vm_register_i16_get(vm, reg_dest)); break;
  case OPCODE_WIDTH_32BIT: vm_register_i32_set(vm, reg_dest, -vm_register_i32_get(vm, reg_dest)); break;
  case OPCODE_WIDTH_64BIT: vm_register_i64_set(vm, reg_dest, -vm_register_i64_get(vm, reg_dest)); break;
  default: unreachable();
  }
}

static inline void vm_execute_INEG_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_i8_set (vm, mem_dest, -vm_mem_i8_get (vm, mem_dest)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_i16_set(vm, mem_dest, -vm_mem_i16_get(vm, mem_dest)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_i32_set(vm, mem_dest, -vm_mem_i32_get(vm, mem_dest)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_i64_set(vm, mem_dest, -vm_mem_i64_get(vm, mem_dest)); break;
  default: unreachable();
  }
}

static inline void vm_execute_INEG(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_INEG_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_INEG_mem(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_IINC_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_i8_set (vm, reg_dest, vm_IADD8 (vm, vm_register_i8_get (vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_16BIT: vm_register_i16_set(vm, reg_dest, vm_IADD16(vm, vm_register_i16_get(vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_32BIT: vm_register_i32_set(vm, reg_dest, vm_IADD32(vm, vm_register_i32_get(vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_64BIT: vm_register_i64_set(vm, reg_dest, vm_IADD64(vm, vm_register_i64_get(vm, reg_dest), 1)); break;
  default: unreachable();
  }
}

static inline void vm_execute_IINC_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_i8_set (vm, mem_dest, vm_IADD8 (vm, vm_mem_i8_get (vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_i16_set(vm, mem_dest, vm_IADD16(vm, vm_mem_i16_get(vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_i32_set(vm, mem_dest, vm_IADD32(vm, vm_mem_i32_get(vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_i64_set(vm, mem_dest, vm_IADD64(vm, vm_mem_i64_get(vm, mem_dest), 1)); break;
  default: unreachable();
  }
}

static inline void vm_execute_IINC(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_IINC_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_IINC_mem(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_IDEC_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_i8_set (vm, reg_dest, vm_ISUB8 (vm, vm_register_i8_get (vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_16BIT: vm_register_i16_set(vm, reg_dest, vm_ISUB16(vm, vm_register_i16_get(vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_32BIT: vm_register_i32_set(vm, reg_dest, vm_ISUB32(vm, vm_register_i32_get(vm, reg_dest), 1)); break;
  case OPCODE_WIDTH_64BIT: vm_register_i64_set(vm, reg_dest, vm_ISUB64(vm, vm_register_i64_get(vm, reg_dest), 1)); break;
  default: unreachable();
  }
}

static inline void vm_execute_IDEC_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_i8_set (vm, mem_dest, vm_ISUB8 (vm, vm_mem_i8_get (vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_16BIT: vm_mem_i16_set(vm, mem_dest, vm_ISUB16(vm, vm_mem_i16_get(vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_32BIT: vm_mem_i32_set(vm, mem_dest, vm_ISUB32(vm, vm_mem_i32_get(vm, mem_dest), 1)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_i64_set(vm, mem_dest, vm_ISUB64(vm, vm_mem_i64_get(vm, mem_dest), 1)); break;
  default: unreachable();
  }
}

static inline void vm_execute_IDEC(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_IDEC_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_IDEC_mem(vm); break;
  default: unreachable();
  }
}

#define template_impl_vm_execute_FLOAT_BIN_OP_reg_reg(OP)\
  do {\
    uint8_t encoded_regs = vm_code_next_u8(vm);\
    register_t reg_dest = register_decode((encoded_regs >> 4) & 0xF, vm->inst.width);\
    register_t reg_src = register_decode(encoded_regs & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_32BIT: vm_register_f32_set(vm, reg_dest, vm_register_f32_get(vm, reg_dest) OP vm_register_f32_get(vm, reg_src)); break;\
    case OPCODE_WIDTH_64BIT: vm_register_f64_set(vm, reg_dest, vm_register_f64_get(vm, reg_dest) OP vm_register_f64_get(vm, reg_src)); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_FLOAT_BIN_OP_mem_reg(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    register_t reg_src = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_32BIT: vm_mem_f32_set(vm, mem_dest, vm_mem_f32_get(vm, mem_dest) OP vm_register_f32_get(vm, reg_src)); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_f64_set(vm, mem_dest, vm_mem_f64_get(vm, mem_dest) OP vm_register_f64_get(vm, reg_src)); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_FLOAT_BIN_OP_reg_mem(OP)\
  do {\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    void* mem_src = vm_code_next_addr(vm);\
    register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_32BIT: vm_register_f32_set(vm, reg_dest, vm_register_f32_get(vm, reg_dest) OP vm_mem_f32_get(vm, mem_src)); break;\
    case OPCODE_WIDTH_64BIT: vm_register_f64_set(vm, reg_dest, vm_register_f64_get(vm, reg_dest) OP vm_mem_f64_get(vm, mem_src)); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_FLOAT_BIN_OP_mem_mem(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    void* mem_src = vm_code_next_addr(vm);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_32BIT: vm_mem_f32_set(vm, mem_dest, vm_mem_f32_get(vm, mem_dest) OP vm_mem_f32_get(vm, mem_src)); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_f64_set(vm, mem_dest, vm_mem_f64_get(vm, mem_dest) OP vm_mem_f64_get(vm, mem_src)); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_FLOAT_BIN_OP_reg_imm(OP)\
  do {\
    uint8_t encoded_reg = vm_code_next_u8(vm);\
    register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_32BIT: vm_register_f32_set(vm, reg_dest, vm_register_f32_get(vm, reg_dest) OP vm_code_next_f32(vm)); break;\
    case OPCODE_WIDTH_64BIT: vm_register_f64_set(vm, reg_dest, vm_register_f64_get(vm, reg_dest) OP vm_code_next_f64(vm)); break;\
    default: unreachable();\
    }\
  } while (0)\

#define template_impl_vm_execute_FLOAT_BIN_OP_mem_imm(OP)\
  do {\
    void* mem_dest = vm_code_next_addr(vm);\
    switch (vm->inst.width)\
    {\
    case OPCODE_WIDTH_32BIT: vm_mem_f32_set(vm, mem_dest, vm_mem_f32_get(vm, mem_dest) OP vm_code_next_f32(vm)); break;\
    case OPCODE_WIDTH_64BIT: vm_mem_f64_set(vm, mem_dest, vm_mem_f64_get(vm, mem_dest) OP vm_code_next_f64(vm)); break;\
    default: unreachable();\
    }\
  } while (0)\

static inline void vm_execute_FADD_reg_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_reg(+); }
static inline void vm_execute_FADD_mem_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_reg(+); }
static inline void vm_execute_FADD_reg_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_mem(+); }
static inline void vm_execute_FADD_mem_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_mem(+); }
static inline void vm_execute_FADD_reg_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_imm(+); }
static inline void vm_execute_FADD_mem_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_imm(+); }

static inline void vm_execute_FADD(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG_REG: vm_execute_FADD_reg_reg(vm); break;
  case OPCODE_PARAM_MEM_REG: vm_execute_FADD_mem_reg(vm); break;
  case OPCODE_PARAM_REG_MEM: vm_execute_FADD_reg_mem(vm); break;
  case OPCODE_PARAM_MEM_MEM: vm_execute_FADD_mem_mem(vm); break;
  case OPCODE_PARAM_REG_IMM: vm_execute_FADD_reg_imm(vm); break;
  case OPCODE_PARAM_MEM_IMM: vm_execute_FADD_mem_imm(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_FSUB_reg_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_reg(-); }
static inline void vm_execute_FSUB_mem_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_reg(-); }
static inline void vm_execute_FSUB_reg_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_mem(-); }
static inline void vm_execute_FSUB_mem_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_mem(-); }
static inline void vm_execute_FSUB_reg_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_imm(-); }
static inline void vm_execute_FSUB_mem_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_imm(-); }

static inline void vm_execute_FSUB(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG_REG: vm_execute_FSUB_reg_reg(vm); break;
  case OPCODE_PARAM_MEM_REG: vm_execute_FSUB_mem_reg(vm); break;
  case OPCODE_PARAM_REG_MEM: vm_execute_FSUB_reg_mem(vm); break;
  case OPCODE_PARAM_MEM_MEM: vm_execute_FSUB_mem_mem(vm); break;
  case OPCODE_PARAM_REG_IMM: vm_execute_FSUB_reg_imm(vm); break;
  case OPCODE_PARAM_MEM_IMM: vm_execute_FSUB_mem_imm(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_FMUL_reg_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_reg(*); }
static inline void vm_execute_FMUL_mem_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_reg(*); }
static inline void vm_execute_FMUL_reg_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_mem(*); }
static inline void vm_execute_FMUL_mem_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_mem(*); }
static inline void vm_execute_FMUL_reg_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_imm(*); }
static inline void vm_execute_FMUL_mem_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_imm(*); }

static inline void vm_execute_FMUL(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG_REG: vm_execute_FMUL_reg_reg(vm); break;
  case OPCODE_PARAM_MEM_REG: vm_execute_FMUL_mem_reg(vm); break;
  case OPCODE_PARAM_REG_MEM: vm_execute_FMUL_reg_mem(vm); break;
  case OPCODE_PARAM_MEM_MEM: vm_execute_FMUL_mem_mem(vm); break;
  case OPCODE_PARAM_REG_IMM: vm_execute_FMUL_reg_imm(vm); break;
  case OPCODE_PARAM_MEM_IMM: vm_execute_FMUL_mem_imm(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_FDIV_reg_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_reg(/); }
static inline void vm_execute_FDIV_mem_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_reg(/); }
static inline void vm_execute_FDIV_reg_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_mem(/); }
static inline void vm_execute_FDIV_mem_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_mem(/); }
static inline void vm_execute_FDIV_reg_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_imm(/); }
static inline void vm_execute_FDIV_mem_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_imm(/); }

static inline void vm_execute_FDIV(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG_REG: vm_execute_FDIV_reg_reg(vm); break;
  case OPCODE_PARAM_MEM_REG: vm_execute_FDIV_mem_reg(vm); break;
  case OPCODE_PARAM_REG_MEM: vm_execute_FDIV_reg_mem(vm); break;
  case OPCODE_PARAM_MEM_MEM: vm_execute_FDIV_mem_mem(vm); break;
  case OPCODE_PARAM_REG_IMM: vm_execute_FDIV_reg_imm(vm); break;
  case OPCODE_PARAM_MEM_IMM: vm_execute_FDIV_mem_imm(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_FMOD_reg_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_reg(/); }
static inline void vm_execute_FMOD_mem_reg(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_reg(/); }
static inline void vm_execute_FMOD_reg_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_mem(/); }
static inline void vm_execute_FMOD_mem_mem(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_mem(/); }
static inline void vm_execute_FMOD_reg_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_reg_imm(/); }
static inline void vm_execute_FMOD_mem_imm(vm_t* vm) { template_impl_vm_execute_FLOAT_BIN_OP_mem_imm(/); }

static inline void vm_execute_FMOD(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG_REG: vm_execute_FMOD_reg_reg(vm); break;
  case OPCODE_PARAM_MEM_REG: vm_execute_FMOD_mem_reg(vm); break;
  case OPCODE_PARAM_REG_MEM: vm_execute_FMOD_reg_mem(vm); break;
  case OPCODE_PARAM_MEM_MEM: vm_execute_FMOD_mem_mem(vm); break;
  case OPCODE_PARAM_REG_IMM: vm_execute_FMOD_reg_imm(vm); break;
  case OPCODE_PARAM_MEM_IMM: vm_execute_FMOD_mem_imm(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_FNEG_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_32BIT: vm_register_f32_set(vm, reg_dest, -vm_register_f32_get(vm, reg_dest)); break;
  case OPCODE_WIDTH_64BIT: vm_register_f64_set(vm, reg_dest, -vm_register_f64_get(vm, reg_dest)); break;
  default: unreachable();
  }
}

static inline void vm_execute_FNEG_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_32BIT: vm_mem_f32_set(vm, mem_dest, -vm_mem_f32_get(vm, mem_dest)); break;
  case OPCODE_WIDTH_64BIT: vm_mem_f64_set(vm, mem_dest, -vm_mem_f64_get(vm, mem_dest)); break;
  default: unreachable();
  }
}

static inline void vm_execute_FNEG(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_FNEG_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_FNEG_mem(vm); break;
  default: unreachable();
  }
}

#define template_impl_vm_AND(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = (lhs & rhs) == 0;\
    vm->regs.FLAGS.negative = (lhs & rhs) >> (sizeof(lhs) * 8 - 1);\
    vm->regs.FLAGS.parity = (PARITY)(lhs & rhs);\
    return lhs & rhs;\
  } while (0)\

template_decls_vm_BIN_OP(AND, UNSIGNED)
template_decls_vm_execute_BIN_OP(AND, UNSIGNED)

#define template_impl_vm_OR(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = (lhs | rhs) == 0;\
    vm->regs.FLAGS.negative = (lhs | rhs) >> (sizeof(lhs) * 8 - 1);\
    vm->regs.FLAGS.parity = (PARITY)(lhs | rhs);\
    return lhs | rhs;\
  } while (0)\

template_decls_vm_BIN_OP(OR, UNSIGNED)
template_decls_vm_execute_BIN_OP(OR, UNSIGNED)

#define template_impl_vm_XOR(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = (lhs ^ rhs) == 0;\
    vm->regs.FLAGS.negative = (lhs ^ rhs) >> (sizeof(lhs) * 8 - 1);\
    vm->regs.FLAGS.parity = (PARITY)(lhs ^ rhs);\
    return lhs ^ rhs;\
  } while (0)\

template_decls_vm_BIN_OP(XOR, UNSIGNED)
template_decls_vm_execute_BIN_OP(XOR, UNSIGNED)

#define template_impl_vm_NOT(MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = value == (MAX);\
    vm->regs.FLAGS.parity = (PARITY)(~value);\
    return ~value;\
  } while (0)\

static inline uint8_t  vm_NOT8 (vm_t* vm, uint8_t  value) { template_impl_vm_NOT(UINT8_MAX,  vm_parity_u8 ); }
static inline uint16_t vm_NOT16(vm_t* vm, uint16_t value) { template_impl_vm_NOT(UINT16_MAX, vm_parity_u16); }
static inline uint32_t vm_NOT32(vm_t* vm, uint32_t value) { template_impl_vm_NOT(UINT32_MAX, vm_parity_u32); }
static inline uint64_t vm_NOT64(vm_t* vm, uint64_t value) { template_impl_vm_NOT(UINT64_MAX, vm_parity_u64); }

static inline void vm_execute_NOT_reg(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_register_u8_set (vm, reg_dest, vm_NOT8 (vm, vm_register_u8_get (vm, reg_dest))); break;
  case OPCODE_WIDTH_16BIT: vm_register_u16_set(vm, reg_dest, vm_NOT16(vm, vm_register_u16_get(vm, reg_dest))); break;
  case OPCODE_WIDTH_32BIT: vm_register_u32_set(vm, reg_dest, vm_NOT32(vm, vm_register_u32_get(vm, reg_dest))); break;
  case OPCODE_WIDTH_64BIT: vm_register_u64_set(vm, reg_dest, vm_NOT64(vm, vm_register_u64_get(vm, reg_dest))); break;
  default: unreachable();
  }
}

static inline void vm_execute_NOT_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_mem_u8_set (vm, mem_dest, vm_NOT8 (vm, vm_mem_u8_get (vm, mem_dest))); break;
  case OPCODE_WIDTH_16BIT: vm_mem_u16_set(vm, mem_dest, vm_NOT16(vm, vm_mem_u16_get(vm, mem_dest))); break;
  case OPCODE_WIDTH_32BIT: vm_mem_u32_set(vm, mem_dest, vm_NOT32(vm, vm_mem_u32_get(vm, mem_dest))); break;
  case OPCODE_WIDTH_64BIT: vm_mem_u64_set(vm, mem_dest, vm_NOT64(vm, vm_mem_u64_get(vm, mem_dest))); break;
  default: unreachable();
  }
}

static inline void vm_execute_NOT(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG: vm_execute_NOT_reg(vm); break;
  case OPCODE_PARAM_MEM: vm_execute_NOT_mem(vm); break;
  default: unreachable();
  }
}

#define template_impl_vm_SHL(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = (lhs << rhs) == 0;\
    vm->regs.FLAGS.negative = (lhs << rhs) >> (sizeof(lhs) * 8 - 1);\
    vm->regs.FLAGS.carry = lhs > (lhs << rhs);\
    vm->regs.FLAGS.parity = (PARITY)(lhs << rhs);\
    return lhs << rhs;\
  } while (0)\

template_decls_vm_BIN_OP(SHL, UNSIGNED)
template_decls_vm_execute_BIN_OP(SHL, UNSIGNED)

#define template_impl_vm_SHR(MIN, MAX, PARITY)\
  do {\
    vm->regs.FLAGS.zero = (lhs >> rhs) == 0;\
    vm->regs.FLAGS.negative = (lhs >> rhs) >> (sizeof(lhs) * 8 - 1);\
    vm->regs.FLAGS.parity = (PARITY)(lhs >> rhs);\
    return lhs >> rhs;\
  } while (0)\

template_decls_vm_BIN_OP(SHR, UNSIGNED)
template_decls_vm_execute_BIN_OP(SHR, UNSIGNED)

static inline void vm_execute_JMP(vm_t* vm)
{
  vm->regs.IP = vm_code_next_u64(vm);
}

static inline void vm_execute_JE(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.zero)
    vm->regs.IP = addr;
}

static inline void vm_execute_JNE(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (!vm->regs.FLAGS.zero)
    vm->regs.IP = addr;
}

static inline void vm_execute_JL(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.negative)
    vm->regs.IP = addr;
}

static inline void vm_execute_JLE(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.zero || vm->regs.FLAGS.negative)
    vm->regs.IP = addr;
}

static inline void vm_execute_JG(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (!vm->regs.FLAGS.zero && !vm->regs.FLAGS.negative)
    vm->regs.IP = addr;
}

static inline void vm_execute_JGE(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.zero || !vm->regs.FLAGS.negative)
    vm->regs.IP = addr;
}

static inline void vm_execute_JZ(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.zero)
    vm->regs.IP = addr;
}

static inline void vm_execute_JNZ(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (!vm->regs.FLAGS.zero)
    vm->regs.IP = addr;
}

static inline void vm_execute_JN(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.negative)
    vm->regs.IP = addr;
}

static inline void vm_execute_JNN(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (!vm->regs.FLAGS.negative)
    vm->regs.IP = addr;
}

static inline void vm_execute_JO(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.overflow)
    vm->regs.IP = addr;
}

static inline void vm_execute_JNO(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (!vm->regs.FLAGS.overflow)
    vm->regs.IP = addr;
}

static inline void vm_execute_JC(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.carry)
    vm->regs.IP = addr;
}

static inline void vm_execute_JNC(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (!vm->regs.FLAGS.carry)
    vm->regs.IP = addr;
}

static inline void vm_execute_JP(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (vm->regs.FLAGS.parity)
    vm->regs.IP = addr;
}

static inline void vm_execute_JNP(vm_t* vm)
{
  uint64_t addr = vm_code_next_u64(vm);

  if (!vm->regs.FLAGS.parity)
    vm->regs.IP = addr;
}

#define template_impl_vm_CMP()\
  do {\
    vm->regs.FLAGS.zero = lhs == rhs;\
    vm->regs.FLAGS.carry = lhs < rhs;\
  } while (0)\

static inline void vm_CMP8 (vm_t* vm, uint8_t  lhs, uint8_t  rhs) { template_impl_vm_CMP(); }
static inline void vm_CMP16(vm_t* vm, uint16_t lhs, uint16_t rhs) { template_impl_vm_CMP(); }
static inline void vm_CMP32(vm_t* vm, uint32_t lhs, uint32_t rhs) { template_impl_vm_CMP(); }
static inline void vm_CMP64(vm_t* vm, uint64_t lhs, uint64_t rhs) { template_impl_vm_CMP(); }

static inline void vm_execute_CMP_reg_reg(vm_t* vm)
{
  uint8_t encoded_regs = vm_code_next_u8(vm);
  register_t reg_dest = register_decode((encoded_regs >> 4) & 0xF, vm->inst.width);
  register_t reg_src = register_decode(encoded_regs & 0xF, vm->inst.width);
  
  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_CMP8 (vm, vm_register_u8_get (vm, reg_dest), vm_register_u8_get (vm, reg_src)); break;
  case OPCODE_WIDTH_16BIT: vm_CMP16(vm, vm_register_u16_get(vm, reg_dest), vm_register_u16_get(vm, reg_src)); break;
  case OPCODE_WIDTH_32BIT: vm_CMP32(vm, vm_register_u32_get(vm, reg_dest), vm_register_u32_get(vm, reg_src)); break;
  case OPCODE_WIDTH_64BIT: vm_CMP64(vm, vm_register_u64_get(vm, reg_dest), vm_register_u64_get(vm, reg_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_CMP_mem_reg(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_src = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_CMP8 (vm, vm_mem_u8_get (vm, mem_dest), vm_register_u8_get (vm, reg_src)); break;
  case OPCODE_WIDTH_16BIT: vm_CMP16(vm, vm_mem_u16_get(vm, mem_dest), vm_register_u16_get(vm, reg_src)); break;
  case OPCODE_WIDTH_32BIT: vm_CMP32(vm, vm_mem_u32_get(vm, mem_dest), vm_register_u32_get(vm, reg_src)); break;
  case OPCODE_WIDTH_64BIT: vm_CMP64(vm, vm_mem_u64_get(vm, mem_dest), vm_register_u64_get(vm, reg_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_CMP_reg_mem(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  void* mem_src = vm_code_next_addr(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_CMP8 (vm, vm_register_u8_get (vm, reg_dest), vm_mem_u8_get (vm, mem_src)); break;
  case OPCODE_WIDTH_16BIT: vm_CMP16(vm, vm_register_u16_get(vm, reg_dest), vm_mem_u16_get(vm, mem_src)); break;
  case OPCODE_WIDTH_32BIT: vm_CMP32(vm, vm_register_u32_get(vm, reg_dest), vm_mem_u32_get(vm, mem_src)); break;
  case OPCODE_WIDTH_64BIT: vm_CMP64(vm, vm_register_u64_get(vm, reg_dest), vm_mem_u64_get(vm, mem_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_CMP_mem_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);
  void* mem_src = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_CMP8 (vm, vm_mem_u8_get (vm, mem_dest), vm_mem_u8_get (vm, mem_src)); break;
  case OPCODE_WIDTH_16BIT: vm_CMP16(vm, vm_mem_u16_get(vm, mem_dest), vm_mem_u16_get(vm, mem_src)); break;
  case OPCODE_WIDTH_32BIT: vm_CMP32(vm, vm_mem_u32_get(vm, mem_dest), vm_mem_u32_get(vm, mem_src)); break;
  case OPCODE_WIDTH_64BIT: vm_CMP64(vm, vm_mem_u64_get(vm, mem_dest), vm_mem_u64_get(vm, mem_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_CMP_reg_imm(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_CMP8 (vm, vm_register_u8_get (vm, reg_dest), vm_code_next_u8 (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_CMP16(vm, vm_register_u16_get(vm, reg_dest), vm_code_next_u16(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_CMP32(vm, vm_register_u32_get(vm, reg_dest), vm_code_next_u32(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_CMP64(vm, vm_register_u64_get(vm, reg_dest), vm_code_next_u64(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_CMP_mem_imm(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_CMP8 (vm, vm_mem_u8_get (vm, mem_dest), vm_code_next_u8 (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_CMP16(vm, vm_mem_u16_get(vm, mem_dest), vm_code_next_u16(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_CMP32(vm, vm_mem_u32_get(vm, mem_dest), vm_code_next_u32(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_CMP64(vm, vm_mem_u64_get(vm, mem_dest), vm_code_next_u64(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_CMP(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG_REG: vm_execute_CMP_reg_reg(vm); break;
  case OPCODE_PARAM_MEM_REG: vm_execute_CMP_mem_reg(vm); break;
  case OPCODE_PARAM_REG_MEM: vm_execute_CMP_reg_mem(vm); break;
  case OPCODE_PARAM_MEM_MEM: vm_execute_CMP_mem_mem(vm); break;
  case OPCODE_PARAM_REG_IMM: vm_execute_CMP_reg_imm(vm); break;
  case OPCODE_PARAM_MEM_IMM: vm_execute_CMP_mem_imm(vm); break;
  default: unreachable();
  }
}

#define template_impl_vm_ICMP(MIN, MAX)\
  do {\
    vm->regs.FLAGS.zero = lhs == rhs;\
    vm->regs.FLAGS.negative = lhs - rhs < 0;\
    vm->regs.FLAGS.overflow = (rhs > 0 && lhs < (MIN) + rhs) || (rhs < 0 && lhs > (MAX) + rhs);\
    vm->regs.FLAGS.carry = lhs < rhs;\
  } while (0)\

static inline void vm_ICMP8 (vm_t* vm, int8_t  lhs, int8_t  rhs) { template_impl_vm_ICMP(INT8_MIN,  INT8_MAX ); }
static inline void vm_ICMP16(vm_t* vm, int16_t lhs, int16_t rhs) { template_impl_vm_ICMP(INT16_MIN, INT16_MAX); }
static inline void vm_ICMP32(vm_t* vm, int32_t lhs, int32_t rhs) { template_impl_vm_ICMP(INT32_MIN, INT32_MAX); }
static inline void vm_ICMP64(vm_t* vm, int64_t lhs, int64_t rhs) { template_impl_vm_ICMP(INT64_MIN, INT64_MAX); }

static inline void vm_execute_ICMP_reg_reg(vm_t* vm)
{
  uint8_t encoded_regs = vm_code_next_u8(vm);
  register_t reg_dest = register_decode((encoded_regs >> 4) & 0xF, vm->inst.width);
  register_t reg_src = register_decode(encoded_regs & 0xF, vm->inst.width);
  
  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_ICMP8 (vm, vm_register_i8_get (vm, reg_dest), vm_register_i8_get (vm, reg_src)); break;
  case OPCODE_WIDTH_16BIT: vm_ICMP16(vm, vm_register_i16_get(vm, reg_dest), vm_register_i16_get(vm, reg_src)); break;
  case OPCODE_WIDTH_32BIT: vm_ICMP32(vm, vm_register_i32_get(vm, reg_dest), vm_register_i32_get(vm, reg_src)); break;
  case OPCODE_WIDTH_64BIT: vm_ICMP64(vm, vm_register_i64_get(vm, reg_dest), vm_register_i64_get(vm, reg_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_ICMP_mem_reg(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_src = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_ICMP8 (vm, vm_mem_i8_get (vm, mem_dest), vm_register_i8_get (vm, reg_src)); break;
  case OPCODE_WIDTH_16BIT: vm_ICMP16(vm, vm_mem_i16_get(vm, mem_dest), vm_register_i16_get(vm, reg_src)); break;
  case OPCODE_WIDTH_32BIT: vm_ICMP32(vm, vm_mem_i32_get(vm, mem_dest), vm_register_i32_get(vm, reg_src)); break;
  case OPCODE_WIDTH_64BIT: vm_ICMP64(vm, vm_mem_i64_get(vm, mem_dest), vm_register_i64_get(vm, reg_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_ICMP_reg_mem(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  void* mem_src = vm_code_next_addr(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_ICMP8 (vm, vm_register_i8_get (vm, reg_dest), vm_mem_i8_get (vm, mem_src)); break;
  case OPCODE_WIDTH_16BIT: vm_ICMP16(vm, vm_register_i16_get(vm, reg_dest), vm_mem_i16_get(vm, mem_src)); break;
  case OPCODE_WIDTH_32BIT: vm_ICMP32(vm, vm_register_i32_get(vm, reg_dest), vm_mem_i32_get(vm, mem_src)); break;
  case OPCODE_WIDTH_64BIT: vm_ICMP64(vm, vm_register_i64_get(vm, reg_dest), vm_mem_i64_get(vm, mem_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_ICMP_mem_mem(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);
  void* mem_src = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_ICMP8 (vm, vm_mem_i8_get (vm, mem_dest), vm_mem_i8_get (vm, mem_src)); break;
  case OPCODE_WIDTH_16BIT: vm_ICMP16(vm, vm_mem_i16_get(vm, mem_dest), vm_mem_i16_get(vm, mem_src)); break;
  case OPCODE_WIDTH_32BIT: vm_ICMP32(vm, vm_mem_i32_get(vm, mem_dest), vm_mem_i32_get(vm, mem_src)); break;
  case OPCODE_WIDTH_64BIT: vm_ICMP64(vm, vm_mem_i64_get(vm, mem_dest), vm_mem_i64_get(vm, mem_src)); break;
  default: unreachable();
  }
}

static inline void vm_execute_ICMP_reg_imm(vm_t* vm)
{
  uint8_t encoded_reg = vm_code_next_u8(vm);
  register_t reg_dest = register_decode(encoded_reg & 0xF, vm->inst.width);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_ICMP8 (vm, vm_register_i8_get (vm, reg_dest), vm_code_next_i8 (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_ICMP16(vm, vm_register_i16_get(vm, reg_dest), vm_code_next_i16(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_ICMP32(vm, vm_register_i32_get(vm, reg_dest), vm_code_next_i32(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_ICMP64(vm, vm_register_i64_get(vm, reg_dest), vm_code_next_i64(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_ICMP_mem_imm(vm_t* vm)
{
  void* mem_dest = vm_code_next_addr(vm);

  switch (vm->inst.width)
  {
  case OPCODE_WIDTH_8BIT:  vm_ICMP8 (vm, vm_mem_i8_get (vm, mem_dest), vm_code_next_i8 (vm)); break;
  case OPCODE_WIDTH_16BIT: vm_ICMP16(vm, vm_mem_i16_get(vm, mem_dest), vm_code_next_i16(vm)); break;
  case OPCODE_WIDTH_32BIT: vm_ICMP32(vm, vm_mem_i32_get(vm, mem_dest), vm_code_next_i32(vm)); break;
  case OPCODE_WIDTH_64BIT: vm_ICMP64(vm, vm_mem_i64_get(vm, mem_dest), vm_code_next_i64(vm)); break;
  default: unreachable();
  }
}

static inline void vm_execute_ICMP(vm_t* vm)
{
  switch (vm->inst.param)
  {
  case OPCODE_PARAM_REG_REG: vm_execute_ICMP_reg_reg(vm); break;
  case OPCODE_PARAM_MEM_REG: vm_execute_ICMP_mem_reg(vm); break;
  case OPCODE_PARAM_REG_MEM: vm_execute_ICMP_reg_mem(vm); break;
  case OPCODE_PARAM_MEM_MEM: vm_execute_ICMP_mem_mem(vm); break;
  case OPCODE_PARAM_REG_IMM: vm_execute_ICMP_reg_imm(vm); break;
  case OPCODE_PARAM_MEM_IMM: vm_execute_ICMP_mem_imm(vm); break;
  default: unreachable();
  }
}

static inline void vm_execute_CLF(vm_t* vm)
{
  assert(vm->inst.param == OPCODE_PARAM_NONE);
  assert(vm->inst.width == OPCODE_WIDTH_NONE);

  memset(&vm->regs.FLAGS, 0, sizeof(vm->regs.FLAGS));
}

static inline void vm_execute_CALL(vm_t* vm)
{
  assert(vm->inst.param == OPCODE_PARAM_LABEL);
  assert(vm->inst.width == OPCODE_WIDTH_64BIT);

  uint64_t addr = vm_code_next_u64(vm);
  vm_stack_u64_push(vm, vm->regs.IP);
  vm->regs.IP = addr;
}

static inline void vm_execute_RET(vm_t* vm)
{
  assert(vm->inst.param == OPCODE_PARAM_NONE);
  assert(vm->inst.width == OPCODE_WIDTH_NONE);

  vm->regs.IP = vm_stack_u64_pop(vm);
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
  case OPCODE_ADD:  vm_execute_ADD(vm); break;
  case OPCODE_SUB:  vm_execute_SUB(vm); break;
  case OPCODE_MUL:  vm_execute_MUL(vm); break;
  case OPCODE_DIV:  vm_execute_DIV(vm); break;
  case OPCODE_MOD:  vm_execute_MOD(vm); break;
  case OPCODE_INC:  vm_execute_INC(vm); break;
  case OPCODE_DEC:  vm_execute_DEC(vm); break;
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
  case OPCODE_JMP:  vm_execute_JMP(vm); break;
  case OPCODE_JE:   vm_execute_JE(vm); break;
  case OPCODE_JNE:  vm_execute_JNE(vm); break;
  case OPCODE_JL:   vm_execute_JL(vm); break;
  case OPCODE_JLE:  vm_execute_JLE(vm); break;
  case OPCODE_JG:   vm_execute_JG(vm); break;
  case OPCODE_JGE:  vm_execute_JGE(vm); break;
  case OPCODE_JZ:   vm_execute_JZ(vm); break;
  case OPCODE_JNZ:  vm_execute_JNZ(vm); break;
  case OPCODE_JN:   vm_execute_JN(vm); break;
  case OPCODE_JNN:  vm_execute_JNN(vm); break;
  case OPCODE_JO:   vm_execute_JO(vm); break;
  case OPCODE_JNO:  vm_execute_JNO(vm); break;
  case OPCODE_JC:   vm_execute_JC(vm); break;
  case OPCODE_JNC:  vm_execute_JNC(vm); break;
  case OPCODE_JP:   vm_execute_JP(vm); break;
  case OPCODE_JNP:  vm_execute_JNP(vm); break;
  case OPCODE_CMP:  vm_execute_CMP(vm); break;
  case OPCODE_ICMP: vm_execute_ICMP(vm); break;
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
