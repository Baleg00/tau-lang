/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/latch.h"

#include "utils/concurrency/thread.h"

bool tau_latch_init(tau_latch_t* latch, size_t count)
{
  if (!tau_mutex_init(&latch->lock))
    return false;

  if (!tau_condvar_init(&latch->cond))
    return false;

  latch->count = count;

  return true;
}

void tau_latch_free(tau_latch_t* latch)
{
  tau_mutex_free(&latch->lock);
  tau_condvar_free(&latch->cond);
}

size_t tau_latch_get_count(tau_latch_t* latch)
{
  tau_mutex_lock(&latch->lock);

  size_t count = latch->count;

  tau_mutex_unlock(&latch->lock);

  return count;
}

void tau_latch_wait(tau_latch_t* latch)
{
  tau_mutex_lock(&latch->lock);

  while (latch->count > 0)
    tau_condvar_wait(&latch->cond, &latch->lock);

  tau_mutex_unlock(&latch->lock);
}

bool tau_latch_wait_for(tau_latch_t* restrict latch, const struct timespec* restrict timeout)
{
  tau_mutex_lock(&latch->lock);

  bool result = true;

  if (latch->count > 0)
    result = tau_condvar_wait_for(&latch->cond, &latch->lock, timeout);

  tau_mutex_unlock(&latch->lock);

  return result;
}

bool tau_latch_wait_until(tau_latch_t* restrict latch, const struct timespec* restrict timepoint)
{
  tau_mutex_lock(&latch->lock);

  bool result = true;

  if (latch->count > 0)
    result = tau_condvar_wait_until(&latch->cond, &latch->lock, timepoint);

  tau_mutex_unlock(&latch->lock);

  return result;
}

void tau_latch_arrive(tau_latch_t* latch)
{
  tau_latch_arrive_n(latch, 1);
}

void tau_latch_arrive_n(tau_latch_t* latch, size_t n)
{
  tau_mutex_lock(&latch->lock);

  if (latch->count > 0)
  {
    TAU_ASSERT(latch->count >= n);
    latch->count -= n;

    if (latch->count == 0)
      tau_condvar_broadcast(&latch->cond);
  }

  tau_mutex_unlock(&latch->lock);
}

void tau_latch_arrive_and_wait(tau_latch_t* latch)
{
  tau_latch_arrive(latch);
  tau_latch_wait(latch);
}
