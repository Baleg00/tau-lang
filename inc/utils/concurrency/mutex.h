/**
 * \file
 *
 * \brief Mutual exclusion library interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_MUTEX_H
#define TAU_MUTEX_H

#include "utils/common.h"

#if TAU_OS_LINUX
# include <pthread.h>
#elif TAU_OS_WINDOWS
# include <windows.h>
#endif

TAU_EXTERN_C_BEGIN

/// Represents a mutex for mutual exclusion.
typedef struct tau_mutex_t
{
#if TAU_OS_LINUX
  pthread_mutex_t native_handle;
#elif TAU_OS_WINDOWS
  CRITICAL_SECTION critical_sec;
#endif
} tau_mutex_t;

/**
 * \brief Initializes a new mutex.
 *
 * \param[out] mutex Pointer to mutex buffer.
 * \returns `true` if mutex was successfully initialized, `false` otherwise.
 */
bool tau_mutex_init(tau_mutex_t* mutex);

/**
 * \brief Frees all resources associated with a mutex.
 *
 * \param[in] mutex Pointer to mutex to be freed.
 */
void tau_mutex_free(tau_mutex_t* mutex);

/**
 * \brief Locks a mutex. If another thread already has the lock, blocks until the lock is acquired.
 *
 * \param[in] mutex Pointer to mutex to lock.
 */
void tau_mutex_lock(tau_mutex_t* mutex);

/**
 * \brief Tries to lock a mutex. If another thread already has the lock, returns immediately.
 *
 * \param[in] mutex Pointer to mutex to lock.
 * \returns `true` if the lock was acquired successfully, `false` otherwise.
 */
bool tau_mutex_try_lock(tau_mutex_t* mutex);

/**
 * \brief Unlocks a mutex.
 *
 * \pre The mutex must be locked by the calling thread.
 *
 * \param[in] mutex Pointer to mutex to unlock.
 */
void tau_mutex_unlock(tau_mutex_t* mutex);

TAU_EXTERN_C_END

#endif
