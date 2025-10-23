/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/stack.h"

#include "utils/collections/vector.h"

tau_stack_t* tau_stack_init(void)
{
  return (tau_stack_t*)tau_vector_init();
}

void tau_stack_free(tau_stack_t* stack)
{
  tau_vector_free((tau_vector_t*)stack);
}

void tau_stack_push(tau_stack_t* restrict stack, void* restrict data)
{
  tau_vector_push((tau_vector_t* restrict)stack, data);
}

void* tau_stack_pop(tau_stack_t* stack)
{
  return tau_vector_pop((tau_vector_t*)stack);
}

void tau_stack_clear(tau_stack_t* stack)
{
  tau_vector_clear((tau_vector_t*)stack);
}

void* tau_stack_top(const tau_stack_t* stack)
{
  return tau_vector_back((const tau_vector_t*)stack);
}

bool tau_stack_empty(const tau_stack_t* stack)
{
  return tau_vector_empty((const tau_vector_t*)stack);
}
