/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/condvar.h"

#if TAU_OS_LINUX

bool tau_condvar_init(tau_condvar_t* condvar)
{
  return pthread_cond_init(&condvar->native_handle, NULL) == 0;
}

void tau_condvar_free(tau_condvar_t* condvar)
{
  int result = pthread_cond_destroy(&condvar->native_handle);
  TAU_ASSERT(result == 0);
}

void tau_condvar_wait(tau_condvar_t* condvar, tau_mutex_t* mutex)
{
  int result = pthread_cond_wait(&condvar->native_handle, &mutex->native_handle);
  TAU_ASSERT(result == 0);
}

bool tau_condvar_wait_for(tau_condvar_t* restrict condvar, tau_mutex_t* restrict mutex, const struct timespec* restrict timeout)
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

bool tau_condvar_wait_until(tau_condvar_t* restrict condvar, tau_mutex_t* restrict mutex, const struct timespec* restrict timepoint)
{
  return pthread_cond_timedwait(&condvar->native_handle, &mutex->native_handle, timepoint) == 0;
}

void tau_condvar_signal(tau_condvar_t* condvar)
{
  int result = pthread_cond_signal(&condvar->native_handle);
  TAU_ASSERT(result == 0);
}

void tau_condvar_broadcast(tau_condvar_t* condvar)
{
  int result = pthread_cond_broadcast(&condvar->native_handle);
  TAU_ASSERT(result == 0);
}

#elif TAU_OS_WINDOWS

bool tau_condvar_init(tau_condvar_t* condvar)
{
  InitializeConditionVariable(&condvar->native_handle);
  return true;
}

void tau_condvar_free(tau_condvar_t* TAU_UNUSED(condvar))
{
}

void tau_condvar_wait(tau_condvar_t* condvar, tau_mutex_t* mutex)
{
  BOOL result = SleepConditionVariableCS(&condvar->native_handle, &mutex->critical_sec, INFINITE);
  TAU_ASSERT(result == TRUE);
}

bool tau_condvar_wait_for(tau_condvar_t* restrict condvar, tau_mutex_t* restrict mutex, const struct timespec* restrict timeout)
{
  return SleepConditionVariableCS(&condvar->native_handle, &mutex->critical_sec, (DWORD)(timeout->tv_sec * 1000 + timeout->tv_nsec / 1000000)) == TRUE;
}

bool tau_condvar_wait_until(tau_condvar_t* restrict condvar, tau_mutex_t* restrict mutex, const struct timespec* restrict timepoint)
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

  return tau_condvar_wait_for(condvar, mutex, &diff);
}

void tau_condvar_signal(tau_condvar_t* condvar)
{
  WakeConditionVariable(&condvar->native_handle);
}

void tau_condvar_broadcast(tau_condvar_t* condvar)
{
  WakeAllConditionVariable(&condvar->native_handle);
}

#else
# error "Condition variables are not implemented for this platform!"
#endif
