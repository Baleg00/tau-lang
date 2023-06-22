#ifndef TAU_VM_H
#define TAU_VM_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "typedefs.h"

struct vm_s
{
  struct
  {
    uint64_t A;  // 64-bit general purpose register A.
    uint64_t B;  // 64-bit general purpose register B.
    uint64_t C;  // 64-bit general purpose register C.
    uint64_t D;  // 64-bit general purpose register D.
    uint64_t E;  // 64-bit general purpose register E.
    uint64_t F;  // 64-bit general purpose register F.

    uint64_t SP; // 64-bit stack pointer register.
    uint64_t BP; // 64-bit base pointer register.
    uint64_t IP; // 64-bit instruction pointer register.
  
    // 8-bit status flags register.
    struct
    {
      uint8_t zero     : 1; // Indicates that the operation result was zero.
      uint8_t negative : 1; // Indicates that sign bit is set in the operation result.
      uint8_t overflow : 1; // Indicates an arithmetic overflow.
      uint8_t carry    : 1; // Indicates that the operation carried a bit.
      uint8_t parity   : 1; // Indicates an even or odd number of 1 bits in the operation result.
      uint8_t reserved : 3; // Reserved bits.
    } FLAGS;
  } regs;

  struct
  {
    size_t size; // Bytecode size in bytes.
    const uint8_t* data; // Pointer to bytecode.
  } code;

  struct
  {
    size_t size; // Stack size in bytes.
    uint8_t* data; // Pointer to stack.
  } stack;

  struct
  {
    uint16_t raw; // Raw bytecode instruction.

    opcode_t opcode; // Decoded opcode.
    opcode_param_t param; // Decoded parameter.
    opcode_width_t width; // Decoded bit width.
  } inst;
};

void vm_init(vm_t* vm, const uint8_t* code, size_t size);
void vm_free(vm_t* vm);

uint8_t vm_register_u8_get(vm_t* vm, register_t reg);
uint16_t vm_register_u16_get(vm_t* vm, register_t reg);
uint32_t vm_register_u32_get(vm_t* vm, register_t reg);
uint64_t vm_register_u64_get(vm_t* vm, register_t reg);
int8_t vm_register_i8_get(vm_t* vm, register_t reg);
int16_t vm_register_i16_get(vm_t* vm, register_t reg);
int32_t vm_register_i32_get(vm_t* vm, register_t reg);
int64_t vm_register_i64_get(vm_t* vm, register_t reg);
float vm_register_f32_get(vm_t* vm, register_t reg);
double vm_register_f64_get(vm_t* vm, register_t reg);

void vm_register_u8_set(vm_t* vm, register_t reg, uint8_t value);
void vm_register_u16_set(vm_t* vm, register_t reg, uint16_t value);
void vm_register_u32_set(vm_t* vm, register_t reg, uint32_t value);
void vm_register_u64_set(vm_t* vm, register_t reg, uint64_t value);
void vm_register_i8_set(vm_t* vm, register_t reg, int8_t value);
void vm_register_i16_set(vm_t* vm, register_t reg, int16_t value);
void vm_register_i32_set(vm_t* vm, register_t reg, int32_t value);
void vm_register_i64_set(vm_t* vm, register_t reg, int64_t value);
void vm_register_f32_set(vm_t* vm, register_t reg, float value);
void vm_register_f64_set(vm_t* vm, register_t reg, double value);

uint8_t vm_code_next_u8(vm_t* vm);
uint16_t vm_code_next_u16(vm_t* vm);
uint32_t vm_code_next_u32(vm_t* vm);
uint64_t vm_code_next_u64(vm_t* vm);
int8_t vm_code_next_i8(vm_t* vm);
int16_t vm_code_next_i16(vm_t* vm);
int32_t vm_code_next_i32(vm_t* vm);
int64_t vm_code_next_i64(vm_t* vm);
float vm_code_next_f32(vm_t* vm);
double vm_code_next_f64(vm_t* vm);
void* vm_code_next_addr(vm_t* vm);

uint8_t vm_mem_u8_get(vm_t* vm, const void* mem);
uint16_t vm_mem_u16_get(vm_t* vm, const void* mem);
uint32_t vm_mem_u32_get(vm_t* vm, const void* mem);
uint64_t vm_mem_u64_get(vm_t* vm, const void* mem);
int8_t vm_mem_i8_get(vm_t* vm, const void* mem);
int16_t vm_mem_i16_get(vm_t* vm, const void* mem);
int32_t vm_mem_i32_get(vm_t* vm, const void* mem);
int64_t vm_mem_i64_get(vm_t* vm, const void* mem);
float vm_mem_f32_get(vm_t* vm, const void* mem);
double vm_mem_f64_get(vm_t* vm, const void* mem);

void vm_mem_u8_set(vm_t* vm, void* mem, uint8_t value);
void vm_mem_u16_set(vm_t* vm, void* mem, uint16_t value);
void vm_mem_u32_set(vm_t* vm, void* mem, uint32_t value);
void vm_mem_u64_set(vm_t* vm, void* mem, uint64_t value);
void vm_mem_i8_set(vm_t* vm, void* mem, int8_t value);
void vm_mem_i16_set(vm_t* vm, void* mem, int16_t value);
void vm_mem_i32_set(vm_t* vm, void* mem, int32_t value);
void vm_mem_i64_set(vm_t* vm, void* mem, int64_t value);
void vm_mem_f32_set(vm_t* vm, void* mem, float value);
void vm_mem_f64_set(vm_t* vm, void* mem, double value);

void* vm_addr_effective(vm_t* vm, addr_mode_t mode, register_t base, register_t index, int32_t scale, int64_t offset);

void vm_stack_u8_push(vm_t* vm, uint8_t value);
void vm_stack_u16_push(vm_t* vm, uint16_t value);
void vm_stack_u32_push(vm_t* vm, uint32_t value);
void vm_stack_u64_push(vm_t* vm, uint64_t value);
void vm_stack_i8_push(vm_t* vm, int8_t value);
void vm_stack_i16_push(vm_t* vm, int16_t value);
void vm_stack_i32_push(vm_t* vm, int32_t value);
void vm_stack_i64_push(vm_t* vm, int64_t value);
void vm_stack_f32_push(vm_t* vm, float value);
void vm_stack_f64_push(vm_t* vm, double value);

uint8_t vm_stack_u8_pop(vm_t* vm);
uint16_t vm_stack_u16_pop(vm_t* vm);
uint32_t vm_stack_u32_pop(vm_t* vm);
uint64_t vm_stack_u64_pop(vm_t* vm);
int8_t vm_stack_i8_pop(vm_t* vm);
int16_t vm_stack_i16_pop(vm_t* vm);
int32_t vm_stack_i32_pop(vm_t* vm);
int64_t vm_stack_i64_pop(vm_t* vm);
float vm_stack_f32_pop(vm_t* vm);
double vm_stack_f64_pop(vm_t* vm);

void vm_fetch(vm_t* vm);
void vm_decode(vm_t* vm);
void vm_execute(vm_t* vm);

void vm_run(vm_t* vm);

#endif