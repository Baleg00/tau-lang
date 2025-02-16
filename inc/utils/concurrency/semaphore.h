/**
 * \file
 *
 * \brief Semaphore library interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_SEMAPHORE_H
#define TAU_SEMAPHORE_H

#include "utils/concurrency/condvar.h"
#include "utils/concurrency/mutex.h"

TAU_EXTERN_C_BEGIN

/// Represents a semaphore for controlling access to resources.
typedef struct semaphore_t {
  mutex_t lock;
  condvar_t cond;
  size_t count;
} semaphore_t;

/**
 * \brief Initializes a new semaphore.
 *
 * \param[out] sem Pointer to semaphore buffer.
 * \param[in] count The initial count value.
 * \returns `true` if semaphore was successfully initialized, `false` otherwise.
 */
bool semaphore_init(semaphore_t* sem, size_t count);

/**
 * \brief Frees all resources associated with a semaphore.
 *
 * \param[in] sem Pointer to semaphore to be freed.
 */
void semaphore_free(semaphore_t* sem);

/**
 * \brief Retrieves the counter of a semaphore.
 *
 * \param[in] sem Pointer to semaphore to be used.
 * \returns Counter of the semaphore.
 */
size_t semaphore_get_count(semaphore_t* sem);

/**
 * \brief Decreases the counter of a semaphore, or blocks until it can.
 *
 * \param[in] sem Pointer to semaphore to be used.
 */
void semaphore_acquire(semaphore_t* sem);

/**
 * \brief Decreases the counter of a semaphore by `n`, or blocks until it can.
 *
 * \param[in] sem Pointer to semaphore to be used.
 * \param[in] n Number to decrease the counter by.
 */
void semaphore_acquire_n(semaphore_t* sem, size_t n);

/**
 * \brief Tries to decrease the counter of a semaphore.
 *
 * \param[in] sem Pointer to semaphore to be used.
 * \returns `true` if the counter was decreased successfully, `false` otherwise.
 */
bool semaphore_try_acquire(semaphore_t* sem);

/**
 * \brief Tries to decrease the counter of a semaphore by `n`.
 *
 * \param[in] sem Pointer to semaphore to be used.
 * \param[in] n Number to decrease the counter by.
 * \returns `true` if the counter was decreased by `n` successfully, `false` otherwise.
 */
bool semaphore_try_acquire_n(semaphore_t* sem, size_t n);

/**
 * \brief Increments the counter of a semaphore.
 *
 * \param[in] sem Pointer to semaphore to be used.
 */
void semaphore_release(semaphore_t* sem);

/**
 * \brief Increments the counter of a semaphore by `n`.
 *
 * \param[in] sem Pointer to semaphore to be used.
 * \param[in] n Number to increment the counter by.
 */
void semaphore_release_n(semaphore_t* sem, size_t n);

TAU_EXTERN_C_END

#endif
