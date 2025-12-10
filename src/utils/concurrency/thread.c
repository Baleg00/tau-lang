/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/thread.h"

#if TAU_OS_LINUX

#include <unistd.h>

bool tau_thread_init(tau_thread_t* thread, tau_thread_func_t func, void* arg)
{
  return pthread_create(&thread->native_handle, NULL, func, arg) == 0;
}

void tau_thread_free(tau_thread_t* TAU_UNUSED(thread))
{
}

void tau_thread_self(tau_thread_t* thread)
{
  thread->native_handle = pthread_self();
}

void tau_thread_sleep(const struct timespec* duration)
{
  int result = nanosleep(duration, NULL);
  TAU_ASSERT(result == 0);
}

void* tau_thread_join(tau_thread_t* thread)
{
  void* tau_thread_result = NULL;

  int join_status = pthread_join(thread->native_handle, &tau_thread_result);
  TAU_ASSERT(join_status == 0);

  return tau_thread_result;
}

void tau_thread_detach(tau_thread_t* thread)
{
  pthread_detach(thread->native_handle);
}

bool tau_thread_equal(tau_thread_t* thread1, tau_thread_t* thread2)
{
  return pthread_equal(thread1->native_handle, thread2->native_handle) != 0;
}

#elif TAU_OS_WINDOWS

// Required for function CompareObjectHandles.
#pragma comment(lib, "mincore")

DWORD WINAPI tau_thread_proc(LPVOID arg)
{
  tau_thread_t* thread = (tau_thread_t*)arg;
  thread->ret = thread->func(thread->arg);
  return 0;
}

bool tau_thread_init(tau_thread_t* thread, tau_thread_func_t func, void* arg)
{
  thread->func = func;
  thread->arg = arg;
  thread->ret = NULL;
  thread->native_handle = CreateThread(NULL, 0, tau_thread_proc, thread, 0, NULL);

  return thread->native_handle != NULL;
}

void tau_thread_free(tau_thread_t* TAU_UNUSED(thread))
{
}

void tau_thread_self(tau_thread_t* thread)
{
  HANDLE pseudo_handle = GetCurrentThread();
  HANDLE real_handle;

  BOOL result = DuplicateHandle(
    GetCurrentProcess(),
    pseudo_handle,
    GetCurrentProcess(),
    &real_handle,
    0,
    FALSE,
    DUPLICATE_SAME_ACCESS
  );

  TAU_ASSERT(result == TRUE);

  thread->func = NULL;
  thread->arg = NULL;
  thread->ret = NULL;
  thread->native_handle = real_handle;
}

void tau_thread_sleep(const struct timespec* duration)
{
  Sleep((DWORD)(duration->tv_sec * 1000 + duration->tv_nsec / 1000000));
}

void* tau_thread_join(tau_thread_t* thread)
{
  DWORD result = WaitForSingleObject(thread->native_handle, INFINITE);
  TAU_ASSERT(result == WAIT_OBJECT_0);
  CloseHandle(thread->native_handle);
  thread->native_handle = INVALID_HANDLE_VALUE;
  return thread->ret;
}

void tau_thread_detach(tau_thread_t* thread)
{
  CloseHandle(thread->native_handle);
  thread->native_handle = INVALID_HANDLE_VALUE;
}

bool tau_thread_equal(tau_thread_t* thread1, tau_thread_t* thread2)
{
  return CompareObjectHandles(thread1->native_handle, thread2->native_handle) == TRUE;
}

#else
# error "Threads are not implemented for this platform!"
#endif
