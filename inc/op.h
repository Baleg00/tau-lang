#ifndef TAU_OP_H
#define TAU_OP_H

#include <stdbool.h>

#include "typedefs.h"

const char* op_kind_to_string(op_kind_t kind);

int op_precedence(op_kind_t kind);

bool op_is_binary(op_kind_t kind);
bool op_is_unary(op_kind_t kind);

#endif