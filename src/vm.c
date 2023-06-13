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

  vm->regs.SP = (uint64_t)vm->stack.data;
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

uint8_t vm_code_next_u8(vm_t* vm)
{
  uint8_t value = vm_mem_u8_get(vm, vm->code.data + vm->regs.IP);
  vm->regs.IP += sizeof(uint8_t);
  return value;
}

uint16_t vm_code_next_u16(vm_t* vm)
{
  uint16_t value = vm_mem_u16_get(vm, vm->code.data + vm->regs.IP);
  vm->regs.IP += sizeof(uint16_t);
  return value;
}

uint32_t vm_code_next_u32(vm_t* vm)
{
  uint32_t value = vm_mem_u32_get(vm, vm->code.data + vm->regs.IP);
  vm->regs.IP += sizeof(uint32_t);
  return value;
}

uint64_t vm_code_next_u64(vm_t* vm)
{
  uint64_t value = vm_mem_u64_get(vm, vm->code.data + vm->regs.IP);
  vm->regs.IP += sizeof(uint64_t);
  return value;
}

float vm_code_next_f32(vm_t* vm)
{
  float value = vm_mem_f32_get(vm, vm->code.data + vm->regs.IP);
  vm->regs.IP += sizeof(float);
  return value;
}

double vm_code_next_f64(vm_t* vm)
{
  double value = vm_mem_f64_get(vm, vm->code.data + vm->regs.IP);
  vm->regs.IP += sizeof(double);
  return value;
}

uint8_t vm_mem_u8_get(vm_t* vm, const void* mem)
{
  unused(vm);

  return *(uint8_t*)mem;
}

uint16_t vm_mem_u16_get(vm_t* vm, const void* mem)
{
  unused(vm);

  uint16_t value;
  memcpy(&value, mem, sizeof(uint16_t));
  return value;
}

uint32_t vm_mem_u32_get(vm_t* vm, const void* mem)
{
  unused(vm);

  uint32_t value;
  memcpy(&value, mem, sizeof(uint32_t));
  return value;
}

uint64_t vm_mem_u64_get(vm_t* vm, const void* mem)
{
  unused(vm);

  uint64_t value;
  memcpy(&value, mem, sizeof(uint64_t));
  return value;
}

float vm_mem_f32_get(vm_t* vm, const void* mem)
{
  unused(vm);

  float value;
  memcpy(&value, mem, sizeof(float));
  return value;
}

double vm_mem_f64_get(vm_t* vm, const void* mem)
{
  unused(vm);

  double value;
  memcpy(&value, mem, sizeof(double));
  return value;
}

void vm_mem_u8_set(vm_t* vm, void* mem, uint8_t value)
{
  unused(vm);

  *(uint8_t*)mem = value;
}

void vm_mem_u16_set(vm_t* vm, void* mem, uint16_t value)
{
  unused(vm);

  memcpy(mem, &value, sizeof(uint16_t));
}

void vm_mem_u32_set(vm_t* vm, void* mem, uint32_t value)
{
  unused(vm);

  memcpy(mem, &value, sizeof(uint32_t));
}

void vm_mem_u64_set(vm_t* vm, void* mem, uint64_t value)
{
  unused(vm);

  memcpy(mem, &value, sizeof(uint64_t));
}

void vm_mem_f32_set(vm_t* vm, void* mem, float value)
{
  unused(vm);

  memcpy(mem, &value, sizeof(float));
}

void vm_mem_f64_set(vm_t* vm, void* mem, double value)
{
  unused(vm);

  memcpy(mem, &value, sizeof(double));
}

static inline uint8_t encode_scale(int32_t scale)
{
  // Highest bit represents sign. (0 - positive, 1 - negative)
  // Lowest 3 bits represent power of 2.
  switch (scale)
  {
  case 2:    return            1;
  case 4:    return            2;
  case 8:    return            3;
  case 16:   return            4;
  case 32:   return            5;
  case 64:   return            6;
  case 128:  return            7;
  case -2:   return (1 << 3) | 1;
  case -4:   return (1 << 3) | 2;
  case -8:   return (1 << 3) | 3;
  case -16:  return (1 << 3) | 4;
  case -32:  return (1 << 3) | 5;
  case -64:  return (1 << 3) | 6;
  case -128: return (1 << 3) | 7;
  default: unreachable();
  }

  return 0;
}

static inline int32_t decode_scale(uint8_t value)
{
  switch (value)
  {
  case            1: return 2;
  case            2: return 4;
  case            3: return 8;
  case            4: return 16;
  case            5: return 32;
  case            6: return 64;
  case            7: return 128;
  case (1 << 3) | 1: return -2;
  case (1 << 3) | 2: return -4;
  case (1 << 3) | 3: return -8;
  case (1 << 3) | 4: return -16;
  case (1 << 3) | 5: return -32;
  case (1 << 3) | 6: return -64;
  case (1 << 3) | 7: return -128;
  default: unreachable();
  }

  return 0;
}

