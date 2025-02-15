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
  struct timespec now;
  timespec_get(&now, TIME_UTC);

  struct timespec timepoint;
  timepoint.tv_sec = now.tv_sec + timeout->tv_sec;
  timepoint.tv_nsec = now.tv_nsec + timeout->tv_nsec;

  if (timepoint.tv_nsec >= 1000000000)
  {
    timepoint.tv_sec++;
    timepoint.tv_nsec -= 1000000000;
  }

  return pthread_cond_timedwait(&condvar->native_handle, &mutex->native_handle, &timepoint) == 0;
}

bool condvar_wait_until(condvar_t* restrict condvar, mutex_t* restrict mutex, const struct timespec* restrict timepoint)
{
  return pthread_cond_timedwait(&condvar->native_handle, &mutex->native_handle, timepoint) == 0;
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
