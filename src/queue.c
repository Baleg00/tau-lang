/**
 * \file queue.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "queue.h"

#include "list.h"

queue_t* queue_init(void)
{
  return (queue_t*)list_init();
}

void queue_free(queue_t* que)
{
  list_free((list_t*)que);
}

void queue_offer(queue_t* que, void* data)
{
  list_push_front((list_t*)que, data);
}

void* queue_poll(queue_t* que)
{
  return list_pop_back((list_t*)que);
}

void* queue_peek(queue_t* que)
{
  return list_back((list_t*)que);
}

bool queue_empty(queue_t* que)
{
  return list_empty((list_t*)que);
}
