#include "tasm.h"

#include <string.h>

#include "util.h"

#include "register.h"
#include "opcode.h"

#define template_impl_tasm_write_primitive()\
  do {\
    memcpy(dest, &value, sizeof(value));\
    return sizeof(value);\
  } while (0)\

size_t tasm_write_u8 (void* dest, uint8_t  value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_u16(void* dest, uint16_t value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_u32(void* dest, uint32_t value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_u64(void* dest, uint64_t value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_i8 (void* dest, int8_t   value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_i16(void* dest, int16_t  value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_i32(void* dest, int32_t  value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_i64(void* dest, int64_t  value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_f32(void* dest, float    value) { template_impl_tasm_write_primitive(); }
size_t tasm_write_f64(void* dest, double   value) { template_impl_tasm_write_primitive(); }

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
  return tasm_addr_encode(dest, mode, base, index, scale, offset);
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

static inline size_t tasm_addr_encode_offset(void* dest, int64_t offset)
{
  *(uint8_t*)dest = ((uint8_t)ADDR_MODE_OFFSET & 0xF) << 4;
  
  memcpy((uint8_t*)dest + 1, &offset, sizeof(int64_t));

  return sizeof(uint8_t) + sizeof(int64_t);
}

static inline size_t tasm_addr_encode_base(void* dest, register_t base)
{
  assert(register_bits(base) == 64);

  *(uint8_t*)dest = (((uint8_t)ADDR_MODE_BASE & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  return sizeof(uint8_t);
}

static inline size_t tasm_addr_encode_base_offset(void* dest, register_t base, int64_t offset)
{
  assert(register_bits(base) == 64);

  *(uint8_t*)dest = (((uint8_t)ADDR_MODE_BASE_OFFSET & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  memcpy((uint8_t*)dest + 1, &offset, sizeof(int64_t));

  return sizeof(uint8_t) + sizeof(int64_t);
}

static inline size_t tasm_addr_encode_base_index(void* dest, register_t base, register_t index)
{
  assert(register_bits(base) == 64);
  assert(register_bits(index) == 64);

  *(uint8_t*)dest = (((uint8_t)ADDR_MODE_BASE_INDEX & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  *((uint8_t*)dest + 1) = (register_encode(index) & 0xF) << 4;

  return 2 * sizeof(uint8_t);
}

static inline size_t tasm_addr_encode_base_index_offset(void* dest, register_t base, register_t index, int64_t offset)
{
  assert(register_bits(base) == 64);
  assert(register_bits(index) == 64);

  *(uint8_t*)dest = (((uint8_t)ADDR_MODE_BASE_INDEX_OFFSET & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  *((uint8_t*)dest + 1) = (register_encode(index) & 0xF) << 4;

  memcpy((uint8_t*)dest + 2, &offset, sizeof(int64_t));

  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

static inline size_t tasm_addr_encode_base_index_scale(void* dest, register_t base, register_t index, int32_t scale)
{
  assert(register_bits(base) == 64);
  assert(register_bits(index) == 64);

  *(uint8_t*)dest = (((uint8_t)ADDR_MODE_BASE_INDEX_SCALE & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  *((uint8_t*)dest + 1) = ((register_encode(index) & 0xF) << 4) |
                         (encode_scale(scale) & 0xF);

  return 2 * sizeof(uint8_t);
}

static inline size_t tasm_addr_encode_index_scale_offset(void* dest, register_t index, int32_t scale, int64_t offset)
{
  assert(register_bits(index) == 64);

  *(uint8_t*)dest = (((uint8_t)ADDR_MODE_INDEX_SCALE_OFFSET & 0xF) << 4);

  *((uint8_t*)dest + 1) = ((register_encode(index) & 0xF) << 4) |
                         (encode_scale(scale) & 0xF);

  memcpy((uint8_t*)dest + 2, &offset, sizeof(int64_t));

  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

static inline size_t tasm_addr_encode_base_index_scale_offset(void* dest, register_t base, register_t index, int32_t scale, int64_t offset)
{
  assert(register_bits(base) == 64);
  assert(register_bits(index) == 64);

  *(uint8_t*)dest = (((uint8_t)ADDR_MODE_BASE_INDEX_SCALE_OFFSET & 0xF) << 4) |
                   (register_encode(base) & 0xF);

  *((uint8_t*)dest + 1) = ((register_encode(index) & 0xF) << 4) |
                         (encode_scale(scale) & 0xF);

  memcpy((uint8_t*)dest + 2, &offset, sizeof(int64_t));

  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

size_t tasm_addr_encode(void* dest, addr_mode_t mode, register_t base, register_t index, int32_t scale, int64_t offset)
{
  switch (mode)
  {
  case ADDR_MODE_OFFSET:                  return tasm_addr_encode_offset(                 dest,                     offset); break;
  case ADDR_MODE_BASE:                    return tasm_addr_encode_base(                   dest, base                      ); break;
  case ADDR_MODE_BASE_OFFSET:             return tasm_addr_encode_base_offset(            dest, base,               offset); break;
  case ADDR_MODE_BASE_INDEX:              return tasm_addr_encode_base_index(             dest, base, index               ); break;
  case ADDR_MODE_BASE_INDEX_OFFSET:       return tasm_addr_encode_base_index_offset(      dest, base, index,        offset); break;
  case ADDR_MODE_BASE_INDEX_SCALE:        return tasm_addr_encode_base_index_scale(       dest, base, index, scale        ); break;
  case ADDR_MODE_INDEX_SCALE_OFFSET:      return tasm_addr_encode_index_scale_offset(     dest,       index, scale, offset); break;
  case ADDR_MODE_BASE_INDEX_SCALE_OFFSET: return tasm_addr_encode_base_index_scale_offset(dest, base, index, scale, offset); break;
  default: unreachable();
  }

  return 0;
}

static inline size_t tasm_addr_decode_offset(const void* src, int64_t* offset)
{
  if (offset != NULL)
    memcpy(offset, (const uint8_t*)src + 1, sizeof(int64_t));

  return sizeof(uint8_t) + sizeof(int64_t);
}

static inline size_t tasm_addr_decode_base(const void* src, register_t* base)
{
  if (base != NULL)
    *base = register_decode((*(const uint8_t*)src) & 0xF, OPCODE_WIDTH_64BIT);

  return sizeof(uint8_t);
}

static inline size_t tasm_addr_decode_base_offset(const void* src, register_t* base, int64_t* offset)
{
  if (base != NULL)
    *base = register_decode((*(const uint8_t*)src) & 0xF, OPCODE_WIDTH_64BIT);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)src + 1, sizeof(int64_t));

  return sizeof(uint8_t) + sizeof(int64_t);
}

static inline size_t tasm_addr_decode_base_index(const void* src, register_t* base, register_t* index)
{
  if (base != NULL)
    *base = register_decode((*(const uint8_t*)src) & 0xF, OPCODE_WIDTH_64BIT);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)src + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  return 2 * sizeof(uint8_t);
}

static inline size_t tasm_addr_decode_base_index_offset(const void* src, register_t* base, register_t* index, int64_t* offset)
{
  if (base != NULL)
    *base = register_decode((*(const uint8_t*)src) & 0xF, OPCODE_WIDTH_64BIT);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)src + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)src + 2, sizeof(int64_t));

  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

static inline size_t tasm_addr_decode_base_index_scale(const void* src, register_t* base, register_t* index, int32_t* scale)
{
  if (base != NULL)
    *base = register_decode((*(const uint8_t*)src) & 0xF, OPCODE_WIDTH_64BIT);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)src + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  if (scale != NULL)
    *scale = decode_scale((*((const uint8_t*)src + 1)) & 0xF);

  return 2 * sizeof(uint8_t);
}

static inline size_t tasm_addr_decode_index_scale_offset(const void* src, register_t* index, int32_t* scale, int64_t* offset)
{
  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)src + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  if (scale != NULL)
    *scale = decode_scale((*((const uint8_t*)src + 1)) & 0xF);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)src + 2, sizeof(int64_t));
  
  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

static inline size_t tasm_addr_decode_base_index_scale_offset(const void* src, register_t* base, register_t* index, int32_t* scale, int64_t* offset)
{
  if (base != NULL)
    *base = register_decode((*(const uint8_t*)src) & 0xF, OPCODE_WIDTH_64BIT);

  if (index != NULL)
    *index = register_decode(((*((const uint8_t*)src + 1)) >> 4) & 0xF, OPCODE_WIDTH_64BIT);

  if (scale != NULL)
    *scale = decode_scale((*((const uint8_t*)src + 1)) & 0xF);

  if (offset != NULL)
    memcpy(offset, (const uint8_t*)src + 2, sizeof(int64_t));
  
  return 2 * sizeof(uint8_t) + sizeof(int64_t);
}

size_t tasm_addr_decode(const void* src, addr_mode_t* mode, register_t* base, register_t* index, int32_t* scale, int64_t* offset)
{
  addr_mode_t mode_value = (*(const uint8_t*)src) >> 4;

  if (mode != NULL)
    *mode = mode_value;

  switch (mode_value)
  {
  case ADDR_MODE_OFFSET:                  return tasm_addr_decode_offset(                 src,                     offset); break;
  case ADDR_MODE_BASE:                    return tasm_addr_decode_base(                   src, base                      ); break;
  case ADDR_MODE_BASE_OFFSET:             return tasm_addr_decode_base_offset(            src, base,               offset); break;
  case ADDR_MODE_BASE_INDEX:              return tasm_addr_decode_base_index(             src, base, index               ); break;
  case ADDR_MODE_BASE_INDEX_OFFSET:       return tasm_addr_decode_base_index_offset(      src, base, index,        offset); break;
  case ADDR_MODE_BASE_INDEX_SCALE:        return tasm_addr_decode_base_index_scale(       src, base, index, scale        ); break;
  case ADDR_MODE_INDEX_SCALE_OFFSET:      return tasm_addr_decode_index_scale_offset(     src,       index, scale, offset); break;
  case ADDR_MODE_BASE_INDEX_SCALE_OFFSET: return tasm_addr_decode_base_index_scale_offset(src, base, index, scale, offset); break;
  default: unreachable();
  }

  return 0;
}
