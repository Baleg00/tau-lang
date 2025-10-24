/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/mutex.h"

#if TAU_OS_LINUX

bool tau_mutex_init(tau_mutex_t* mutex)
{
  return pthread_mutex_init(&mutex->native_handle, NULL) == 0;
}

void tau_mutex_free(tau_mutex_t* mutex)
{
  int result = pthread_mutex_destroy(&mutex->native_handle);
  TAU_ASSERT(result == 0);
}

void tau_mutex_lock(tau_mutex_t* mutex)
{
  int result = pthread_mutex_lock(&mutex->native_handle);
  TAU_ASSERT(result == 0);
}

bool tau_mutex_try_lock(tau_mutex_t* mutex)
{
  return pthread_mutex_trylock(&mutex->native_handle) == 0;
}

bool tau_mutex_try_lock_for(tau_mutex_t* restrict mutex, const struct timespec* restrict timeout)
{
  return pthread_mutex_timedlock(&mutex->native_handle, timeout) == 0;
}

bool tau_mutex_try_lock_until(tau_mutex_t* restrict mutex, const struct timespec* restrict timepoint)
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

void tau_mutex_unlock(tau_mutex_t* mutex)
{
  int result = pthread_mutex_unlock(&mutex->native_handle);
  TAU_ASSERT(result == 0);
}

#elif TAU_OS_WINDOWS

bool tau_mutex_init(tau_mutex_t* mutex)
{
  mutex->native_handle = CreateMutex(NULL, false, NULL);
  InitializeCriticalSection(&mutex->critical_sec);
  return mutex->native_handle != NULL;
}

void tau_mutex_free(tau_mutex_t* mutex)
{
  BOOL result = CloseHandle(mutex->native_handle);
  TAU_ASSERT(result == TRUE);

  DeleteCriticalSection(&mutex->critical_sec);
}

void tau_mutex_lock(tau_mutex_t* mutex)
{
  DWORD result = WaitForSingleObject(mutex->native_handle, INFINITE);
  TAU_ASSERT(result == WAIT_OBJECT_0);
  EnterCriticalSection(&mutex->critical_sec);
}

bool tau_mutex_try_lock(tau_mutex_t* mutex)
{
  DWORD result = WaitForSingleObject(mutex->native_handle, 0);

  if (result == WAIT_OBJECT_0)
  {
    EnterCriticalSection(&mutex->critical_sec);
    return true;
  }

  return false;
}

bool tau_mutex_try_lock_for(tau_mutex_t* restrict mutex, const struct timespec* restrict timeout)
{
  DWORD result = WaitForSingleObject(mutex->native_handle, (DWORD)(timeout->tv_sec * 1000 + timeout->tv_nsec / 1000000));

  if (result == WAIT_OBJECT_0)
  {
    EnterCriticalSection(&mutex->critical_sec);
    return true;
  }

  return false;
}

bool tau_mutex_try_lock_until(tau_mutex_t* restrict mutex, const struct timespec* restrict timepoint)
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

  return tau_mutex_try_lock_for(mutex, &diff);
}

void tau_mutex_unlock(tau_mutex_t* mutex)
{
  LeaveCriticalSection(&mutex->critical_sec);
  BOOL result = ReleaseMutex(mutex->native_handle);
  TAU_ASSERT(result == TRUE);
}

#else
# error "Mutexes are not implemented for this platform!"
#endif
