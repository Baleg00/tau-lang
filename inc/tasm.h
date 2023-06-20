#ifndef TAU_TASM_H
#define TAU_TASM_H

#include <stdint.h>

#include "typedefs.h"

size_t tasm_write_u8(void* dest, uint8_t value);
size_t tasm_write_u16(void* dest, uint16_t value);
size_t tasm_write_u32(void* dest, uint32_t value);
size_t tasm_write_u64(void* dest, uint64_t value);
size_t tasm_write_i8(void* dest, int8_t value);
size_t tasm_write_i16(void* dest, int16_t value);
size_t tasm_write_i32(void* dest, int32_t value);
size_t tasm_write_i64(void* dest, int64_t value);
size_t tasm_write_f32(void* dest, float value);
size_t tasm_write_f64(void* dest, double value);

size_t tasm_write_register(void* dest, register_t reg);
size_t tasm_write_registers(void* dest, register_t reg1, register_t reg2);

size_t tasm_write_opcode(void* dest, opcode_t opcode, opcode_param_t param, opcode_width_t width);

size_t tasm_write_addr(void* dest, addr_mode_t mode, register_t base, register_t index, int32_t scale, int64_t offset);

#endif