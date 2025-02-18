/**
 * \file
 *
 * \brief Barrier library interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_BARRIER_H
#define TAU_BARRIER_H

#include "utils/concurrency/condvar.h"
#include "utils/concurrency/mutex.h"

TAU_EXTERN_C_BEGIN

/// Represents a reusable barrier for synchronizing multiple threads.
typedef struct barrier_t
{
  mutex_t lock;
  condvar_t cond;
  size_t phase;
  size_t initial_count;
  size_t count;
} barrier_t;

/**
 * \brief Initializes a new barrier.
 *
 * \param[out] barrier Pointer to barrier buffer.
 * \param[in] count The initial count value.
 * \returns `true` if barrier was successfully initialized, `false` otherwise.
 */
bool barrier_init(barrier_t* barrier, size_t count);

/**
 * \brief Frees all resources associated with a barrier.
 *
 * \param[in] barrier Pointer to barrier to be freed.
 */
void barrier_free(barrier_t* barrier);

/**
 * \brief Retrieves the barrier counter.
 *
 * \param[in] barrier Pointer to barrier to be used.
 * \returns Counter of the barrier.
 */
size_t barrier_get_count(barrier_t* barrier);

/**
 * \brief Retrieves the number of the barrier's current phase.
 *
 * \param[in] barrier Pointer to barrier to be used.
 * \returns Number of the barrier's current phase.
 */
size_t barrier_get_phase(barrier_t* barrier);

/**
 * \brief Blocks until the current barrier phase is completed.
 *
 * \param[in] barrier Pointer to barrier to be used.
 */
void barrier_wait(barrier_t* barrier);

/**
 * \brief Blocks until the current barrier phase is completed, or the specified timeout duration has passed.
 *
 * \param[in] barrier Pointer to barrier to be used.
 * \param[in] timeout Duration to wait for.
 * \returns `true` if the current barrier phase completed within the timeout duration, `false` otherwise.
 */
bool barrier_wait_for(barrier_t* restrict barrier, const struct timespec* restrict timeout);

/**
 * \brief Blocks until the current barrier phase is completed, or the specified time point is reached.
 *
 * \param[in] barrier Pointer to barrier to be used.
 * \param[in] timepoint Time point to wait until.
 * \returns `true` if the current barrier phase completed within the timeout duration, `false` otherwise.
 */
bool barrier_wait_until(barrier_t* restrict barrier, const struct timespec* restrict timepoint);

/**
 * \brief Decrements the barrier counter by one.
 *
 * \param[in] barrier Pointer to barrier to be used.
 */
void barrier_arrive(barrier_t* barrier);

/**
 * \brief Decrements the barrier counter by `n`.
 *
 * \pre The barrier counter is at least `n`.
 *
 * \param[in] barrier Pointer to barrier to be used.
 * \param[in] n Value by which to decrement the counter.
 */
void barrier_arrive_n(barrier_t* barrier, size_t n);

/**
 * \brief Decrements the barrier counter by one, and blocks until it reaches zero.
 *
 * \pre The barrier counter has not yet reached zero.
 *
 * \param[in] barrier Pointer to barrier to be used.
 */
void barrier_arrive_and_wait(barrier_t* barrier);

TAU_EXTERN_C_END

#endif
