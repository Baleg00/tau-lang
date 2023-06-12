#ifndef TAU_VM_H
#define TAU_VM_H

#include <stdint.h>

#include "typedefs.h"

struct vm_s
{
  struct
  {
    uint64_t A;
    uint64_t B;
    uint64_t C;
    uint64_t D;
    uint64_t E;
    uint64_t F;
    uint64_t SP;
    uint64_t BP;
    uint64_t IP;
  } regs;

  struct
  {
    uint8_t zero : 1;
    uint8_t negative : 1;
    uint8_t overflow : 1;
  } flags;

  struct
  {
    size_t size;
    const uint8_t* data;
  } code;

  struct
  {
    size_t size;
    uint8_t* data;
    ptrdiff_t offset;
  } stack;

  struct
  {
    uint16_t raw;

    opcode_t opcode;
    opcode_param_t param;
    opcode_width_t width;
  } inst;
};

void vm_init(vm_t* vm, const uint8_t* code, size_t size);
void vm_free(vm_t* vm);

uint8_t vm_register_u8_get(vm_t* vm, register_t reg);
uint16_t vm_register_u16_get(vm_t* vm, register_t reg);
uint32_t vm_register_u32_get(vm_t* vm, register_t reg);
uint64_t vm_register_u64_get(vm_t* vm, register_t reg);
float vm_register_f32_get(vm_t* vm, register_t reg);
double vm_register_f64_get(vm_t* vm, register_t reg);

void vm_register_u8_set(vm_t* vm, register_t reg, uint8_t value);
void vm_register_u16_set(vm_t* vm, register_t reg, uint16_t value);
void vm_register_u32_set(vm_t* vm, register_t reg, uint32_t value);
void vm_register_u64_set(vm_t* vm, register_t reg, uint64_t value);
void vm_register_f32_set(vm_t* vm, register_t reg, float value);
void vm_register_f64_set(vm_t* vm, register_t reg, double value);

uint8_t vm_code_next_u8(vm_t* vm);
uint16_t vm_code_next_u16(vm_t* vm);
uint32_t vm_code_next_u32(vm_t* vm);
uint64_t vm_code_next_u64(vm_t* vm);
float vm_code_next_f32(vm_t* vm);
double vm_code_next_f64(vm_t* vm);

uint8_t vm_mem_u8_get(vm_t* vm, const void* mem);
uint16_t vm_mem_u16_get(vm_t* vm, const void* mem);
uint32_t vm_mem_u32_get(vm_t* vm, const void* mem);
uint64_t vm_mem_u64_get(vm_t* vm, const void* mem);
float vm_mem_f32_get(vm_t* vm, const void* mem);
double vm_mem_f64_get(vm_t* vm, const void* mem);

void vm_mem_u8_set(vm_t* vm, void* mem, uint8_t value);
void vm_mem_u16_set(vm_t* vm, void* mem, uint16_t value);
void vm_mem_u32_set(vm_t* vm, void* mem, uint32_t value);
void vm_mem_u64_set(vm_t* vm, void* mem, uint64_t value);
void vm_mem_f32_set(vm_t* vm, void* mem, float value);
void vm_mem_f64_set(vm_t* vm, void* mem, double value);

void vm_stack_u8_push(vm_t* vm, uint8_t value);
void vm_stack_u16_push(vm_t* vm, uint16_t value);
void vm_stack_u32_push(vm_t* vm, uint32_t value);
void vm_stack_u64_push(vm_t* vm, uint64_t value);
void vm_stack_f32_push(vm_t* vm, float value);
void vm_stack_f64_push(vm_t* vm, double value);

uint8_t vm_stack_u8_pop(vm_t* vm);
uint16_t vm_stack_u16_pop(vm_t* vm);
uint32_t vm_stack_u32_pop(vm_t* vm);
uint64_t vm_stack_u64_pop(vm_t* vm);
float vm_stack_f32_pop(vm_t* vm);
double vm_stack_f64_pop(vm_t* vm);

void vm_fetch(vm_t* vm);
void vm_decode(vm_t* vm);
void vm_execute(vm_t* vm);

void vm_run(vm_t* vm);

#endif