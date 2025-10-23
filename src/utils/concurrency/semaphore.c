/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/semaphore.h"

bool tau_semaphore_init(tau_semaphore_t* sem, size_t count)
{
  if (!tau_mutex_init(&sem->lock))
    return false;

  if (!tau_condvar_init(&sem->cond))
    return false;

  sem->count = count;

  return true;
}

void tau_semaphore_free(tau_semaphore_t* sem)
{
  tau_mutex_free(&sem->lock);
  tau_condvar_free(&sem->cond);
}

size_t tau_semaphore_get_count(tau_semaphore_t* sem)
{
  tau_mutex_lock(&sem->lock);

  size_t count = sem->count;

  tau_mutex_unlock(&sem->lock);

  return count;
}

void tau_semaphore_acquire(tau_semaphore_t* sem)
{
  tau_semaphore_acquire_n(sem, 1);
}

void tau_semaphore_acquire_n(tau_semaphore_t* sem, size_t n)
{
  tau_mutex_lock(&sem->lock);

  while (sem->count < n)
    tau_condvar_wait(&sem->cond, &sem->lock);

  sem->count -= n;

  tau_mutex_unlock(&sem->lock);
}

bool tau_semaphore_try_acquire(tau_semaphore_t* sem)
{
  return tau_semaphore_try_acquire_n(sem, 1);
}

bool tau_semaphore_try_acquire_n(tau_semaphore_t* sem, size_t n)
{
  tau_mutex_lock(&sem->lock);

  if (sem->count < n)
  {
    tau_mutex_unlock(&sem->lock);
    return false;
  }

  sem->count -= n;

  tau_mutex_unlock(&sem->lock);

  return true;
}

void tau_semaphore_release(tau_semaphore_t* sem)
{
  tau_semaphore_release_n(sem, 1);
}

void tau_semaphore_release_n(tau_semaphore_t* sem, size_t n)
{
  tau_mutex_lock(&sem->lock);

  sem->count += n;

  for (size_t i = 0; i < n; i++)
    tau_condvar_signal(&sem->cond);

  tau_mutex_unlock(&sem->lock);
}
