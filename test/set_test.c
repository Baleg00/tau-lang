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

TEST_CASE(set_init)
{
  set_t* set = set_init(cmp_int);

  TEST_ASSERT_NOT_NULL(set);
  TEST_ASSERT_TRUE(set_empty(set));

  set_free(set);
}

TEST_CASE(set_add)
{
  int data1 = 1, data2 = 2, data3 = 3;

  set_t* set = set_init(cmp_int);

  TEST_ASSERT_TRUE(set_add(set, &data1));
  TEST_ASSERT_TRUE(set_add(set, &data2));
  TEST_ASSERT_TRUE(set_add(set, &data3));
  TEST_ASSERT_FALSE(set_add(set, &data1));
  TEST_ASSERT_FALSE(set_add(set, &data2));
  TEST_ASSERT_FALSE(set_add(set, &data3));

  set_free(set);
}

TEST_CASE(set_remove)
{
  int data1 = 1, data2 = 2, data3 = 3;

  set_t* set = set_init(cmp_int);

  set_add(set, &data1);
  set_add(set, &data2);
  set_add(set, &data3);

  TEST_ASSERT_TRUE(set_remove(set, &data1));
  TEST_ASSERT_TRUE(set_remove(set, &data2));
  TEST_ASSERT_TRUE(set_remove(set, &data3));
  TEST_ASSERT_FALSE(set_remove(set, &data1));
  TEST_ASSERT_FALSE(set_remove(set, &data2));
  TEST_ASSERT_FALSE(set_remove(set, &data3));

  set_free(set);
}

TEST_CASE(set_contains)
{
  int data1 = 1, data2 = 2, data3 = 3;

  set_t* set = set_init(cmp_int);

  TEST_ASSERT_FALSE(set_contains(set, &data1));
  TEST_ASSERT_FALSE(set_contains(set, &data2));
  TEST_ASSERT_FALSE(set_contains(set, &data3));
  set_add(set, &data1);
  TEST_ASSERT_TRUE(set_contains(set, &data1));
  set_add(set, &data2);
  TEST_ASSERT_TRUE(set_contains(set, &data1));
  TEST_ASSERT_TRUE(set_contains(set, &data2));
  set_add(set, &data3);
  TEST_ASSERT_TRUE(set_contains(set, &data1));
  TEST_ASSERT_TRUE(set_contains(set, &data2));
  TEST_ASSERT_TRUE(set_contains(set, &data3));

  set_free(set);
}

TEST_CASE(set_get)
{
  int data1 = 1, data2 = 2, data3 = 3;

  set_t* set = set_init(cmp_int);

  TEST_ASSERT_NULL(set_get(set, &data1));
  TEST_ASSERT_NULL(set_get(set, &data2));
  TEST_ASSERT_NULL(set_get(set, &data3));

  set_add(set, &data1);
  TEST_ASSERT_PTR_EQUAL(set_get(set, &data1), &data1);
  TEST_ASSERT_NULL(set_get(set, &data2));
  TEST_ASSERT_NULL(set_get(set, &data3));
  set_add(set, &data2);
  TEST_ASSERT_PTR_EQUAL(set_get(set, &data1), &data1);
  TEST_ASSERT_PTR_EQUAL(set_get(set, &data2), &data2);
  TEST_ASSERT_NULL(set_get(set, &data3));
  set_add(set, &data3);
  TEST_ASSERT_PTR_EQUAL(set_get(set, &data1), &data1);
  TEST_ASSERT_PTR_EQUAL(set_get(set, &data2), &data2);
  TEST_ASSERT_PTR_EQUAL(set_get(set, &data3), &data3);

  set_free(set);
}

TEST_CASE(set_min)
{
  int data1 = 1, data2 = 2, data3 = 3;

  set_t* set = set_init(cmp_int);

  set_add(set, &data1);
  set_add(set, &data2);
  set_add(set, &data3);

  TEST_ASSERT_PTR_EQUAL(set_min(set), &data1);

  set_free(set);
}

TEST_CASE(set_min_empty)
{
  set_t* set = set_init(cmp_int);

  TEST_ASSERT_NULL(set_min(set));

  set_free(set);
}

TEST_CASE(set_max)
{
  int data1 = 1, data2 = 2, data3 = 3;

  set_t* set = set_init(cmp_int);

  set_add(set, &data1);
  set_add(set, &data2);
  set_add(set, &data3);

  TEST_ASSERT_PTR_EQUAL(set_max(set), &data3);

  set_free(set);
}

TEST_CASE(set_max_empty)
{
  set_t* set = set_init(cmp_int);

  TEST_ASSERT_NULL(set_max(set));

  set_free(set);
}

TEST_CASE(set_size)
{
  int data1 = 1, data2 = 2, data3 = 3;

  set_t* set = set_init(cmp_int);

  TEST_ASSERT_EQUAL(set_size(set), 0);
  set_add(set, &data1);
  TEST_ASSERT_EQUAL(set_size(set), 1);
  set_add(set, &data2);
  TEST_ASSERT_EQUAL(set_size(set), 2);
  set_add(set, &data3);
  TEST_ASSERT_EQUAL(set_size(set), 3);

  set_free(set);
}

TEST_CASE(set_for_each)
{
  int data1 = 1, data2 = 2, data3 = 3;

  set_t* set = set_init(cmp_int);

  set_add(set, &data1);
  set_add(set, &data2);
  set_add(set, &data3);

  set_for_each(set, iter_increment);

  TEST_ASSERT_EQUAL(data1, 2);
  TEST_ASSERT_EQUAL(data2, 3);
  TEST_ASSERT_EQUAL(data3, 4);

  set_free(set);
}

TEST_MAIN()
{
  TEST_RUN(set_init);
  TEST_RUN(set_add);
  TEST_RUN(set_remove);
  TEST_RUN(set_contains);
  TEST_RUN(set_get);
  TEST_RUN(set_min);
  TEST_RUN(set_min_empty);
  TEST_RUN(set_max);
  TEST_RUN(set_max_empty);
  TEST_RUN(set_size);
  TEST_RUN(set_for_each);
}
