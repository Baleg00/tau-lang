#include "test.h"

#include "utils/concurrency/condvar.h"
#include "utils/concurrency/mutex.h"
#include "utils/concurrency/thread.h"

static tau_condvar_t g_cv;
static tau_mutex_t g_mutex;

static void sleep_ms(long ms)
{
  struct timespec dur = { .tv_sec = ms / 1000, .tv_nsec = (ms % 1000) * 1000000L };
  tau_thread_sleep(&dur);
}

/// Represents a waiter. Locks mutex, waits on condvar, sets flag after wake, unlocks.
typedef struct waiter_wait_ctx_t
{
  bool awoken; ///< `true` when awakened by condvar (or due to timeout).
} waiter_wait_ctx_t;

/**
 * \brief Locks mutex and waits on condition variable. Stores whether thread was awoken.
 *
 * \param arg Pointer to a `waiter_wait_ctx_t`.
 * \returns `NULL`.
 */
static void* waiter_wait(void* arg)
{
  waiter_wait_ctx_t* ctx = (waiter_wait_ctx_t*)arg;

  tau_mutex_lock(&g_mutex);
  tau_condvar_wait(&g_cv, &g_mutex);
  ctx->awoken = true;
  tau_mutex_unlock(&g_mutex);

  return NULL;
}

/// Waiter that uses wait_for and reports return value (`true` if signaled, `false` if timeout).
typedef struct waiter_wait_for_ctx_t
{
  struct timespec timeout;
  bool ret;
} waiter_wait_for_ctx_t;

/**
 * \brief Locks mutex and waits on condition variable for a specified timeout.
 * Stores whether condition variable was signaled or timeout exceeded.
 *
 * \param arg Pointer to a `waiter_wait_for_ctx_t`.
 * \returns `NULL`.
 */
static void* waiter_wait_for(void* arg)
{
  waiter_wait_for_ctx_t* ctx = (waiter_wait_for_ctx_t*)arg;

  tau_mutex_lock(&g_mutex);
  ctx->ret = tau_condvar_wait_for(&g_cv, &g_mutex, &ctx->timeout);
  tau_mutex_unlock(&g_mutex);

  return NULL;
}

/// Waiter that uses wait_until and reports return value (`true` if signaled, `false` if timeout).
typedef struct waiter_wait_until_ctx_t
{
  struct timespec timepoint;
  bool ret;
} waiter_wait_until_ctx_t;

/**
 * \brief Locks mutex and waits on condition variable until a specified timepoint.
 * Stores whether condition variable was signaled or timepoint was reached.
 *
 * \param arg Pointer to a `waiter_wait_until_ctx_t`.
 * \returns `NULL`.
 */
static void* waiter_return_wait_until(void* arg)
{
  waiter_wait_until_ctx_t* ctx = (waiter_wait_until_ctx_t*)arg;

  tau_mutex_lock(&g_mutex);
  ctx->ret = tau_condvar_wait_until(&g_cv, &g_mutex, &ctx->timepoint);
  tau_mutex_unlock(&g_mutex);

  return NULL;
}

TEST_CASE(condvar_init_free)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

TEST_CASE(condvar_wait_signal_single_waiter)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  waiter_wait_ctx_t ctx = { .awoken = false };

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, waiter_wait, &ctx));

  sleep_ms(20);

  tau_mutex_lock(&g_mutex);
  tau_condvar_signal(&g_cv);
  tau_mutex_unlock(&g_mutex);

  tau_thread_join(&thread);
  tau_thread_free(&thread);

  TEST_ASSERT_TRUE(ctx.awoken);

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

/// Number of threads to use in test case condvar_broadcast_multiple_waiters.
#define BROADCAST_COUNT 4

TEST_CASE(condvar_broadcast_multiple_waiters)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  tau_thread_t threads[BROADCAST_COUNT];
  waiter_wait_ctx_t ctxs[BROADCAST_COUNT];

  for (int i = 0; i < BROADCAST_COUNT; ++i)
  {
    ctxs[i].awoken = false;
    TEST_ASSERT_TRUE(tau_thread_init(&threads[i], waiter_wait, &ctxs[i]));
  }

  sleep_ms(20);

  tau_mutex_lock(&g_mutex);
  tau_condvar_broadcast(&g_cv);
  tau_mutex_unlock(&g_mutex);

  for (int i = 0; i < BROADCAST_COUNT; ++i)
  {
    tau_thread_join(&threads[i]);
    tau_thread_free(&threads[i]);
    TEST_ASSERT_TRUE(ctxs[i].awoken);
  }

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

/// Number of threads to use in test case condvar_signal_wakes_one_each_call.
#define SIGNAL_ONE_COUNT 4

