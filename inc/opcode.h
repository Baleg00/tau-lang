#ifndef TAU_OPCODE_H
#define TAU_OPCODE_H

#include <stdint.h>

#include "typedefs.h"

uint16_t opcode_encode(opcode_t opcode, opcode_param_t param, opcode_width_t width);
void opcode_decode(uint16_t value, opcode_t* opcode, opcode_param_t* param, opcode_width_t* width);

const char* opcode_to_string(opcode_t opcode);

size_t opcode_param_count(opcode_t opcode);

#endif