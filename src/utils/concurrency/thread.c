/**
 * \file
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/concurrency/thread.h"

#if TAU_OS_LINUX

#include <unistd.h>

bool thread_init(thread_t* thread, thread_func_t func, void* arg)
{
  return pthread_create(&thread->native_handle, NULL, func, arg) == 0;
}

void thread_free(thread_t* UNUSED(thread))
{
}

void thread_self(thread_t* thread)
{
  thread->native_handle = pthread_self();
}

void thread_sleep(const struct timespec* duration)
{
  int result = nanosleep(duration, NULL);
  ASSERT(result == 0);
}

void* thread_join(thread_t* thread)
{
  void* thread_result = NULL;

  int join_status = pthread_join(thread->native_handle, &thread_result);
  ASSERT(join_status == 0);

  return thread_result;
}

void thread_detach(thread_t* thread)
{
  pthread_detach(thread->native_handle);
}

bool thread_equal(thread_t* thread1, thread_t* thread2)
{
  return pthread_equal(thread1->native_handle, thread2->native_handle) != 0;
}

#else
# error "Threads are not implemented for this platform!"
#endif
