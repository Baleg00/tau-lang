#include <time.h>

#include "test.h"

#include "utils/concurrency/mutex.h"
#include "utils/concurrency/thread.h"

static tau_mutex_t g_test_mutex;

/**
 * \brief Locks a mutex, sleeps for 100 milliseconds and then unlocks the mutex.
 *
 * \param arg Pointer to a tau_mutex_t.
 * \returns NULL
 */
static void* thread_lock(void* arg)
{
  tau_mutex_t* m = (tau_mutex_t*)arg;

  tau_mutex_lock(m);

  struct timespec dur = { .tv_sec = 0, .tv_nsec = 100 * 1000000L };
  tau_thread_sleep(&dur);

  tau_mutex_unlock(m);

  return NULL;
}

TEST_CASE(mutex_init_free)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_test_mutex));
  tau_mutex_free(&g_test_mutex);
}

TEST_CASE(mutex_lock_unlock)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_test_mutex));
  tau_mutex_lock(&g_test_mutex);
  tau_mutex_unlock(&g_test_mutex);
  tau_mutex_free(&g_test_mutex);
}

TEST_CASE(mutex_try_lock_free)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_test_mutex));
  TEST_ASSERT_TRUE(tau_mutex_try_lock(&g_test_mutex));
  tau_mutex_unlock(&g_test_mutex);
  tau_mutex_free(&g_test_mutex);
}

TEST_CASE(mutex_try_lock_contention)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_test_mutex));

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, thread_lock, &g_test_mutex));

  struct timespec dur = { .tv_sec = 0, .tv_nsec = 10 * 1000000L };
  tau_thread_sleep(&dur);

  TEST_ASSERT_FALSE(tau_mutex_try_lock(&g_test_mutex));

  tau_thread_join(&thread);
  tau_thread_free(&thread);
  tau_mutex_free(&g_test_mutex);
}

TEST_CASE(mutex_try_lock_for_timeout)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_test_mutex));

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, thread_lock, &g_test_mutex));

  struct timespec dur = { .tv_sec = 0, .tv_nsec = 10 * 1000000L };
  tau_thread_sleep(&dur);

  struct timespec timeout = { .tv_sec = 0, .tv_nsec = 5 * 1000000L};
  TEST_ASSERT_FALSE(tau_mutex_try_lock_for(&g_test_mutex, &timeout));

  tau_thread_join(&thread);
  tau_thread_free(&thread);
  tau_mutex_free(&g_test_mutex);
}

TEST_CASE(mutex_try_lock_until_timeout)
{
  TEST_ASSERT_TRUE(tau_mutex_init(&g_test_mutex));

  tau_thread_t thread;
  TEST_ASSERT_TRUE(tau_thread_init(&thread, thread_lock, &g_test_mutex));

  struct timespec dur = { .tv_sec = 0, .tv_nsec = 10 * 1000000L };
  tau_thread_sleep(&dur);

  struct timespec timepoint;
  timespec_get(&timepoint, TIME_UTC);
  timepoint.tv_nsec += 10 * 1000000L;
  TEST_ASSERT_FALSE(tau_mutex_try_lock_until(&g_test_mutex, &timepoint));

  tau_thread_join(&thread);
  tau_thread_free(&thread);
  tau_mutex_free(&g_test_mutex);
}

TEST_MAIN()
{
  TEST_RUN(mutex_init_free);
  TEST_RUN(mutex_lock_unlock);
  TEST_RUN(mutex_try_lock_free);
  TEST_RUN(mutex_try_lock_contention);
  TEST_RUN(mutex_try_lock_for_timeout);
  TEST_RUN(mutex_try_lock_until_timeout);
}
