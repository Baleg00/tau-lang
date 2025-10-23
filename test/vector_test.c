#include "test.h"

#include "utils/collections/vector.h"

TEST_CASE(tau_vector_init)
{
  tau_vector_t* vec = tau_vector_init();

  TEST_ASSERT_NOT_NULL(vec);

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_init_from_buffer_empty)
{
  tau_vector_t* vec = tau_vector_init_from_buffer(NULL, 0);

  TEST_ASSERT_NOT_NULL(vec);

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_init_from_buffer)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  tau_vector_t* vec = tau_vector_init_from_buffer(buffer, 3);

  TEST_ASSERT_NOT_NULL(vec);
  TEST_ASSERT_EQUAL(tau_vector_size(vec), 3);
  TEST_ASSERT_EQUAL(tau_vector_get(vec, 0), &data1);
  TEST_ASSERT_EQUAL(tau_vector_get(vec, 1), &data2);
  TEST_ASSERT_EQUAL(tau_vector_get(vec, 2), &data3);

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_push)
{
  int data1 = 5, data2 = 10;

  tau_vector_t* vec = tau_vector_init();

  tau_vector_push(vec, &data1);
  tau_vector_push(vec, &data2);

  TEST_ASSERT_EQUAL(tau_vector_size(vec), 2);
  TEST_ASSERT_EQUAL(*(int*)tau_vector_get(vec, 0), data1);
  TEST_ASSERT_EQUAL(*(int*)tau_vector_get(vec, 1), data2);

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_pop)
{
  int data1 = 5, data2 = 10;

  tau_vector_t* vec = tau_vector_init();

  tau_vector_push(vec, &data1);
  tau_vector_push(vec, &data2);

  void* popped = tau_vector_pop(vec);

  TEST_ASSERT_EQUAL(tau_vector_size(vec), 1);
  TEST_ASSERT_EQUAL(*(int*)popped, data2);

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_set)
{
  int data1 = 5, data2 = 10;

  tau_vector_t* vec = tau_vector_init();

  tau_vector_push(vec, &data1);
  tau_vector_push(vec, &data2);

  int new_data = 20;
  tau_vector_set(vec, 0, &new_data);

  TEST_ASSERT_EQUAL(tau_vector_size(vec), 2);
  TEST_ASSERT_EQUAL(*(int*)tau_vector_get(vec, 0), new_data);

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_insert)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  tau_vector_t* vec = tau_vector_init_from_buffer(buffer, 3);

  int new_data = 4;

  tau_vector_insert(vec, 0, &new_data);

  TEST_ASSERT_EQUAL(tau_vector_size(vec), 4);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 0), &new_data);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 1), &data1);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 2), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 3), &data3);

  tau_vector_insert(vec, 4, &new_data);

  TEST_ASSERT_EQUAL(tau_vector_size(vec), 5);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 0), &new_data);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 1), &data1);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 2), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 3), &data3);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 4), &new_data);

  tau_vector_insert(vec, 2, &new_data);

  TEST_ASSERT_EQUAL(tau_vector_size(vec), 6);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 0), &new_data);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 1), &data1);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 2), &new_data);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 3), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 4), &data3);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 5), &new_data);

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_remove)
{
  int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5;
  void* buffer[] = { &data1, &data2, &data3, &data4, &data5 };

  tau_vector_t* vec = tau_vector_init_from_buffer(buffer, 5);

  TEST_ASSERT_PTR_EQUAL(tau_vector_remove(vec, 0), &data1);
  TEST_ASSERT_EQUAL(tau_vector_size(vec), 4);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 0), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 1), &data3);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 2), &data4);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 3), &data5);

  TEST_ASSERT_PTR_EQUAL(tau_vector_remove(vec, 3), &data5);
  TEST_ASSERT_EQUAL(tau_vector_size(vec), 3);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 0), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 1), &data3);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 2), &data4);

  TEST_ASSERT_PTR_EQUAL(tau_vector_remove(vec, 1), &data3);
  TEST_ASSERT_EQUAL(tau_vector_size(vec), 2);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 0), &data2);
  TEST_ASSERT_PTR_EQUAL(tau_vector_get(vec, 1), &data4);

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_clear)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  tau_vector_t* vec = tau_vector_init_from_buffer(buffer, 3);

  TEST_ASSERT_EQUAL(tau_vector_size(vec), 3);
  TEST_ASSERT_FALSE(tau_vector_empty(vec));

  tau_vector_clear(vec);

  TEST_ASSERT_EQUAL(tau_vector_size(vec), 0);
  TEST_ASSERT_TRUE(tau_vector_empty(vec));

  tau_vector_free(vec);
}

TEST_CASE(tau_vector_to_buffer)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  tau_vector_t* vec = tau_vector_init_from_buffer(buffer, 3);

  void* dst[3];
  tau_vector_to_buffer(vec, dst);

  TEST_ASSERT_PTR_EQUAL(dst[0], &data1);
  TEST_ASSERT_PTR_EQUAL(dst[1], &data2);
  TEST_ASSERT_PTR_EQUAL(dst[2], &data3);

  tau_vector_free(vec);
}

TEST_MAIN()
{
  TEST_RUN(tau_vector_init);
  TEST_RUN(tau_vector_init_from_buffer_empty);
  TEST_RUN(tau_vector_init_from_buffer);
  TEST_RUN(tau_vector_push);
  TEST_RUN(tau_vector_pop);
  TEST_RUN(tau_vector_set);
  TEST_RUN(tau_vector_insert);
  TEST_RUN(tau_vector_remove);
  TEST_RUN(tau_vector_clear);
  TEST_RUN(tau_vector_to_buffer);
}
