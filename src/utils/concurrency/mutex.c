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

void tau_mutex_unlock(tau_mutex_t* mutex)
{
  int result = pthread_mutex_unlock(&mutex->native_handle);
  TAU_ASSERT(result == 0);
}

#elif TAU_OS_WINDOWS

bool tau_mutex_init(tau_mutex_t* mutex)
{
  InitializeCriticalSection(&mutex->critical_sec);
  return true;
}

void tau_mutex_free(tau_mutex_t* mutex)
{
  DeleteCriticalSection(&mutex->critical_sec);
}

void tau_mutex_lock(tau_mutex_t* mutex)
{
  EnterCriticalSection(&mutex->critical_sec);
}

bool tau_mutex_try_lock(tau_mutex_t* mutex)
{
  return TryEnterCriticalSection(&mutex->critical_sec);
}

void tau_mutex_unlock(tau_mutex_t* mutex)
{
  LeaveCriticalSection(&mutex->critical_sec);
}

#else
# error "Mutexes are not implemented for this platform!"
#endif
