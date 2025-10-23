/**
 * \file
 *
 * \brief Condition variable library interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_CONDVAR_H
#define TAU_CONDVAR_H

#include "utils/common.h"
#include "utils/concurrency/mutex.h"

TAU_EXTERN_C_BEGIN

/// Represents a condition variable for thread synchronization.
typedef struct tau_condvar_t
{
#if TAU_OS_LINUX
  pthread_cond_t native_handle;
#endif
} tau_condvar_t;

/**
 * \brief Initializes a new condition variable.
 *
 * \param[out] condvar Pointer to condition variable buffer.
 * \returns `true` if condition variable was successfully initialized, `false` otherwise.
 */
bool tau_condvar_init(tau_condvar_t* condvar);

/**
 * \brief Frees all resources associated with a condition variable.
 *
 * \param[in] condvar Pointer to condition variable to be freed.
 */
void tau_condvar_free(tau_condvar_t* condvar);

/**
 * \brief Blocks until a condition variable is signaled.
 *
 * \pre `mutex` must be locked by the calling thread.
 *
 * \param[in] condvar Pointer to condition variable to wait on.
 * \param[in] mutex Pointer to mutex to be used.
 */
void tau_condvar_wait(tau_condvar_t* restrict condvar, tau_mutex_t* restrict mutex);

/**
 * \brief Blocks until a condition variable is signaled, or the specified duration has passed.
 *
 * \pre `mutex` must be locked by the calling thread.
 *
 * \param[in] condvar Pointer to condition variable to wait on.
 * \param[in] mutex Pointer to mutex to be used.
 * \param[in] timeout Duration to wait for.
 * \returns `true` if the thread was awakened by a signal before the duration has passed, `false` otherwise.
 */
bool tau_condvar_wait_for(tau_condvar_t* restrict condvar, tau_mutex_t* restrict mutex, const struct timespec* restrict timeout);

/**
 * \brief Blocks until a condition variable is signaled, or the specified time point is reached.
 *
 * \pre `mutex` must be locked by the calling thread.
 *
 * \param[in] condvar Pointer to condition variable to wait on.
 * \param[in] mutex Pointer to mutex to be used.
 * \param[in] timepoint Time point to wait until.
 * \returns `true` if the thread was awakened by a signal before the time point has been reached, `false` otherwise.
 */
bool tau_condvar_wait_until(tau_condvar_t* restrict condvar, tau_mutex_t* restrict mutex, const struct timespec* restrict timepoint);

/**
 * \brief Awakens one waiting thread.
 *
 * \param[in] condvar Pointer to condition variable to be used.
 */
void tau_condvar_signal(tau_condvar_t* condvar);

/**
 * \brief Awakens all waiting threads.
 *
 * \param[in] condvar Pointer to condition variable to be used.
 */
void tau_condvar_broadcast(tau_condvar_t* condvar);

TAU_EXTERN_C_END

#endif
