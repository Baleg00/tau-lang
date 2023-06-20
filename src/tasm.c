#include "tasm.h"

#include <string.h>

#include "register.h"
#include "opcode.h"
#include "vm.h"

#define tasm_write_primitive_impl()\
  do {\
    memcpy(dest, &value, sizeof(value));\
    return sizeof(value);\
  } while (0)\

size_t tasm_write_u8 (void* dest, uint8_t  value) { tasm_write_primitive_impl(); }
size_t tasm_write_u16(void* dest, uint16_t value) { tasm_write_primitive_impl(); }
size_t tasm_write_u32(void* dest, uint32_t value) { tasm_write_primitive_impl(); }
size_t tasm_write_u64(void* dest, uint64_t value) { tasm_write_primitive_impl(); }
size_t tasm_write_i8 (void* dest, int8_t   value) { tasm_write_primitive_impl(); }
size_t tasm_write_i16(void* dest, int16_t  value) { tasm_write_primitive_impl(); }
size_t tasm_write_i32(void* dest, int32_t  value) { tasm_write_primitive_impl(); }
size_t tasm_write_i64(void* dest, int64_t  value) { tasm_write_primitive_impl(); }
size_t tasm_write_f32(void* dest, float    value) { tasm_write_primitive_impl(); }
size_t tasm_write_f64(void* dest, double   value) { tasm_write_primitive_impl(); }

size_t tasm_write_register(void* dest, register_t reg)
{
  *(uint8_t*)dest = register_encode(reg);

  return sizeof(uint8_t);
}

size_t tasm_write_registers(void* dest, register_t reg1, register_t reg2)
{
  *(uint8_t*)dest = ((register_encode(reg1) & 0xFF) << 4) |
                    (register_encode(reg2) & 0xF);

  return sizeof(uint8_t);
}

size_t tasm_write_opcode(void* dest, opcode_t opcode, opcode_param_t param, opcode_width_t width)
{
  uint16_t encoded_opcode = opcode_encode(opcode, param, width);
  memcpy(dest, &encoded_opcode, sizeof(encoded_opcode));

  return sizeof(uint16_t);
}

size_t tasm_write_addr(void* dest, addr_mode_t mode, register_t base, register_t index, int32_t scale, int64_t offset)
{
  return vm_addr_encode(dest, mode, base, index, scale, offset);
}
