#include "stack.h"

stack_t* stack_init(void)
{
  return (stack_t*)list_init();
}

void stack_free(stack_t* stack)
{
  list_free((list_t*)stack);
}

void stack_push(stack_t* stack, void* data)
{
  list_push_back((list_t*)stack, data);
}

void* stack_pop(stack_t* stack)
{
  return list_pop_back((list_t*)stack);
}

void* stack_peek(stack_t* stack)
{
  return list_back((list_t*)stack);
}

bool stack_empty(stack_t* stack)
{
  return list_empty((list_t*)stack);
}
