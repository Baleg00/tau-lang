#include "test.h"

#include "utils/collections/stack.h"

TEST_CASE(tau_stack_init)
{
  tau_stack_t* stack = tau_stack_init();

  TEST_ASSERT_NOT_NULL(stack);
  TEST_ASSERT_TRUE(tau_stack_empty(stack));

  tau_stack_free(stack);
}

TEST_CASE(tau_stack_push)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_stack_t* stack = tau_stack_init();

  TEST_ASSERT_TRUE(tau_stack_empty(stack));

  tau_stack_push(stack, &data1);
  tau_stack_push(stack, &data2);
  tau_stack_push(stack, &data3);

  TEST_ASSERT_FALSE(tau_stack_empty(stack));

  tau_stack_free(stack);
}

TEST_CASE(tau_stack_pop)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_stack_t* stack = tau_stack_init();

  TEST_ASSERT_TRUE(tau_stack_empty(stack));

  tau_stack_push(stack, &data1);
  tau_stack_push(stack, &data2);
  tau_stack_push(stack, &data3);

  TEST_ASSERT_FALSE(tau_stack_empty(stack));

  TEST_ASSERT_PTR_EQUAL(tau_stack_pop(stack), &data3);
  TEST_ASSERT_PTR_EQUAL(tau_stack_pop(stack), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_stack_pop(stack), &data1);

  TEST_ASSERT_TRUE(tau_stack_empty(stack));

  tau_stack_free(stack);
}

TEST_CASE(tau_stack_top)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_stack_t* stack = tau_stack_init();

  tau_stack_push(stack, &data1);
  TEST_ASSERT_PTR_EQUAL(tau_stack_top(stack), &data1);
  tau_stack_push(stack, &data2);
  TEST_ASSERT_PTR_EQUAL(tau_stack_top(stack), &data2);
  tau_stack_push(stack, &data3);
  TEST_ASSERT_PTR_EQUAL(tau_stack_top(stack), &data3);

  tau_stack_free(stack);
}

TEST_MAIN()
{
  TEST_RUN(tau_stack_init);
  TEST_RUN(tau_stack_push);
  TEST_RUN(tau_stack_pop);
  TEST_RUN(tau_stack_top);
}
