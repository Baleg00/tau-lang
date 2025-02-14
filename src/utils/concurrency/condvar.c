/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/condvar.h"

#if TAU_OS_LINUX

bool condvar_init(condvar_t* condvar)
{
  return pthread_cond_init(&condvar->native_handle, NULL) == 0;
}

void condvar_free(condvar_t* condvar)
{
  int result = pthread_cond_destroy(&condvar->native_handle);
  ASSERT(result == 0);
}

void condvar_wait(condvar_t* condvar, mutex_t* mutex)
{
  int result = pthread_cond_wait(&condvar->native_handle, &mutex->native_handle);
  ASSERT(result == 0);
}

bool condvar_wait_for(condvar_t* restrict condvar, mutex_t* restrict mutex, const struct timespec* restrict timeout)
{
  return pthread_cond_timedwait(&condvar->native_handle, &mutex->native_handle, timeout) == 0;
}

bool condvar_wait_until(condvar_t* restrict condvar, mutex_t* restrict mutex, const struct timespec* restrict timepoint)
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

  return pthread_cond_timedwait(&condvar->native_handle, &mutex->native_handle, &diff) == 0;
}

void condvar_signal(condvar_t* condvar)
{
  int result = pthread_cond_signal(&condvar->native_handle);
  ASSERT(result == 0);
}

void condvar_broadcast(condvar_t* condvar)
{
  int result = pthread_cond_broadcast(&condvar->native_handle);
  ASSERT(result == 0);
}

#else
# error "Condition variables are not implemented for this platform!"
#endif
