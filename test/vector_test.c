#include "test.h"

#include "utils/collections/vector.h"

TEST_CASE(vector_init)
{
  vector_t* vec = vector_init();

  TEST_ASSERT_NOT_NULL(vec);

  vector_free(vec);
}

TEST_CASE(vector_init_from_buffer_empty)
{
  vector_t* vec = vector_init_from_buffer(NULL, 0);

  TEST_ASSERT_NOT_NULL(vec);

  vector_free(vec);
}

TEST_CASE(vector_init_from_buffer)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  vector_t* vec = vector_init_from_buffer(buffer, 3);

  TEST_ASSERT_NOT_NULL(vec);
  TEST_ASSERT_EQUAL(vector_size(vec), 3);
  TEST_ASSERT_EQUAL(vector_get(vec, 0), &data1);
  TEST_ASSERT_EQUAL(vector_get(vec, 1), &data2);
  TEST_ASSERT_EQUAL(vector_get(vec, 2), &data3);

  vector_free(vec);
}

TEST_CASE(vector_push)
{
  int data1 = 5, data2 = 10;

  vector_t* vec = vector_init();

  vector_push(vec, &data1);
  vector_push(vec, &data2);

  TEST_ASSERT_EQUAL(vector_size(vec), 2);
  TEST_ASSERT_EQUAL(*(int*)vector_get(vec, 0), data1);
  TEST_ASSERT_EQUAL(*(int*)vector_get(vec, 1), data2);

  vector_free(vec);
}

TEST_CASE(vector_pop)
{
  int data1 = 5, data2 = 10;

  vector_t* vec = vector_init();

  vector_push(vec, &data1);
  vector_push(vec, &data2);

  void* popped = vector_pop(vec);

  TEST_ASSERT_EQUAL(vector_size(vec), 1);
  TEST_ASSERT_EQUAL(*(int*)popped, data2);

  vector_free(vec);
}

TEST_CASE(vector_set)
{
  int data1 = 5, data2 = 10;

  vector_t* vec = vector_init();

  vector_push(vec, &data1);
  vector_push(vec, &data2);

  int new_data = 20;
  vector_set(vec, 0, &new_data);

  TEST_ASSERT_EQUAL(vector_size(vec), 2);
  TEST_ASSERT_EQUAL(*(int*)vector_get(vec, 0), new_data);

  vector_free(vec);
}

TEST_CASE(vector_insert)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  vector_t* vec = vector_init_from_buffer(buffer, 3);

  int new_data = 4;

  vector_insert(vec, 0, &new_data);

  TEST_ASSERT_EQUAL(vector_size(vec), 4);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 0), &new_data);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 1), &data1);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 2), &data2);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 3), &data3);

  vector_insert(vec, 4, &new_data);

  TEST_ASSERT_EQUAL(vector_size(vec), 5);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 0), &new_data);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 1), &data1);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 2), &data2);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 3), &data3);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 4), &new_data);

  vector_insert(vec, 2, &new_data);

  TEST_ASSERT_EQUAL(vector_size(vec), 6);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 0), &new_data);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 1), &data1);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 2), &new_data);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 3), &data2);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 4), &data3);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 5), &new_data);

  vector_free(vec);
}

TEST_CASE(vector_remove)
{
  int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5;
  void* buffer[] = { &data1, &data2, &data3, &data4, &data5 };

  vector_t* vec = vector_init_from_buffer(buffer, 5);

  TEST_ASSERT_PTR_EQUAL(vector_remove(vec, 0), &data1);
  TEST_ASSERT_EQUAL(vector_size(vec), 4);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 0), &data2);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 1), &data3);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 2), &data4);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 3), &data5);

  TEST_ASSERT_PTR_EQUAL(vector_remove(vec, 3), &data5);
  TEST_ASSERT_EQUAL(vector_size(vec), 3);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 0), &data2);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 1), &data3);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 2), &data4);

  TEST_ASSERT_PTR_EQUAL(vector_remove(vec, 1), &data3);
  TEST_ASSERT_EQUAL(vector_size(vec), 2);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 0), &data2);
  TEST_ASSERT_PTR_EQUAL(vector_get(vec, 1), &data4);

  vector_free(vec);
}

TEST_CASE(vector_clear)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  vector_t* vec = vector_init_from_buffer(buffer, 3);

  TEST_ASSERT_EQUAL(vector_size(vec), 3);
  TEST_ASSERT_FALSE(vector_empty(vec));

  vector_clear(vec);

  TEST_ASSERT_EQUAL(vector_size(vec), 0);
  TEST_ASSERT_TRUE(vector_empty(vec));

  vector_free(vec);
}

TEST_CASE(vector_to_buffer)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  vector_t* vec = vector_init_from_buffer(buffer, 3);

  void* dst[3];
  vector_to_buffer(vec, dst);

  TEST_ASSERT_PTR_EQUAL(dst[0], &data1);
  TEST_ASSERT_PTR_EQUAL(dst[1], &data2);
  TEST_ASSERT_PTR_EQUAL(dst[2], &data3);

  vector_free(vec);
}

TEST_MAIN()
{
  TEST_RUN(vector_init);
  TEST_RUN(vector_init_from_buffer_empty);
  TEST_RUN(vector_init_from_buffer);
  TEST_RUN(vector_push);
  TEST_RUN(vector_pop);
  TEST_RUN(vector_set);
  TEST_RUN(vector_insert);
  TEST_RUN(vector_remove);
  TEST_RUN(vector_clear);
  TEST_RUN(vector_to_buffer);
}
