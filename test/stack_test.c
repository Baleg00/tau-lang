#include "test.h"

#include "utils/collections/stack.h"

TEST_CASE(stack_init)
{
  stack_t* stack = stack_init();

  TEST_ASSERT_NOT_NULL(stack);
  TEST_ASSERT_TRUE(stack_empty(stack));

  stack_free(stack);
}

TEST_CASE(stack_push)
{
  int data1 = 1, data2 = 2, data3 = 3;

  stack_t* stack = stack_init();

  TEST_ASSERT_TRUE(stack_empty(stack));

  stack_push(stack, &data1);
  stack_push(stack, &data2);
  stack_push(stack, &data3);

  TEST_ASSERT_FALSE(stack_empty(stack));

  stack_free(stack);
}

TEST_CASE(stack_pop)
{
  int data1 = 1, data2 = 2, data3 = 3;

  stack_t* stack = stack_init();

  TEST_ASSERT_TRUE(stack_empty(stack));

  stack_push(stack, &data1);
  stack_push(stack, &data2);
  stack_push(stack, &data3);

  TEST_ASSERT_FALSE(stack_empty(stack));

  TEST_ASSERT_PTR_EQUAL(stack_pop(stack), &data3);
  TEST_ASSERT_PTR_EQUAL(stack_pop(stack), &data2);
  TEST_ASSERT_PTR_EQUAL(stack_pop(stack), &data1);

  TEST_ASSERT_TRUE(stack_empty(stack));

  stack_free(stack);
}

TEST_CASE(stack_top)
{
  int data1 = 1, data2 = 2, data3 = 3;

  stack_t* stack = stack_init();

  stack_push(stack, &data1);
  TEST_ASSERT_PTR_EQUAL(stack_top(stack), &data1);
  stack_push(stack, &data2);
  TEST_ASSERT_PTR_EQUAL(stack_top(stack), &data2);
  stack_push(stack, &data3);
  TEST_ASSERT_PTR_EQUAL(stack_top(stack), &data3);

  stack_free(stack);
}

TEST_MAIN()
{
  TEST_RUN(stack_init);
  TEST_RUN(stack_push);
  TEST_RUN(stack_pop);
  TEST_RUN(stack_top);
}
