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
typedef enum tau_promise_state_t
{
  TAU_PROMISE_PENDING, ///< No value or error has been set yet.
  TAU_PROMISE_FULFILLED, ///< A value has been set and can be retrieved.
  TAU_PROMISE_BROKEN, ///< An error has been set instead of a value.
} tau_promise_state_t;

/// Represents a promise for asynchronous result handling.
typedef struct tau_promise_t
{
  tau_promise_state_t state;
  tau_mutex_t lock;
  tau_condvar_t cond;
  void* data;
} tau_promise_t;

/// Enumeration of future states.
typedef enum tau_future_state_t
{
  TAU_FUTURE_PENDING, ///< No value or error has been set yet.
  TAU_FUTURE_READY, ///< A value has been set and can be retrieved.
  TAU_FUTURE_FAILED, ///< An error has been set instead of a value.
} tau_future_state_t;

/// Represents a future value to be computed asynchronously.
typedef struct tau_future_t
{
  tau_promise_t* promise;
} tau_future_t;

/**
 * \brief Initializes a new promise.
 *
 * \param[out] promise Pointer to promise buffer.
 * \returns `true` if promise was initialized successfully, `false` otherwise.
 */
bool tau_promise_init(tau_promise_t* promise);

/**
 * \brief Frees all resources associated with a promise.
 *
 * \param[in] promise Pointer to promise to be freed.
 */
void tau_promise_free(tau_promise_t* promise);

/**
 * \brief Retrieves the state of a promise.
 *
 * \param[in] promise Pointer to promise to be used.
 * \returns State of the promise.
 */
tau_promise_state_t tau_promise_get_state(tau_promise_t* promise);

/**
 * \brief Fulfills promise and sets a value.
 *
 * \pre The state of the promise is `TAU_PROMISE_PENDING`.
 * \post The state of the promise is `TAU_PROMISE_FULFILLED`.
 *
 * \param[in,out] promise Pointer to promise to be used.
 * \param[in] value Value to set.
 */
void tau_promise_set_value(tau_promise_t* promise, void* value);

/**
 * \brief Breaks promise and sets an error instead of a value.
 *
 * \pre The state of the promise is `TAU_PROMISE_PENDING`.
 * \post The state of the promise is `TAU_PROMISE_BROKEN`.
 *
 * \param[in,out] promise Pointer to promise to be used.
 * \param[in] error Error to set.
 */
void tau_promise_set_error(tau_promise_t* promise, void* error);

/**
 * \brief Retrieves a new future associated with a promise.
 *
 * \param[in] promise Pointer to promise to be used.
 * \param[out] future Pointer to future buffer.
 */
void tau_promise_get_future(tau_promise_t* restrict promise, tau_future_t* restrict future);

/**
 * \brief Frees all resources associated with a future.
 *
 * \param[in] future Pointer to future to be freed.
 */
void tau_future_free(tau_future_t* future);

/**
 * \brief Blocks until a future value is available.
 *
 * \param[in] future Pointer to future to be used.
 */
void tau_future_wait(tau_future_t* future);

/**
 * \brief Blocks until a future value is available, or the specified timeout duration has passed.
 *
 * \param[in] future Pointer to future to be used.
 * \param[in] timeout Duration to wait for.
 * \returns `true` if the future value was made available within the timeout duration, `false` otherwise.
 */
bool tau_future_wait_for(tau_future_t* restrict future, const struct timespec* restrict timeout);

/**
 * \brief Blocks until a future value is available, or the specified time point has been reached.
 *
 * \param[in] future Pointer to future to be used.
 * \param[in] timepoint Time point to wait until.
 * \returns `true` if the future value was made available before the time point, `false` otherwise.
 */
bool tau_future_wait_until(tau_future_t* restrict future, const struct timespec* restrict timepoint);

/**
 * \brief Retrieves the state of a future.
 *
 * \param[in] future Pointer to future to be used.
 * \returns State of the future.
 */
tau_future_state_t tau_future_get_state(tau_future_t* future);

/**
 * \brief Retrieves a future value.
 *
 * \pre The state of the future must be `TAU_FUTURE_READY`.
 *
 * \param[in] future Pointer to future to be used.
 * \returns Future value.
 */
void* tau_future_get_value(tau_future_t* future);

/**
 * \brief Retrieves a future error.
 *
 * \pre The state of the future must be `TAU_FUTURE_FAILED`.
 *
 * \param[in] future Pointer to future to be used.
 * \returns Future error.
 */
void* tau_future_get_error(tau_future_t* future);

TAU_EXTERN_C_END

#endif
