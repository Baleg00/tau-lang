#include "test.h"

#include "utils/collections/queue.h"

TEST_CASE(tau_queue_init)
{
  tau_queue_t* queue = tau_queue_init();

  TEST_ASSERT_NOT_NULL(queue);
  TEST_ASSERT_TRUE(tau_queue_empty(queue));

  tau_queue_free(queue);
}

TEST_CASE(tau_queue_offer_poll)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_queue_t* queue = tau_queue_init();

  tau_queue_offer(queue, &data1);
  tau_queue_offer(queue, &data2);
  tau_queue_offer(queue, &data3);

  TEST_ASSERT_FALSE(tau_queue_empty(queue));
  TEST_ASSERT_PTR_EQUAL(tau_queue_poll(queue), &data1);
  TEST_ASSERT_PTR_EQUAL(tau_queue_poll(queue), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_queue_poll(queue), &data3);
  TEST_ASSERT_TRUE(tau_queue_empty(queue));

  tau_queue_free(queue);
}

TEST_CASE(tau_queue_peek)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_queue_t* queue = tau_queue_init();

  tau_queue_offer(queue, &data1);
  TEST_ASSERT_PTR_EQUAL(tau_queue_peek(queue), &data1);
  tau_queue_offer(queue, &data2);
  TEST_ASSERT_PTR_EQUAL(tau_queue_peek(queue), &data1);
  tau_queue_offer(queue, &data3);
  TEST_ASSERT_PTR_EQUAL(tau_queue_peek(queue), &data1);
  tau_queue_poll(queue);
  TEST_ASSERT_PTR_EQUAL(tau_queue_peek(queue), &data2);
  tau_queue_poll(queue);
  TEST_ASSERT_PTR_EQUAL(tau_queue_peek(queue), &data3);

  tau_queue_free(queue);
}

TEST_MAIN()
{
  TEST_RUN(tau_queue_init);
  TEST_RUN(tau_queue_offer_poll);
  TEST_RUN(tau_queue_peek);
}
