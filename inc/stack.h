#ifndef TAU_STACK_H
#define TAU_STACK_H

#include <stdbool.h>

#include "list.h"

typedef list_t stack_t;

stack_t* stack_init(void);
void stack_free(stack_t* stack);

void stack_push(stack_t* stack, void* data);
void* stack_pop(stack_t* stack);
void* stack_peek(stack_t* stack);

bool stack_empty(stack_t* stack);

#endif