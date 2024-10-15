/**
 * \file
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

void stack_push(stack_t* restrict stack, void* restrict data)
{
  vector_push((vector_t* restrict)stack, data);
}

void* stack_pop(stack_t* stack)
{
  return vector_pop((vector_t*)stack);
}

void stack_clear(stack_t* stack)
{
  vector_clear((vector_t*)stack);
}

void* stack_top(const stack_t* stack)
{
  return vector_back((const vector_t*)stack);
}

bool stack_empty(const stack_t* stack)
{
  return vector_empty((const vector_t*)stack);
}
