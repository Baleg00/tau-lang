#include "test.h"

#include "utils/collections/queue.h"

TEST_CASE(queue_init)
{
  queue_t* queue = queue_init();

  TEST_ASSERT_NOT_NULL(queue);
  TEST_ASSERT_TRUE(queue_empty(queue));

  queue_free(queue);
}

TEST_CASE(queue_offer_poll)
{
  int data1 = 1, data2 = 2, data3 = 3;

  queue_t* queue = queue_init();

  queue_offer(queue, &data1);
  queue_offer(queue, &data2);
  queue_offer(queue, &data3);

  TEST_ASSERT_FALSE(queue_empty(queue));
  TEST_ASSERT_PTR_EQUAL(queue_poll(queue), &data1);
  TEST_ASSERT_PTR_EQUAL(queue_poll(queue), &data2);
  TEST_ASSERT_PTR_EQUAL(queue_poll(queue), &data3);
  TEST_ASSERT_TRUE(queue_empty(queue));

  queue_free(queue);
}

TEST_CASE(queue_peek)
{
  int data1 = 1, data2 = 2, data3 = 3;

  queue_t* queue = queue_init();

  queue_offer(queue, &data1);
  TEST_ASSERT_PTR_EQUAL(queue_peek(queue), &data1);
  queue_offer(queue, &data2);
  TEST_ASSERT_PTR_EQUAL(queue_peek(queue), &data1);
  queue_offer(queue, &data3);
  TEST_ASSERT_PTR_EQUAL(queue_peek(queue), &data1);
  queue_poll(queue);
  TEST_ASSERT_PTR_EQUAL(queue_peek(queue), &data2);
  queue_poll(queue);
  TEST_ASSERT_PTR_EQUAL(queue_peek(queue), &data3);

  queue_free(queue);
}

TEST_MAIN()
{
  TEST_RUN(queue_init);
  TEST_RUN(queue_offer_poll);
  TEST_RUN(queue_peek);
}
