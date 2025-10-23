/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/queue.h"

#include "utils/collections/list.h"

tau_queue_t* tau_queue_init(void)
{
  return (tau_queue_t*)tau_list_init();
}

void tau_queue_free(tau_queue_t* que)
{
  tau_list_free((tau_list_t*)que);
}

void tau_queue_offer(tau_queue_t* que, void* data)
{
  tau_list_push_front((tau_list_t*)que, data);
}

void* tau_queue_poll(tau_queue_t* que)
{
  return tau_list_pop_back((tau_list_t*)que);
}

void* tau_queue_peek(const tau_queue_t* que)
{
  return tau_list_back((const tau_list_t*)que);
}

bool tau_queue_empty(const tau_queue_t* que)
{
  return tau_list_empty((const tau_list_t*)que);
}
