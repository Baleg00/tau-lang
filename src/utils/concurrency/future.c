/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/future.h"

bool tau_promise_init(tau_promise_t* promise)
{
  if (!tau_mutex_init(&promise->lock))
    return false;

  if (!tau_condvar_init(&promise->cond))
    return false;

  promise->state = TAU_PROMISE_PENDING;
  promise->data = NULL;

  return true;
}

void tau_promise_free(tau_promise_t* promise)
{
  tau_mutex_free(&promise->lock);
  tau_condvar_free(&promise->cond);
}

tau_promise_state_t tau_promise_get_state(tau_promise_t* promise)
{
  tau_mutex_lock(&promise->lock);

  tau_promise_state_t result = promise->state;

  tau_mutex_unlock(&promise->lock);

  return result;
}

void tau_promise_set_value(tau_promise_t* promise, void* value)
{
  tau_mutex_lock(&promise->lock);

  promise->state = TAU_PROMISE_FULFILLED;
  promise->data = value;

  tau_condvar_broadcast(&promise->cond);

  tau_mutex_unlock(&promise->lock);
}

void tau_promise_set_error(tau_promise_t* promise, void* error)
{
  tau_mutex_lock(&promise->lock);

  promise->state = TAU_PROMISE_BROKEN;
  promise->data = error;

  tau_condvar_broadcast(&promise->cond);

  tau_mutex_unlock(&promise->lock);
}

void tau_promise_get_future(tau_promise_t* restrict promise, tau_future_t* restrict future)
{
  tau_mutex_lock(&promise->lock);

  TAU_ASSERT(promise->state == TAU_PROMISE_PENDING);
  future->promise = promise;

  tau_mutex_unlock(&promise->lock);
}

void tau_future_free(tau_future_t* TAU_UNUSED(future))
{
}

void tau_future_wait(tau_future_t* future)
{
  tau_mutex_lock(&future->promise->lock);

  while (future->promise->state == TAU_PROMISE_PENDING)
    tau_condvar_wait(&future->promise->cond, &future->promise->lock);

  tau_mutex_unlock(&future->promise->lock);
}

bool tau_future_wait_for(tau_future_t* restrict future, const struct timespec* restrict timeout)
{
  tau_mutex_lock(&future->promise->lock);

  bool result = tau_condvar_wait_for(&future->promise->cond, &future->promise->lock, timeout);

  tau_mutex_unlock(&future->promise->lock);

  return result;
}

bool tau_future_wait_until(tau_future_t* restrict future, const struct timespec* restrict timepoint)
{
  tau_mutex_lock(&future->promise->lock);

  bool result = tau_condvar_wait_until(&future->promise->cond, &future->promise->lock, timepoint);

  tau_mutex_unlock(&future->promise->lock);

  return result;
}

tau_future_state_t tau_future_get_state(tau_future_t* future)
{
  tau_mutex_lock(&future->promise->lock);

  tau_promise_state_t tau_promise_state = future->promise->state;

  tau_mutex_unlock(&future->promise->lock);

  tau_future_state_t result = TAU_FUTURE_PENDING;

  switch (tau_promise_state)
  {
  case TAU_PROMISE_PENDING: result = TAU_FUTURE_PENDING; break;
  case TAU_PROMISE_FULFILLED: result = TAU_FUTURE_READY; break;
  case TAU_PROMISE_BROKEN: result = TAU_FUTURE_FAILED; break;
  default: TAU_UNREACHABLE();
  }

  return result;
}

void* tau_future_get_value(tau_future_t* future)
{
  TAU_ASSERT(future->promise->state == TAU_PROMISE_FULFILLED);
  return future->promise->data;
}

void* tau_future_get_error(tau_future_t* future)
{
  TAU_ASSERT(future->promise->state == TAU_PROMISE_BROKEN);
  return future->promise->data;
}
