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
#endif

TAU_EXTERN_C_BEGIN

/// Represents a thread.
typedef struct thread_t
{
#if TAU_OS_LINUX
  pthread_t native_handle;
#endif
} thread_t;

/**
 * \brief Function pointer type for thread worker functions.
 *
 * \param[in] arg User-defined argument passed to the thread.
 * \returns User-defined return value.
 */
typedef void*(*thread_func_t)(void* arg);

/**
 * \brief Initializes and starts a new thread.
 *
 * \param[out] thread Pointer to thread buffer.
 * \param[in] func Function to execute in the thread.
 * \param[in] arg Argument to pass to the thread function.
 * \returns `true` if thread was successfully initialized, `false` otherwise.
 */
bool thread_init(thread_t* thread, thread_func_t func, void* arg);

/**
 * \brief Frees all resources associated with a thread (must be called after joining the thread).
 *
 * \param[in] thread Pointer to thread to be freed.
 */
void thread_free(thread_t* thread);

/**
 * \brief Obtain a handle for the calling thread.
 *
 * \param[out] thread Pointer to thread buffer.
 */
void thread_self(thread_t* thread);

/**
 * \brief Wait for a thread to finish execution and free its resources.
 *
 * \param[in] thread Pointer to thread to join.
 * \returns User-defined return value of joined thread.
 */
void* thread_join(thread_t* thread);

/**
 * \brief Detaches a thread.
 *
 * \param[in] thread Pointer to thread to detach.
 */
void thread_detach(thread_t* thread);

/**
 * \brief Checks if two threads are equal.
 *
 * \param[in] thread1 Pointer to thread to compare.
 * \param[in] thread2 Pointer to thread to compare.
 * \returns `true` if the threads are the same, `false` otherwise.
 */
bool thread_equal(thread_t* thread1, thread_t* thread2);

TAU_EXTERN_C_END

#endif
