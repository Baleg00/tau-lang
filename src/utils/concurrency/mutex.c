/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/mutex.h"

#if TAU_OS_LINUX

bool mutex_init(mutex_t* mutex)
{
  return pthread_mutex_init(&mutex->native_handle, NULL) == 0;
}

void mutex_free(mutex_t* mutex)
{
  int result = pthread_mutex_destroy(&mutex->native_handle);
  ASSERT(result == 0);
}

void mutex_lock(mutex_t* mutex)
{
  int result = pthread_mutex_lock(&mutex->native_handle);
  ASSERT(result == 0);
}

bool mutex_try_lock(mutex_t* mutex)
{
  return pthread_mutex_trylock(&mutex->native_handle) == 0;
}

bool mutex_try_lock_for(mutex_t* restrict mutex, const struct timespec* restrict timeout)
{
  return pthread_mutex_timedlock(&mutex->native_handle, timeout) == 0;
}

bool mutex_try_lock_until(mutex_t* restrict mutex, const struct timespec* restrict timepoint)
{
  struct timespec now;
  timespec_get(&now, TIME_UTC);

  struct timespec diff;
  diff.tv_sec = timepoint->tv_sec - now.tv_sec;
  diff.tv_nsec = timepoint->tv_nsec - now.tv_nsec;

  if (diff.tv_nsec < 0)
  {
    diff.tv_sec--;
    diff.tv_nsec += 1000000000;
  }

  if (diff.tv_sec < 0)
    return false;

  return pthread_mutex_timedlock(&mutex->native_handle, &diff) == 0;
}

void mutex_unlock(mutex_t* mutex)
{
  int result = pthread_mutex_unlock(&mutex->native_handle);
  ASSERT(result == 0);
}

#else
# error "Mutexes are not implemented for this platform!"
#endif
