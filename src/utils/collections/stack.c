/**
 * \file stack.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/stack.h"

#include "utils/collections/vector.h"

stack_t* stack_init(void)
{
  return (stack_t*)vector_init();
}

void stack_free(stack_t* stack)
{
  vector_free((vector_t*)stack);
}

void stack_push(stack_t* stack, void* data)
{
  vector_push((vector_t*)stack, data);
}

void* stack_pop(stack_t* stack)
{
  return vector_pop((vector_t*)stack);
}

void* stack_top(stack_t* stack)
{
  return vector_back((vector_t*)stack);
}

bool stack_empty(stack_t* stack)
{
  return vector_empty((vector_t*)stack);
}
