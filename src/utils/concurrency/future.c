/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/future.h"

bool promise_init(promise_t* promise)
{
  if (!mutex_init(&promise->lock))
    return false;

  if (!condvar_init(&promise->cond))
    return false;

  promise->state = PROMISE_PENDING;
  promise->data = NULL;

  return true;
}

void promise_free(promise_t* promise)
{
  mutex_free(&promise->lock);
  condvar_free(&promise->cond);
}

promise_state_t promise_get_state(promise_t* promise)
{
  mutex_lock(&promise->lock);

  promise_state_t result = promise->state;

  mutex_unlock(&promise->lock);

  return result;
}

void promise_set_value(promise_t* promise, void* value)
{
  mutex_lock(&promise->lock);

  promise->state = PROMISE_FULFILLED;
  promise->data = value;

  condvar_broadcast(&promise->cond);

  mutex_unlock(&promise->lock);
}

void promise_set_error(promise_t* promise, void* error)
{
  mutex_lock(&promise->lock);

  promise->state = PROMISE_BROKEN;
  promise->data = error;

  condvar_broadcast(&promise->cond);

  mutex_unlock(&promise->lock);
}

void promise_get_future(promise_t* restrict promise, future_t* restrict future)
{
  mutex_lock(&promise->lock);

  ASSERT(promise->state == PROMISE_PENDING);
  future->promise = promise;

  mutex_unlock(&promise->lock);
}

void future_free(future_t* UNUSED(future))
{
}

void future_wait(future_t* future)
{
  mutex_lock(&future->promise->lock);

  while (future->promise->state == PROMISE_PENDING)
    condvar_wait(&future->promise->cond, &future->promise->lock);

  mutex_unlock(&future->promise->lock);
}

bool future_wait_for(future_t* restrict future, const struct timespec* restrict timeout)
{
  mutex_lock(&future->promise->lock);

  bool result = condvar_wait_for(&future->promise->cond, &future->promise->lock, timeout);

  mutex_unlock(&future->promise->lock);

  return result;
}

bool future_wait_until(future_t* restrict future, const struct timespec* restrict timepoint)
{
  mutex_lock(&future->promise->lock);

  bool result = condvar_wait_until(&future->promise->cond, &future->promise->lock, timepoint);

  mutex_unlock(&future->promise->lock);

  return result;
}

future_state_t future_get_state(future_t* future)
{
  mutex_lock(&future->promise->lock);

  promise_state_t promise_state = future->promise->state;

  mutex_unlock(&future->promise->lock);

  future_state_t result = FUTURE_PENDING;

  switch (promise_state)
  {
  case PROMISE_PENDING: result = FUTURE_PENDING; break;
  case PROMISE_FULFILLED: result = FUTURE_READY; break;
  case PROMISE_BROKEN: result = FUTURE_FAILED; break;
  default: UNREACHABLE();
  }

  return result;
}

void* future_get_value(future_t* future)
{
  ASSERT(future->promise->state == PROMISE_FULFILLED);
  return future->promise->data;
}

void* future_get_error(future_t* future)
{
  ASSERT(future->promise->state == PROMISE_BROKEN);
  return future->promise->data;
}
