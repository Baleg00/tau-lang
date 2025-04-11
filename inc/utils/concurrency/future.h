/**
 * \file
 *
 * \brief Future-promise library interface.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_FUTURE_H
#define TAU_FUTURE_H

#include "utils/concurrency/condvar.h"
#include "utils/concurrency/mutex.h"

TAU_EXTERN_C_BEGIN

/// Enumeration of promise states.
typedef enum promise_state_t
{
  PROMISE_PENDING, ///< No value or error has been set yet.
  PROMISE_FULFILLED, ///< A value has been set and can be retrieved.
  PROMISE_BROKEN, ///< An error has been set instead of a value.
} promise_state_t;

/// Represents a promise for asynchronous result handling.
typedef struct promise_t
{
  promise_state_t state;
  mutex_t lock;
  condvar_t cond;
  void* data;
} promise_t;

/// Enumeration of future states.
typedef enum future_state_t
{
  FUTURE_PENDING, ///< No value or error has been set yet.
  FUTURE_READY, ///< A value has been set and can be retrieved.
  FUTURE_FAILED, ///< An error has been set instead of a value.
} future_state_t;

/// Represents a future value to be computed asynchronously.
typedef struct future_t
{
  promise_t* promise;
} future_t;

/**
 * \brief Initializes a new promise.
 *
 * \param[out] promise Pointer to promise buffer.
 * \returns `true` if promise was initialized successfully, `false` otherwise.
 */
bool promise_init(promise_t* promise);

/**
 * \brief Frees all resources associated with a promise.
 *
 * \param[in] promise Pointer to promise to be freed.
 */
void promise_free(promise_t* promise);

/**
 * \brief Retrieves the state of a promise.
 *
 * \param[in] promise Pointer to promise to be used.
 * \returns State of the promise.
 */
promise_state_t promise_get_state(promise_t* promise);

/**
 * \brief Fulfills promise and sets a value.
 *
 * \pre The state of the promise is `PROMISE_PENDING`.
 * \post The state of the promise is `PROMISE_FULFILLED`.
 *
 * \param[in,out] promise Pointer to promise to be used.
 * \param[in] value Value to set.
 */
void promise_set_value(promise_t* promise, void* value);

/**
 * \brief Breaks promise and sets an error instead of a value.
 *
 * \pre The state of the promise is `PROMISE_PENDING`.
 * \post The state of the promise is `PROMISE_BROKEN`.
 *
 * \param[in,out] promise Pointer to promise to be used.
 * \param[in] error Error to set.
 */
void promise_set_error(promise_t* promise, void* error);

/**
 * \brief Retrieves a new future associated with a promise.
 *
 * \param[in] promise Pointer to promise to be used.
 * \param[out] future Pointer to future buffer.
 */
void promise_get_future(promise_t* restrict promise, future_t* restrict future);

/**
 * \brief Frees all resources associated with a future.
 *
 * \param[in] future Pointer to future to be freed.
 */
void future_free(future_t* future);

/**
 * \brief Blocks until a future value is available.
 *
 * \param[in] future Pointer to future to be used.
 */
void future_wait(future_t* future);

/**
 * \brief Blocks until a future value is available, or the specified timeout duration has passed.
 *
 * \param[in] future Pointer to future to be used.
 * \param[in] timeout Duration to wait for.
 * \returns `true` if the future value was made available within the timeout duration, `false` otherwise.
 */
bool future_wait_for(future_t* restrict future, const struct timespec* restrict timeout);

/**
 * \brief Blocks until a future value is available, or the specified time point has been reached.
 *
 * \param[in] future Pointer to future to be used.
 * \param[in] timepoint Time point to wait until.
 * \returns `true` if the future value was made available before the time point, `false` otherwise.
 */
bool future_wait_until(future_t* restrict future, const struct timespec* restrict timepoint);

/**
 * \brief Retrieves the state of a future.
 *
 * \param[in] future Pointer to future to be used.
 * \returns State of the future.
 */
future_state_t future_get_state(future_t* future);

/**
 * \brief Retrieves a future value.
 *
 * \pre The state of the future must be `FUTURE_READY`.
 *
 * \param[in] future Pointer to future to be used.
 * \returns Future value.
 */
void* future_get_value(future_t* future);

/**
 * \brief Retrieves a future error.
 *
 * \pre The state of the future must be `FUTURE_FAILED`.
 *
 * \param[in] future Pointer to future to be used.
 * \returns Future error.
 */
void* future_get_error(future_t* future);

TAU_EXTERN_C_END

#endif
