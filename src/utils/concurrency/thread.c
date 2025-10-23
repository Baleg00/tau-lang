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

#else
# error "Threads are not implemented for this platform!"
#endif
