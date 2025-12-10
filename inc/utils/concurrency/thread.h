/**
 * \file
 *
 * \brief Thread library interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_THREAD_H
#define TAU_THREAD_H

#include "utils/common.h"

#if TAU_OS_LINUX
# include <pthread.h>
#elif TAU_OS_WINDOWS
# include <windows.h>
#endif

TAU_EXTERN_C_BEGIN

/**
 * \brief Function pointer type for thread worker functions.
 *
 * \param[in] arg User-defined argument passed to the thread.
 * \returns User-defined return value.
 */
typedef void*(*tau_thread_func_t)(void* arg);

/// Represents a thread.
typedef struct tau_thread_t
{
#if TAU_OS_LINUX
  pthread_t native_handle;
#elif TAU_OS_WINDOWS
  HANDLE native_handle;
  tau_thread_func_t func;
  void* arg;
  void* ret;
#endif
} tau_thread_t;

/**
 * \brief Initializes and starts a new thread.
 *
 * \param[out] thread Pointer to thread buffer.
 * \param[in] func Function to execute in the thread.
 * \param[in] arg Argument to pass to the thread function.
 * \returns `true` if thread was successfully initialized, `false` otherwise.
 */
bool tau_thread_init(tau_thread_t* thread, tau_thread_func_t func, void* arg);

/**
 * \brief Frees all resources associated with a thread (must be called after joining the thread).
 *
 * \param[in] thread Pointer to thread to be freed.
 */
void tau_thread_free(tau_thread_t* thread);

/**
 * \brief Obtain a handle for the calling thread.
 *
 * \param[out] thread Pointer to thread buffer.
 */
void tau_thread_self(tau_thread_t* thread);

/**
 * \brief Suspends the execution of the calling thread for a specific duration.
 *
 * \param[in] duration Duration to sleep for.
 */
void tau_thread_sleep(const struct timespec* duration);

/**
 * \brief Wait for a thread to finish execution and free its resources.
 *
 * \param[in] thread Pointer to thread to join.
 * \returns User-defined return value of joined thread.
 */
void* tau_thread_join(tau_thread_t* thread);

/**
 * \brief Detaches a thread.
 *
 * \param[in] thread Pointer to thread to detach.
 */
void tau_thread_detach(tau_thread_t* thread);

/**
 * \brief Checks if two threads are equal.
 *
 * \param[in] thread1 Pointer to thread to compare.
 * \param[in] thread2 Pointer to thread to compare.
 * \returns `true` if the threads are the same, `false` otherwise.
 */
bool tau_thread_equal(tau_thread_t* thread1, tau_thread_t* thread2);

TAU_EXTERN_C_END

#endif
