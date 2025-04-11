/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/barrier.h"

bool barrier_init(barrier_t* barrier, size_t count)
{
  if (!mutex_init(&barrier->lock))
    return false;

  if (!condvar_init(&barrier->cond))
    return false;

  barrier->phase = 0;
  barrier->initial_count = count;
  barrier->count = count;

  return true;
}

void barrier_free(barrier_t* barrier)
{
  mutex_free(&barrier->lock);
  condvar_free(&barrier->cond);
}

size_t barrier_get_count(barrier_t* barrier)
{
  mutex_lock(&barrier->lock);

  size_t count = barrier->count;

  mutex_unlock(&barrier->lock);

  return count;
}

size_t barrier_get_phase(barrier_t* barrier)
{
  mutex_lock(&barrier->lock);

  size_t phase = barrier->phase;

  mutex_unlock(&barrier->lock);

  return phase;
}

void barrier_wait(barrier_t* barrier)
{
  mutex_lock(&barrier->lock);

  size_t phase = barrier->phase;

  while (barrier->phase == phase)
    condvar_wait(&barrier->cond, &barrier->lock);

  mutex_unlock(&barrier->lock);
}

bool barrier_wait_for(barrier_t* restrict barrier, const struct timespec* restrict timeout)
{
  mutex_lock(&barrier->lock);

  bool result = condvar_wait_for(&barrier->cond, &barrier->lock, timeout);

  mutex_unlock(&barrier->lock);

  return result;
}

bool barrier_wait_until(barrier_t* restrict barrier, const struct timespec* restrict timepoint)
{
  mutex_lock(&barrier->lock);

  bool result = condvar_wait_until(&barrier->cond, &barrier->lock, timepoint);

  mutex_unlock(&barrier->lock);

  return result;
}

void barrier_arrive(barrier_t* barrier)
{
  barrier_arrive_n(barrier, 1);
}

void barrier_arrive_n(barrier_t* barrier, size_t n)
{
  mutex_lock(&barrier->lock);

  if (barrier->count > 0)
  {
    ASSERT(barrier->count >= n);
    barrier->count -= n;

    if (barrier->count == 0)
    {
      barrier->phase++;
      barrier->count = barrier->initial_count;
      condvar_broadcast(&barrier->cond);
    }
  }

  mutex_unlock(&barrier->lock);
}

void barrier_arrive_and_wait(barrier_t* barrier)
{
  mutex_lock(&barrier->lock);

  if (barrier->count > 0)
  {
    barrier->count--;

    if (barrier->count == 0)
    {
      barrier->phase++;
      barrier->count = barrier->initial_count;
      condvar_broadcast(&barrier->cond);
    }
    else
    {
      size_t phase = barrier->phase;

      while (barrier->phase == phase)
        condvar_wait(&barrier->cond, &barrier->lock);
    }
  }

  mutex_unlock(&barrier->lock);
}
