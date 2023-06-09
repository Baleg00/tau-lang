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

uint8_t vm_register_8bit_get(vm_t* vm, register_t reg);
uint16_t vm_register_16bit_get(vm_t* vm, register_t reg);
uint32_t vm_register_32bit_get(vm_t* vm, register_t reg);
uint64_t vm_register_64bit_get(vm_t* vm, register_t reg);

void vm_register_get(vm_t* vm, register_t reg, void* data);

void vm_register_8bit_set(vm_t* vm, register_t reg, uint8_t value);
void vm_register_16bit_set(vm_t* vm, register_t reg, uint16_t value);
void vm_register_32bit_set(vm_t* vm, register_t reg, uint32_t value);
void vm_register_64bit_set(vm_t* vm, register_t reg, uint64_t value);

void vm_register_set(vm_t* vm, register_t reg, const void* data);

uint8_t vm_code_next_8bit(vm_t* vm);
uint16_t vm_code_next_16bit(vm_t* vm);
uint32_t vm_code_next_32bit(vm_t* vm);
uint64_t vm_code_next_64bit(vm_t* vm);

uint8_t vm_mem_8bit_get(const void* mem);
uint16_t vm_mem_16bit_get(const void* mem);
uint32_t vm_mem_32bit_get(const void* mem);
uint64_t vm_mem_64bit_get(const void* mem);

void vm_mem_8bit_set(void* mem, uint8_t value);
void vm_mem_16bit_set(void* mem, uint16_t value);
void vm_mem_32bit_set(void* mem, uint32_t value);
void vm_mem_64bit_set(void* mem, uint64_t value);

void vm_fetch(vm_t* vm);
void vm_decode(vm_t* vm);
void vm_execute(vm_t* vm);

void vm_run(vm_t* vm);

#endif