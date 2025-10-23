/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/barrier.h"

bool tau_barrier_init(tau_barrier_t* barrier, size_t count)
{
  if (!tau_mutex_init(&barrier->lock))
    return false;

  if (!tau_condvar_init(&barrier->cond))
    return false;

  barrier->phase = 0;
  barrier->initial_count = count;
  barrier->count = count;

  return true;
}

void tau_barrier_free(tau_barrier_t* barrier)
{
  tau_mutex_free(&barrier->lock);
  tau_condvar_free(&barrier->cond);
}

size_t tau_barrier_get_count(tau_barrier_t* barrier)
{
  tau_mutex_lock(&barrier->lock);

  size_t count = barrier->count;

  tau_mutex_unlock(&barrier->lock);

  return count;
}

size_t tau_barrier_get_phase(tau_barrier_t* barrier)
{
  tau_mutex_lock(&barrier->lock);

  size_t phase = barrier->phase;

  tau_mutex_unlock(&barrier->lock);

  return phase;
}

void tau_barrier_wait(tau_barrier_t* barrier)
{
  tau_mutex_lock(&barrier->lock);

  size_t phase = barrier->phase;

  while (barrier->phase == phase)
    tau_condvar_wait(&barrier->cond, &barrier->lock);

  tau_mutex_unlock(&barrier->lock);
}

bool tau_barrier_wait_for(tau_barrier_t* restrict barrier, const struct timespec* restrict timeout)
{
  tau_mutex_lock(&barrier->lock);

  bool result = tau_condvar_wait_for(&barrier->cond, &barrier->lock, timeout);

  tau_mutex_unlock(&barrier->lock);

  return result;
}

bool tau_barrier_wait_until(tau_barrier_t* restrict barrier, const struct timespec* restrict timepoint)
{
  tau_mutex_lock(&barrier->lock);

  bool result = tau_condvar_wait_until(&barrier->cond, &barrier->lock, timepoint);

  tau_mutex_unlock(&barrier->lock);

  return result;
}

void tau_barrier_arrive(tau_barrier_t* barrier)
{
  tau_barrier_arrive_n(barrier, 1);
}

void tau_barrier_arrive_n(tau_barrier_t* barrier, size_t n)
{
  tau_mutex_lock(&barrier->lock);

  if (barrier->count > 0)
  {
    TAU_ASSERT(barrier->count >= n);
    barrier->count -= n;

    if (barrier->count == 0)
    {
      barrier->phase++;
      barrier->count = barrier->initial_count;
      tau_condvar_broadcast(&barrier->cond);
    }
  }

  tau_mutex_unlock(&barrier->lock);
}

void tau_barrier_arrive_and_wait(tau_barrier_t* barrier)
{
  tau_mutex_lock(&barrier->lock);

  if (barrier->count > 0)
  {
    barrier->count--;

    if (barrier->count == 0)
    {
      barrier->phase++;
      barrier->count = barrier->initial_count;
      tau_condvar_broadcast(&barrier->cond);
    }
    else
    {
      size_t phase = barrier->phase;

      while (barrier->phase == phase)
        tau_condvar_wait(&barrier->cond, &barrier->lock);
    }
  }

  tau_mutex_unlock(&barrier->lock);
}
