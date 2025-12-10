#include "test.h"

#include "utils/concurrency/thread.h"
#include "utils/concurrency/mutex.h"
#include "utils/timer.h"

/// Argument type for set_flag_after_delay.
typedef struct flag_arg_t
{
  tau_mutex_t mtx; ///< Mutex to protect flag.
  int flag; ///< The flag to set.
  int set_to; ///< The value to set flag to.
  int ms_delay; ///< The delay in milliseconds before setting flag.
} flag_arg_t;

/// Argument type for multi_worker.
typedef struct multi_arg_t
{
  int id; ///< The worker ID.
  int ms_delay; ///< Time to sleep in milliseconds.
} multi_arg_t;

/**
 * \brief Allocates a new integer and sets its value to arg.
 *
 * \param arg The value to initialize the newly allocated integer with.
 * \returns Pointer to the newly allocated integer.
 */
static void* ret_int_alloc(void* arg)
{
  int val = (int)(intptr_t)arg;
  int* p = malloc(sizeof(int));

  if (p == NULL)
    return NULL;

  *p = val;

  return p;
}

/**
 * \brief Sets an integer flag to a specified value after a delay.
 *
 * \param arg A pointer to flag_arg_t.
 * \returns NULL
 */
static void* set_flag_after_delay(void* arg)
{
  flag_arg_t* fa = (flag_arg_t*)arg;

  struct timespec dur;
  dur.tv_sec = fa->ms_delay / 1000;
  dur.tv_nsec = fa->ms_delay % 1000 * 1000000L;
  tau_thread_sleep(&dur);

  tau_mutex_lock(&fa->mtx);
  fa->flag = fa->set_to;
  tau_mutex_unlock(&fa->mtx);

  return NULL;
}

/**
 * \brief Compares the current thread and the one received through arg.
 *
 * \param arg Pointer to a tau_thread_t.
 * \returns Non-zero value if threads are not equal, NULL otherwise.
 */
static void* compare_self_in_thread(void* arg)
{
  tau_thread_t* main_thread = (tau_thread_t*)arg;
  tau_thread_t self;
  tau_thread_self(&self);

  return tau_thread_equal(&self, main_thread) ? (void*)(intptr_t)1 : NULL;
}

/**
 * \brief Allocates a new integer and sets its value after a delay.
 *
 * \param arg Pointer to a multi_arg_t.
 * \returns Pointer to the newly allocated integer.
 */
static void* multi_worker(void* arg)
{
  multi_arg_t* ma = (multi_arg_t*)arg;

  if (ma->ms_delay > 0)
  {
    struct timespec dur;
    dur.tv_sec = ma->ms_delay / 1000;
    dur.tv_nsec = ma->ms_delay % 1000 * 1000000L;
    tau_thread_sleep(&dur);
  }

  int* p = malloc(sizeof(int));

  if (p == NULL)
    return NULL;

  *p = ma->id * 10;

  return p;
}

TEST_CASE(init_join_return)
{
  tau_thread_t thread;

  int id = 42;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, ret_int_alloc, (void*)(intptr_t)id));

  void* ret = tau_thread_join(&thread);
  TEST_ASSERT_NOT_NULL(ret);

  int value = *(int*)ret;
  free(ret);

  tau_thread_free(&thread);

  TEST_ASSERT_EQUAL(value, id);
}

TEST_CASE(self_and_equal)
{
  tau_thread_t main_self;
  tau_thread_self(&main_self);

  TEST_ASSERT_TRUE(tau_thread_equal(&main_self, &main_self));

  tau_thread_t other;
  TEST_ASSERT_TRUE(tau_thread_init(&other, compare_self_in_thread, &main_self));

  void* ret = tau_thread_join(&other);
  tau_thread_free(&other);

  TEST_ASSERT_NULL(ret);
}

TEST_CASE(detach_runs_and_completes)
{
  flag_arg_t fa = { .flag = 0, .set_to = 123, .ms_delay = 50 };
  tau_mutex_init(&fa.mtx);

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, set_flag_after_delay, &fa));

  tau_thread_detach(&thread);

  struct timespec dur = { .tv_sec = 0, .tv_nsec = 200 * 1000000L };
  tau_thread_sleep(&dur);

  tau_mutex_lock(&fa.mtx);
  TEST_ASSERT_EQUAL(fa.flag, 123);
  tau_mutex_unlock(&fa.mtx);

  tau_mutex_free(&fa.mtx);
}

TEST_CASE(sleep_duration)
{
  struct timespec dur = { .tv_sec = 0, .tv_nsec = 100 * 1000000L };

  uint64_t before = tau_timer_now();
  tau_thread_sleep(&dur);
  uint64_t after = tau_timer_now();

  long elapsed_ns = (long)((double)(after - before) / (double)tau_timer_freq() * 1000000000.0);
  long expected_ns = (long)(dur.tv_sec * 1000000000L + dur.tv_nsec);

  const long leeway_ns = 30L * 1000000L;

  TEST_ASSERT_TRUE(elapsed_ns > (expected_ns - leeway_ns));
  TEST_ASSERT_TRUE(elapsed_ns < (expected_ns + leeway_ns));
}

/// Number of threads to use in test case multiple_threads_return_values.
#define MULTI_COUNT 8

TEST_CASE(multiple_threads_return_values)
{
  tau_thread_t threads[MULTI_COUNT];
  multi_arg_t args[MULTI_COUNT];

  for (int i = 0; i < MULTI_COUNT; ++i)
  {
    args[i].id = i;
    args[i].ms_delay = i % 3 * 20;

    TEST_ASSERT_TRUE(tau_thread_init(&threads[i], multi_worker, &args[i]));
  }

  for (int i = 0; i < MULTI_COUNT; ++i)
  {
    void* ret = tau_thread_join(&threads[i]);

    TEST_ASSERT_NOT_NULL(ret);

    int val = *(int*)ret;

    free(ret);
    tau_thread_free(&threads[i]);

    TEST_ASSERT_EQUAL(val, i * 10);
  }
}

TEST_CASE(self_stability)
{
  tau_thread_t old_self;
  tau_thread_self(&old_self);

  for (int i = 0; i < 50; ++i)
  {
    tau_thread_t self;
    tau_thread_self(&self);

    TEST_ASSERT_TRUE(tau_thread_equal(&old_self, &self));

    old_self = self;
  }
}

TEST_MAIN()
{
  TEST_RUN(init_join_return);
  TEST_RUN(self_and_equal);
  TEST_RUN(detach_runs_and_completes);
  TEST_RUN(sleep_duration);
  TEST_RUN(multiple_threads_return_values);
  TEST_RUN(self_stability);
}
