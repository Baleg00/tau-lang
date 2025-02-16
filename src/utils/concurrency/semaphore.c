/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/semaphore.h"

bool semaphore_init(semaphore_t* sem, size_t count)
{
  if (!mutex_init(&sem->lock))
    return false;

  if (!condvar_init(&sem->cond))
    return false;

  sem->count = count;

  return true;
}

void semaphore_free(semaphore_t* sem)
{
  mutex_free(&sem->lock);
  condvar_free(&sem->cond);
}

size_t semaphore_get_count(semaphore_t* sem)
{
  mutex_lock(&sem->lock);

  size_t count = sem->count;

  mutex_unlock(&sem->lock);

  return count;
}

void semaphore_acquire(semaphore_t* sem)
{
  semaphore_acquire_n(sem, 1);
}

void semaphore_acquire_n(semaphore_t* sem, size_t n)
{
  mutex_lock(&sem->lock);

  while (sem->count < n)
    condvar_wait(&sem->cond, &sem->lock);

  sem->count -= n;

  mutex_unlock(&sem->lock);
}

bool semaphore_try_acquire(semaphore_t* sem)
{
  return semaphore_try_acquire_n(sem, 1);
}

bool semaphore_try_acquire_n(semaphore_t* sem, size_t n)
{
  mutex_lock(&sem->lock);

  if (sem->count < n)
  {
    mutex_unlock(&sem->lock);
    return false;
  }

  sem->count -= n;

  mutex_unlock(&sem->lock);

  return true;
}

void semaphore_release(semaphore_t* sem)
{
  semaphore_release_n(sem, 1);
}

void semaphore_release_n(semaphore_t* sem, size_t n)
{
  mutex_lock(&sem->lock);

  sem->count += n;

  for (size_t i = 0; i < n; i++)
    condvar_signal(&sem->cond);

  mutex_unlock(&sem->lock);
}
