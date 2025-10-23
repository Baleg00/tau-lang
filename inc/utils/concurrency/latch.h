/**
 * \file
 *
 * \brief Latch library interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_LATCH_H
#define TAU_LATCH_H

#include "utils/concurrency/condvar.h"
#include "utils/concurrency/mutex.h"

TAU_EXTERN_C_BEGIN

/// Represents a latch (or single-use barrier) for synchronizing multiple threads.
typedef struct tau_latch_t
{
  tau_mutex_t lock;
  tau_condvar_t cond;
  size_t count;
} tau_latch_t;

/**
 * \brief Initializes a new latch.
 *
 * \param[out] latch Pointer to latch buffer.
 * \param[in] count The initial count value.
 * \returns `true` if latch was successfully initialized, `false` otherwise.
 */
bool tau_latch_init(tau_latch_t* latch, size_t count);

/**
 * \brief Frees all resources associated with a latch.
 *
 * \param[in] latch Pointer to latch to be freed.
 */
void tau_latch_free(tau_latch_t* latch);

/**
 * \brief Retrieves the latch counter.
 *
 * \param[in] latch Pointer to latch to be used.
 * \returns Counter of the latch.
 */
size_t tau_latch_get_count(tau_latch_t* latch);

/**
 * \brief Blocks until the latch counter reaches zero.
 *
 * \param[in] latch Pointer to latch to be used.
 */
void tau_latch_wait(tau_latch_t* latch);

/**
 * \brief Blocks until the latch counter reaches zero, or the specified timeout duration has passed.
 *
 * \param[in] latch Pointer to latch to be used.
 * \param[in] timeout Duration to wait for.
 * \returns `true` if the thread was awakened due to the latch counter reaching zero, `false` otherwise.
 */
bool tau_latch_wait_for(tau_latch_t* restrict latch, const struct timespec* restrict timeout);

/**
 * \brief Blocks until the latch counter reaches zero, or the specified time point is reached.
 *
 * \param[in] latch Pointer to latch to be used.
 * \param[in] timepoint Time point to wait until.
 * \returns `true` if the thread was awakened due to the latch counter reaching zero, `false` otherwise.
 */
bool tau_latch_wait_until(tau_latch_t* restrict latch, const struct timespec* restrict timepoint);

/**
 * \brief Decrements the latch counter by one.
 *
 * \pre The latch counter has not yet reached zero.
 *
 * \param[in] latch Pointer to latch to be used.
 */
void tau_latch_arrive(tau_latch_t* latch);

/**
 * \brief Decrements the latch counter by `n`.
 *
 * \pre The latch counter is at least `n`.
 *
 * \param[in] latch Pointer to latch to be used.
 * \param[in] n Value by which to decrement the counter.
 */
void tau_latch_arrive_n(tau_latch_t* latch, size_t n);

/**
 * \brief Decrements the latch counter by one, and blocks until it reaches zero.
 *
 * \pre The latch counter has not yet reached zero.
 *
 * \param[in] latch Pointer to latch to be used.
 */
void tau_latch_arrive_and_wait(tau_latch_t* latch);

TAU_EXTERN_C_END

#endif