static size_t vm_mem_addr_encode_offset(vm_t* vm, void* mem, int64_t offset)
{
  unused(vm);

  *(uint8_t*)mem = ((uint8_t)ADDR_MODE_OFFSET & 0xF) << 4;
  
  memcpy((uint8_t*)mem + 1, &offset, sizeof(int64_t));

  return sizeof(uint8_t) + sizeof(int64_t);
}

static size_t vm_mem_addr_encode_base(vm_t* vm, void* mem, register_t base)
{
  unused(vm);

  assert(register_bits(base) == 64);

  *(uint8_t*)mem = (((uint8_t)ADDR_MODE_BASE & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  return sizeof(uint8_t);
}

static size_t vm_mem_addr_encode_base_offset(vm_t* vm, void* mem, register_t base, int64_t offset)
{
  unused(vm);

  assert(register_bits(base) == 64);

  *(uint8_t*)mem = (((uint8_t)ADDR_MODE_BASE_OFFSET & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  memcpy((uint8_t*)mem + 1, &offset, sizeof(int64_t));

  return sizeof(uint8_t) + sizeof(int64_t);
}

static size_t vm_mem_addr_encode_base_index(vm_t* vm, void* mem, register_t base, register_t index)
{
  unused(vm);

  assert(register_bits(base) == 64);
  assert(register_bits(index) == 64);

  *(uint8_t*)mem = (((uint8_t)ADDR_MODE_BASE_INDEX & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  *((uint8_t*)mem + 1) = (register_encode(index) & 0xF) << 4;

  return 2 * sizeof(uint8_t);
}

static size_t vm_mem_addr_encode_base_index_offset(vm_t* vm, void* mem, register_t base, register_t index, int64_t offset)
{
  unused(vm);

  assert(register_bits(base) == 64);
  assert(register_bits(index) == 64);

  *(uint8_t*)mem = (((uint8_t)ADDR_MODE_BASE_INDEX_OFFSET & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  *((uint8_t*)mem + 1) = (register_encode(index) & 0xF) << 4;

  memcpy((uint8_t*)mem + 2, &offset, sizeof(int64_t));

  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

static size_t vm_mem_addr_encode_base_index_scale(vm_t* vm, void* mem, register_t base, register_t index, int32_t scale)
{
  unused(vm);

  assert(register_bits(base) == 64);
  assert(register_bits(index) == 64);

  *(uint8_t*)mem = (((uint8_t)ADDR_MODE_BASE_INDEX_SCALE & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  *((uint8_t*)mem + 1) = ((register_encode(index) & 0xF) << 4) |
                         (encode_scale(scale) & 0xF);

  return 2 * sizeof(uint8_t);
}

static size_t vm_mem_addr_encode_index_scale_offset(vm_t* vm, void* mem, register_t index, int32_t scale, int64_t offset)
{
  unused(vm);

  assert(register_bits(index) == 64);

  *(uint8_t*)mem = (((uint8_t)ADDR_MODE_INDEX_SCALE_OFFSET & 0xF) << 4);

  *((uint8_t*)mem + 1) = ((register_encode(index) & 0xF) << 4) |
                         (encode_scale(scale) & 0xF);

  memcpy((uint8_t*)mem + 2, &offset, sizeof(int64_t));

  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

static size_t vm_mem_addr_encode_base_index_scale_offset(vm_t* vm, void* mem, register_t base, register_t index, int32_t scale, int64_t offset)
{
  unused(vm);

  assert(register_bits(base) == 64);
  assert(register_bits(index) == 64);

  *(uint8_t*)mem = (((uint8_t)ADDR_MODE_BASE_INDEX_SCALE_OFFSET & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  *((uint8_t*)mem + 1) = ((register_encode(index) & 0xF) << 4) |
                         (encode_scale(scale) & 0xF);

  memcpy((uint8_t*)mem + 2, &offset, sizeof(int64_t));

  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

size_t vm_mem_addr_encode(vm_t* vm, void* mem, addr_mode_t mode, register_t base, register_t index, int32_t scale, int64_t offset)
{
  switch (mode)
  {
  case ADDR_MODE_OFFSET:                  return vm_mem_addr_encode_offset(                 vm, mem,                     offset); break;
  case ADDR_MODE_BASE:                    return vm_mem_addr_encode_base(                   vm, mem, base                      ); break;
  case ADDR_MODE_BASE_OFFSET:             return vm_mem_addr_encode_base_offset(            vm, mem, base,               offset); break;
  case ADDR_MODE_BASE_INDEX:              return vm_mem_addr_encode_base_index(             vm, mem, base, index               ); break;
  case ADDR_MODE_BASE_INDEX_OFFSET:       return vm_mem_addr_encode_base_index_offset(      vm, mem, base, index,        offset); break;
  case ADDR_MODE_BASE_INDEX_SCALE:        return vm_mem_addr_encode_base_index_scale(       vm, mem, base, index, scale        ); break;
  case ADDR_MODE_INDEX_SCALE_OFFSET:      return vm_mem_addr_encode_index_scale_offset(     vm, mem,       index, scale, offset); break;
  case ADDR_MODE_BASE_INDEX_SCALE_OFFSET: return vm_mem_addr_encode_base_index_scale_offset(vm, mem, base, index, scale, offset); break;
  default: unreachable();
  }

  return 0;
}

static size_t vm_mem_addr_decode_offset(vm_t* vm, const void* mem, int64_t* offset)
{
  unused(vm);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)mem + 1, sizeof(int64_t));

  return sizeof(uint8_t) + sizeof(int64_t);
}

static size_t vm_mem_addr_decode_base(vm_t* vm, const void* mem, register_t* base)
{
  unused(vm);

  if (base != NULL)
    *base = register_decode((*(const uint8_t*)mem) & 0xF, OPCODE_WIDTH_64BIT);

  return sizeof(uint8_t);
}

static size_t vm_mem_addr_decode_base_offset(vm_t* vm, const void* mem, register_t* base, int64_t* offset)
{
  unused(vm);
  
  if (base != NULL)
    *base = register_decode((*(const uint8_t*)mem) & 0xF, OPCODE_WIDTH_64BIT);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)mem + 1, sizeof(int64_t));

  return sizeof(uint8_t) + sizeof(int64_t);
}

static size_t vm_mem_addr_decode_base_index(vm_t* vm, const void* mem, register_t* base, register_t* index)
{
  unused(vm);

  if (base != NULL)
    *base = register_decode((*(const uint8_t*)mem) & 0xF, OPCODE_WIDTH_64BIT);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)mem + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  return 2 * sizeof(uint8_t);
}

static size_t vm_mem_addr_decode_base_index_offset(vm_t* vm, const void* mem, register_t* base, register_t* index, int64_t* offset)
{
  unused(vm);

  if (base != NULL)
    *base = register_decode((*(const uint8_t*)mem) & 0xF, OPCODE_WIDTH_64BIT);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)mem + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)mem + 2, sizeof(int64_t));

  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

static size_t vm_mem_addr_decode_base_index_scale(vm_t* vm, const void* mem, register_t* base, register_t* index, int32_t* scale)
{
  unused(vm);

  if (base != NULL)
    *base = register_decode((*(const uint8_t*)mem) & 0xF, OPCODE_WIDTH_64BIT);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)mem + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  if (scale != NULL)
    *scale = decode_scale((*((const uint8_t*)mem + 1)) & 0xF);

  return 2 * sizeof(uint8_t);
}

static size_t vm_mem_addr_decode_index_scale_offset(vm_t* vm, const void* mem, register_t* index, int32_t* scale, int64_t* offset)
{
  unused(vm);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)mem + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  if (scale != NULL)
    *scale = decode_scale((*((const uint8_t*)mem + 1)) & 0xF);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)mem + 2, sizeof(int64_t));
  
  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

static size_t vm_mem_addr_decode_base_index_scale_offset(vm_t* vm, const void* mem, register_t* base, register_t* index, int32_t* scale, int64_t* offset)
{
  unused(vm);

  if (base != NULL)
    *base = register_decode((*(const uint8_t*)mem) & 0xF, OPCODE_WIDTH_64BIT);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)mem + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  if (scale != NULL)
    *scale = decode_scale((*((const uint8_t*)mem + 1)) & 0xF);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)mem + 2, sizeof(int64_t));
  
  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

size_t vm_mem_addr_decode(vm_t* vm, const void* mem, addr_mode_t* mode, register_t* base, register_t* index, int32_t* scale, int64_t* offset)
{
  addr_mode_t mode_value = (*(const uint8_t*)mem) >> 4;

  if (mode != NULL)
    *mode = mode_value;

  switch (mode_value)
  {
  case ADDR_MODE_OFFSET:                  return vm_mem_addr_decode_offset(                 vm, mem,                     offset); break;
  case ADDR_MODE_BASE:                    return vm_mem_addr_decode_base(                   vm, mem, base                      ); break;
  case ADDR_MODE_BASE_OFFSET:             return vm_mem_addr_decode_base_offset(            vm, mem, base,               offset); break;
  case ADDR_MODE_BASE_INDEX:              return vm_mem_addr_decode_base_index(             vm, mem, base, index               ); break;
  case ADDR_MODE_BASE_INDEX_OFFSET:       return vm_mem_addr_decode_base_index_offset(      vm, mem, base, index,        offset); break;
  case ADDR_MODE_BASE_INDEX_SCALE:        return vm_mem_addr_decode_base_index_scale(       vm, mem, base, index, scale        ); break;
  case ADDR_MODE_INDEX_SCALE_OFFSET:      return vm_mem_addr_decode_index_scale_offset(     vm, mem,       index, scale, offset); break;
  case ADDR_MODE_BASE_INDEX_SCALE_OFFSET: return vm_mem_addr_decode_base_index_scale_offset(vm, mem, base, index, scale, offset); break;
  default: unreachable();
  }

  return 0;
}

void vm_stack_u8_push(vm_t* vm, uint8_t value)
{
  assert((uint8_t*)vm->regs.SP + sizeof(uint8_t) < vm->stack.data + vm->stack.size);
  memcpy((void*)vm->regs.SP, &value, sizeof(uint8_t));
  vm->regs.SP += sizeof(uint8_t);
}

void vm_stack_u16_push(vm_t* vm, uint16_t value)
{
  assert((uint8_t*)vm->regs.SP + sizeof(uint16_t) < vm->stack.data + vm->stack.size);
  memcpy((void*)vm->regs.SP, &value, sizeof(uint16_t));
  vm->regs.SP += sizeof(uint16_t);
}

void vm_stack_u32_push(vm_t* vm, uint32_t value)
{
  assert((uint8_t*)vm->regs.SP + sizeof(uint32_t) < vm->stack.data + vm->stack.size);
  memcpy((void*)vm->regs.SP, &value, sizeof(uint32_t));
  vm->regs.SP += sizeof(uint32_t);
}

void vm_stack_u64_push(vm_t* vm, uint64_t value)
{
  assert((uint8_t*)vm->regs.SP + sizeof(uint64_t) < vm->stack.data + vm->stack.size);
  memcpy((void*)vm->regs.SP, &value, sizeof(uint64_t));
  vm->regs.SP += sizeof(uint64_t);
}

void vm_stack_f32_push(vm_t* vm, float value)
{
  assert((uint8_t*)vm->regs.SP + sizeof(float) < vm->stack.data + vm->stack.size);
  memcpy((void*)vm->regs.SP, &value, sizeof(float));
  vm->regs.SP += sizeof(float);
}

void vm_stack_f64_push(vm_t* vm, double value)
{
  assert((uint8_t*)vm->regs.SP + sizeof(double) < vm->stack.data + vm->stack.size);
  memcpy((void*)vm->regs.SP, &value, sizeof(double));
  vm->regs.SP += sizeof(double);
}

uint8_t vm_stack_u8_pop(vm_t* vm)
{
  assert((uint8_t*)vm->regs.SP - sizeof(uint8_t) >= vm->stack.data);
  vm->regs.SP -= sizeof(uint8_t);
  uint8_t value;
  memcpy(&value, (void*)vm->regs.SP, sizeof(value));
  return value;
}

uint16_t vm_stack_u16_pop(vm_t* vm)
{
  assert((uint8_t*)vm->regs.SP - sizeof(uint16_t) >= vm->stack.data);
  vm->regs.SP -= sizeof(uint16_t);
  uint16_t value;
  memcpy(&value, (void*)vm->regs.SP, sizeof(value));
  return value;
}

uint32_t vm_stack_u32_pop(vm_t* vm)
{
  assert((uint8_t*)vm->regs.SP - sizeof(uint32_t) >= vm->stack.data);
  vm->regs.SP -= sizeof(uint32_t);
  uint32_t value;
  memcpy(&value, (void*)vm->regs.SP, sizeof(value));
  return value;
}

uint64_t vm_stack_u64_pop(vm_t* vm)
{
  assert((uint8_t*)vm->regs.SP - sizeof(uint64_t) >= vm->stack.data);
  vm->regs.SP -= sizeof(uint64_t);
  uint64_t value;
  memcpy(&value, (void*)vm->regs.SP, sizeof(value));
  return value;
}

float vm_stack_f32_pop(vm_t* vm)
{
  assert((uint8_t*)vm->regs.SP - sizeof(float) >= vm->stack.data);
  vm->regs.SP -= sizeof(float);
  float value;
  memcpy(&value, (void*)vm->regs.SP, sizeof(value));
  return value;
}

double vm_stack_f64_pop(vm_t* vm)
{
  assert((uint8_t*)vm->regs.SP - sizeof(double) >= vm->stack.data);
  vm->regs.SP -= sizeof(double);
  double value;
  memcpy(&value, (void*)vm->regs.SP, sizeof(value));
  return value;
}

void vm_fetch(vm_t* vm)
{
  vm->inst.raw = vm_code_next_u16(vm);
}

void vm_decode(vm_t* vm)
{
  opcode_decode(vm->inst.raw, &vm->inst.opcode, &vm->inst.param, &vm->inst.width);
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
