/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/latch.h"

#include "utils/concurrency/thread.h"

bool latch_init(latch_t* latch, size_t count)
{
  if (!mutex_init(&latch->lock))
    return false;

  if (!condvar_init(&latch->cond))
    return false;

  latch->count = count;

  return true;
}

void latch_free(latch_t* latch)
{
  mutex_free(&latch->lock);
  condvar_free(&latch->cond);
}

size_t latch_get_count(latch_t* latch)
{
  mutex_lock(&latch->lock);

  size_t count = latch->count;

  mutex_unlock(&latch->lock);

  return count;
}

void latch_wait(latch_t* latch)
{
  mutex_lock(&latch->lock);

  while (latch->count > 0)
    condvar_wait(&latch->cond, &latch->lock);

  mutex_unlock(&latch->lock);
}

bool latch_wait_for(latch_t* restrict latch, const struct timespec* restrict timeout)
{
  mutex_lock(&latch->lock);

  bool result = true;

  if (latch->count > 0)
    result = condvar_wait_for(&latch->cond, &latch->lock, timeout);

  mutex_unlock(&latch->lock);

  return result;
}

bool latch_wait_until(latch_t* restrict latch, const struct timespec* restrict timepoint)
{
  mutex_lock(&latch->lock);

  bool result = true;

  if (latch->count > 0)
    result = condvar_wait_until(&latch->cond, &latch->lock, timepoint);

  mutex_unlock(&latch->lock);

  return result;
}

void latch_arrive(latch_t* latch)
{
  latch_arrive_n(latch, 1);
}

void latch_arrive_n(latch_t* latch, size_t n)
{
  mutex_lock(&latch->lock);

  if (latch->count > 0)
  {
    ASSERT(latch->count >= n);
    latch->count -= n;

    if (latch->count == 0)
      condvar_broadcast(&latch->cond);
  }

  mutex_unlock(&latch->lock);
}

void latch_arrive_and_wait(latch_t* latch)
{
  latch_arrive(latch);
  latch_wait(latch);
}
