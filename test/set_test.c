#include "test.h"

#include "utils/collections/set.h"

int cmp_int(const void* lhs, const void* rhs)
{
  return *(int*)lhs - *(int*)rhs;
}

void iter_increment(void* data)
{
  ++*(int*)data;
}

TEST_CASE(tau_set_init)
{
  tau_set_t* set = tau_set_init(cmp_int);

  TEST_ASSERT_NOT_NULL(set);
  TEST_ASSERT_TRUE(tau_set_empty(set));

  tau_set_free(set);
}

TEST_CASE(tau_set_add)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_set_t* set = tau_set_init(cmp_int);

  TEST_ASSERT_TRUE(tau_set_add(set, &data1));
  TEST_ASSERT_TRUE(tau_set_add(set, &data2));
  TEST_ASSERT_TRUE(tau_set_add(set, &data3));
  TEST_ASSERT_FALSE(tau_set_add(set, &data1));
  TEST_ASSERT_FALSE(tau_set_add(set, &data2));
  TEST_ASSERT_FALSE(tau_set_add(set, &data3));

  tau_set_free(set);
}

TEST_CASE(tau_set_remove)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_set_t* set = tau_set_init(cmp_int);

  tau_set_add(set, &data1);
  tau_set_add(set, &data2);
  tau_set_add(set, &data3);

  TEST_ASSERT_TRUE(tau_set_remove(set, &data1));
  TEST_ASSERT_TRUE(tau_set_remove(set, &data2));
  TEST_ASSERT_TRUE(tau_set_remove(set, &data3));
  TEST_ASSERT_FALSE(tau_set_remove(set, &data1));
  TEST_ASSERT_FALSE(tau_set_remove(set, &data2));
  TEST_ASSERT_FALSE(tau_set_remove(set, &data3));

  tau_set_free(set);
}

TEST_CASE(tau_set_contains)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_set_t* set = tau_set_init(cmp_int);

  TEST_ASSERT_FALSE(tau_set_contains(set, &data1));
  TEST_ASSERT_FALSE(tau_set_contains(set, &data2));
  TEST_ASSERT_FALSE(tau_set_contains(set, &data3));
  tau_set_add(set, &data1);
  TEST_ASSERT_TRUE(tau_set_contains(set, &data1));
  tau_set_add(set, &data2);
  TEST_ASSERT_TRUE(tau_set_contains(set, &data1));
  TEST_ASSERT_TRUE(tau_set_contains(set, &data2));
  tau_set_add(set, &data3);
  TEST_ASSERT_TRUE(tau_set_contains(set, &data1));
  TEST_ASSERT_TRUE(tau_set_contains(set, &data2));
  TEST_ASSERT_TRUE(tau_set_contains(set, &data3));

  tau_set_free(set);
}

TEST_CASE(tau_set_get)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_set_t* set = tau_set_init(cmp_int);

  TEST_ASSERT_NULL(tau_set_get(set, &data1));
  TEST_ASSERT_NULL(tau_set_get(set, &data2));
  TEST_ASSERT_NULL(tau_set_get(set, &data3));

  tau_set_add(set, &data1);
  TEST_ASSERT_PTR_EQUAL(tau_set_get(set, &data1), &data1);
  TEST_ASSERT_NULL(tau_set_get(set, &data2));
  TEST_ASSERT_NULL(tau_set_get(set, &data3));
  tau_set_add(set, &data2);
  TEST_ASSERT_PTR_EQUAL(tau_set_get(set, &data1), &data1);
  TEST_ASSERT_PTR_EQUAL(tau_set_get(set, &data2), &data2);
  TEST_ASSERT_NULL(tau_set_get(set, &data3));
  tau_set_add(set, &data3);
  TEST_ASSERT_PTR_EQUAL(tau_set_get(set, &data1), &data1);
  TEST_ASSERT_PTR_EQUAL(tau_set_get(set, &data2), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_set_get(set, &data3), &data3);

  tau_set_free(set);
}

TEST_CASE(tau_set_min)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_set_t* set = tau_set_init(cmp_int);

  tau_set_add(set, &data1);
  tau_set_add(set, &data2);
  tau_set_add(set, &data3);

  TEST_ASSERT_PTR_EQUAL(tau_set_min(set), &data1);

  tau_set_free(set);
}

TEST_CASE(tau_set_min_empty)
{
  tau_set_t* set = tau_set_init(cmp_int);

  TEST_ASSERT_NULL(tau_set_min(set));

  tau_set_free(set);
}

TEST_CASE(tau_set_max)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_set_t* set = tau_set_init(cmp_int);

  tau_set_add(set, &data1);
  tau_set_add(set, &data2);
  tau_set_add(set, &data3);

  TEST_ASSERT_PTR_EQUAL(tau_set_max(set), &data3);

  tau_set_free(set);
}

TEST_CASE(tau_set_max_empty)
{
  tau_set_t* set = tau_set_init(cmp_int);

  TEST_ASSERT_NULL(tau_set_max(set));

  tau_set_free(set);
}

TEST_CASE(tau_set_size)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_set_t* set = tau_set_init(cmp_int);

  TEST_ASSERT_EQUAL(tau_set_size(set), 0);
  tau_set_add(set, &data1);
  TEST_ASSERT_EQUAL(tau_set_size(set), 1);
  tau_set_add(set, &data2);
  TEST_ASSERT_EQUAL(tau_set_size(set), 2);
  tau_set_add(set, &data3);
  TEST_ASSERT_EQUAL(tau_set_size(set), 3);

  tau_set_free(set);
}

TEST_CASE(tau_set_for_each)
{
  int data1 = 1, data2 = 2, data3 = 3;

  tau_set_t* set = tau_set_init(cmp_int);

  tau_set_add(set, &data1);
  tau_set_add(set, &data2);
  tau_set_add(set, &data3);

  tau_set_for_each(set, iter_increment);

  TEST_ASSERT_EQUAL(data1, 2);
  TEST_ASSERT_EQUAL(data2, 3);
  TEST_ASSERT_EQUAL(data3, 4);

  tau_set_free(set);
}

TEST_MAIN()
{
  TEST_RUN(tau_set_init);
  TEST_RUN(tau_set_add);
  TEST_RUN(tau_set_remove);
  TEST_RUN(tau_set_contains);
  TEST_RUN(tau_set_get);
  TEST_RUN(tau_set_min);
  TEST_RUN(tau_set_min_empty);
  TEST_RUN(tau_set_max);
  TEST_RUN(tau_set_max_empty);
  TEST_RUN(tau_set_size);
  TEST_RUN(tau_set_for_each);
}
