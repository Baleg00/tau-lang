#ifndef TAU_REGISTER_H
#define TAU_REGISTER_H

#include <stdint.h>
#include <stdbool.h>

#include "typedefs.h"

uint8_t register_encode(register_t reg);
register_t register_decode(uint8_t data, opcode_width_t width);

const char* register_to_string(register_t reg);

size_t register_bits(register_t reg);

bool register_is_8bit(register_t reg);
bool register_is_16bit(register_t reg);
bool register_is_32bit(register_t reg);
bool register_is_64bit(register_t reg);

#endif