TEST_CASE(condvar_signal_wakes_one_each_call)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  tau_thread_t threads[SIGNAL_ONE_COUNT];
  waiter_wait_ctx_t ctxs[SIGNAL_ONE_COUNT];

  for (int i = 0; i < SIGNAL_ONE_COUNT; ++i)
  {
    ctxs[i].awoken = false;
    TEST_ASSERT_TRUE(tau_thread_init(&threads[i], waiter_wait, &ctxs[i]));
  }

  sleep_ms(20);

  for (int s = 0; s < SIGNAL_ONE_COUNT; ++s)
  {
    tau_mutex_lock(&g_mutex);
    tau_condvar_signal(&g_cv);
    tau_mutex_unlock(&g_mutex);

    sleep_ms(10);
  }

  for (int i = 0; i < SIGNAL_ONE_COUNT; ++i)
  {
    tau_thread_join(&threads[i]);
    tau_thread_free(&threads[i]);
    TEST_ASSERT_TRUE(ctxs[i].awoken);
  }

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

TEST_CASE(condvar_wait_for_timeout)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  waiter_wait_for_ctx_t ctx = {
    .timeout = { .tv_sec  = 0, .tv_nsec = 10 * 1000000L },
    .ret = true
  };

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, waiter_wait_for, &ctx));

  sleep_ms(50);

  tau_thread_join(&thread);
  tau_thread_free(&thread);

  TEST_ASSERT_FALSE(ctx.ret);

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

TEST_CASE(condvar_wait_for_awaken_before_timeout)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  waiter_wait_for_ctx_t ctx = {
    .timeout = { .tv_sec  = 0, .tv_nsec = 500 * 1000000L },
    .ret = false
  };

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, waiter_wait_for, &ctx));

  sleep_ms(50);

  tau_mutex_lock(&g_mutex);
  tau_condvar_signal(&g_cv);
  tau_mutex_unlock(&g_mutex);

  tau_thread_join(&thread);
  tau_thread_free(&thread);

  TEST_ASSERT_TRUE(ctx.ret);

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

TEST_CASE(condvar_wait_until_timeout)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  waiter_wait_for_ctx_t ctx = { .ret = true };

  timespec_get(&ctx.timeout, TIME_UTC);

  ctx.timeout.tv_nsec += 10 * 1000000L;

  if (ctx.timeout.tv_nsec >= 1000000000L)
  {
    ctx.timeout.tv_sec += 1;
    ctx.timeout.tv_nsec -= 1000000000L;
  }

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, waiter_return_wait_until, &ctx));

  sleep_ms(50);

  tau_thread_join(&thread);
  tau_thread_free(&thread);

  TEST_ASSERT_FALSE(ctx.ret);

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

TEST_CASE(condvar_wait_until_awaken_before_timepoint)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  waiter_wait_for_ctx_t ctx = { .ret = false };

  timespec_get(&ctx.timeout, TIME_UTC);

  ctx.timeout.tv_nsec += 500 * 1000000L;

  if (ctx.timeout.tv_nsec >= 1000000000L)
  {
    ctx.timeout.tv_sec += 1;
    ctx.timeout.tv_nsec -= 1000000000L;
  }

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, waiter_return_wait_until, &ctx));

  sleep_ms(20);

  tau_mutex_lock(&g_mutex);
  tau_condvar_signal(&g_cv);
  tau_mutex_unlock(&g_mutex);

  tau_thread_join(&thread);
  tau_thread_free(&thread);

  TEST_ASSERT_TRUE(ctx.ret);

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

TEST_CASE(condvar_signal_with_no_waiters_is_safe)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_mutex));
  TEST_ASSERT_TRUE(tau_condvar_init(&g_cv));

  tau_mutex_lock(&g_mutex);
  tau_condvar_signal(&g_cv);
  tau_condvar_broadcast(&g_cv);
  tau_mutex_unlock(&g_mutex);

  tau_condvar_free(&g_cv);
  tau_mutex_free(&g_mutex);
}

TEST_MAIN()
{
  TEST_RUN(condvar_init_free);
  TEST_RUN(condvar_wait_signal_single_waiter);
  TEST_RUN(condvar_broadcast_multiple_waiters);
  TEST_RUN(condvar_signal_wakes_one_each_call);
  TEST_RUN(condvar_wait_for_timeout);
  TEST_RUN(condvar_wait_for_awaken_before_timeout);
  TEST_RUN(condvar_wait_until_timeout);
  TEST_RUN(condvar_wait_until_awaken_before_timepoint);
  TEST_RUN(condvar_signal_with_no_waiters_is_safe);
}
