#include "test.h"

#include "utils/collections/list.h"

void iter_increment(void* data)
{
  ++*(int*)data;
}

TEST_CASE(list_init)
{
  list_t* list = list_init();

  TEST_ASSERT_NOT_NULL(list);
  TEST_ASSERT_EQUAL(list_size(list), 0);

  list_free(list);
}

TEST_CASE(list_init_from_buffer)
{
  int data1 = 1, data2 = 2, data3 = 3;
  void* buffer[] = { &data1, &data2, &data3 };

  list_t* list = list_init_from_buffer(buffer, 3);
  TEST_ASSERT_EQUAL(list_size(list), 3);
  list_node_t* node = list_front_node(list);
  TEST_ASSERT_PTR_EQUAL(list_node_get(node), &data1);
  node = list_node_next(node);
  TEST_ASSERT_PTR_EQUAL(list_node_get(node), &data2);
  node = list_node_next(node);
  TEST_ASSERT_PTR_EQUAL(list_node_get(node), &data3);
  node = list_node_next(node);
  TEST_ASSERT_NULL(node);

  list_free(list);
}

TEST_CASE(list_init_from_buffer_null)
{
  list_t* list = list_init_from_buffer(NULL, 0);

  TEST_ASSERT_EQUAL(list_size(list), 0);

  list_free(list);
}

TEST_CASE(list_size)
{
  int data1, data2, data3;

  list_t* list = list_init();

  TEST_ASSERT_EQUAL(list_size(list), 0);
  list_push_back(list, &data1);
  TEST_ASSERT_EQUAL(list_size(list), 1);
  list_push_back(list, &data2);
  TEST_ASSERT_EQUAL(list_size(list), 2);
  list_push_back(list, &data3);
  TEST_ASSERT_EQUAL(list_size(list), 3);
  list_pop_back(list);
  TEST_ASSERT_EQUAL(list_size(list), 2);
  list_pop_back(list);
  TEST_ASSERT_EQUAL(list_size(list), 1);
  list_pop_back(list);
  TEST_ASSERT_EQUAL(list_size(list), 0);

  list_free(list);
}

TEST_CASE(list_push_back)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_back(list, &data1);
  TEST_ASSERT_PTR_EQUAL(list_back(list), &data1);
  TEST_ASSERT_EQUAL(list_size(list), 1);
  list_push_back(list, &data2);
  TEST_ASSERT_PTR_EQUAL(list_back(list), &data2);
  TEST_ASSERT_EQUAL(list_size(list), 2);
  list_push_back(list, &data3);
  TEST_ASSERT_PTR_EQUAL(list_back(list), &data3);
  TEST_ASSERT_EQUAL(list_size(list), 3);

  list_free(list);
}

TEST_CASE(list_pop_back)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_push_back(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_pop_back(list), &data3);
  TEST_ASSERT_EQUAL(list_size(list), 2);
  TEST_ASSERT_PTR_EQUAL(list_pop_back(list), &data2);
  TEST_ASSERT_EQUAL(list_size(list), 1);
  TEST_ASSERT_PTR_EQUAL(list_pop_back(list), &data1);
  TEST_ASSERT_EQUAL(list_size(list), 0);

  list_free(list);
}

TEST_CASE(list_push_front)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_front(list, &data1);
  TEST_ASSERT_PTR_EQUAL(list_front(list), &data1);
  TEST_ASSERT_EQUAL(list_size(list), 1);
  list_push_front(list, &data2);
  TEST_ASSERT_PTR_EQUAL(list_front(list), &data2);
  TEST_ASSERT_EQUAL(list_size(list), 2);
  list_push_front(list, &data3);
  TEST_ASSERT_PTR_EQUAL(list_front(list), &data3);
  TEST_ASSERT_EQUAL(list_size(list), 3);

  list_free(list);
}

TEST_CASE(list_pop_front)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_front(list, &data1);
  list_push_front(list, &data2);
  list_push_front(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_pop_front(list), &data3);
  TEST_ASSERT_EQUAL(list_size(list), 2);
  TEST_ASSERT_PTR_EQUAL(list_pop_front(list), &data2);
  TEST_ASSERT_EQUAL(list_size(list), 1);
  TEST_ASSERT_PTR_EQUAL(list_pop_front(list), &data1);
  TEST_ASSERT_EQUAL(list_size(list), 0);

  list_free(list);
}

TEST_CASE(list_insert_before_head)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_node_t* node1 = list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_push_back(list, &data3);

  int data4 = 4;

  list_node_t* new_head = list_insert_before(node1, &data4);
  TEST_ASSERT_PTR_EQUAL(list_front(list), &data4);

  TEST_ASSERT_PTR_EQUAL(list_node_next(new_head), node1);
  TEST_ASSERT_PTR_EQUAL(list_node_prev(node1), new_head);

  list_free(list);
}

TEST_CASE(list_insert_before_middle)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_node_t* node1 = list_push_back(list, &data1);
  list_node_t* node2 = list_push_back(list, &data2);
  list_push_back(list, &data3);

  int data4 = 4;

  list_node_t* new_node = list_insert_before(node2, &data4);

  TEST_ASSERT_PTR_EQUAL(list_node_next(node1), new_node);
  TEST_ASSERT_PTR_EQUAL(list_node_prev(node2), new_node);

  TEST_ASSERT_PTR_EQUAL(list_node_next(new_node), node2);
  TEST_ASSERT_PTR_EQUAL(list_node_prev(new_node), node1);

  list_free(list);
}

TEST_CASE(list_insert_after_tail)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_node_t* node3 = list_push_back(list, &data3);

  int data4 = 4;

  list_node_t* new_tail = list_insert_after(node3, &data4);
  TEST_ASSERT_PTR_EQUAL(list_back(list), &data4);

  TEST_ASSERT_PTR_EQUAL(list_node_next(node3), new_tail);
  TEST_ASSERT_PTR_EQUAL(list_node_prev(new_tail), node3);

  list_free(list);
}

TEST_CASE(list_insert_after_middle)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_node_t* node1 = list_push_back(list, &data1);
  list_node_t* node2 = list_push_back(list, &data2);
  list_push_back(list, &data3);

  int data4 = 4;

  list_node_t* new_node = list_insert_after(node1, &data4);

  TEST_ASSERT_PTR_EQUAL(list_node_next(node1), new_node);
  TEST_ASSERT_PTR_EQUAL(list_node_prev(node2), new_node);

  TEST_ASSERT_PTR_EQUAL(list_node_next(new_node), node2);
  TEST_ASSERT_PTR_EQUAL(list_node_prev(new_node), node1);

  list_free(list);
}

TEST_CASE(list_remove_head)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_node_t* node1 = list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_push_back(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_remove(node1), &data1);
  TEST_ASSERT_PTR_EQUAL(list_front(list), &data2);
  TEST_ASSERT_NULL(list_node_prev(list_front_node(list)));

  list_free(list);
}

TEST_CASE(list_remove_tail)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_node_t* node3 = list_push_back(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_remove(node3), &data3);
  TEST_ASSERT_PTR_EQUAL(list_back(list), &data2);
  TEST_ASSERT_NULL(list_node_next(list_back_node(list)));

  list_free(list);
}

TEST_CASE(list_remove_middle)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_node_t* node1 = list_push_back(list, &data1);
  list_node_t* node2 = list_push_back(list, &data2);
  list_node_t* node3 = list_push_back(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_remove(node2), &data2);
  TEST_ASSERT_EQUAL(list_node_next(node1), node3);
  TEST_ASSERT_EQUAL(list_node_prev(node3), node1);

  list_free(list);
}

TEST_CASE(list_remove_before_head)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_back(list, &data1);
  list_node_t* node2 = list_push_back(list, &data2);
  list_push_back(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_remove_before(node2), &data1);
  TEST_ASSERT_PTR_EQUAL(list_front(list), &data2);
  TEST_ASSERT_NULL(list_node_prev(node2));

  list_free(list);
}

TEST_CASE(list_remove_before_middle)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_node_t* node1 = list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_node_t* node3 = list_push_back(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_remove_before(node3), &data2);
  TEST_ASSERT_PTR_EQUAL(list_node_next(node1), node3);
  TEST_ASSERT_PTR_EQUAL(list_node_prev(node3), node1);

  list_free(list);
}

TEST_CASE(list_remove_after_tail)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_back(list, &data1);
  list_node_t* node2 = list_push_back(list, &data2);
  list_push_back(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_remove_after(node2), &data3);
  TEST_ASSERT_PTR_EQUAL(list_back(list), &data2);
  TEST_ASSERT_NULL(list_node_next(node2));

  list_free(list);
}

TEST_CASE(list_remove_after_middle)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_node_t* node1 = list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_node_t* node3 = list_push_back(list, &data3);

  TEST_ASSERT_PTR_EQUAL(list_remove_after(node1), &data2);
  TEST_ASSERT_PTR_EQUAL(list_node_next(node1), node3);
  TEST_ASSERT_PTR_EQUAL(list_node_prev(node3), node1);

  list_free(list);
}

TEST_CASE(list_for_each)
{
  list_t* list = list_init();

  int data1 = 1, data2 = 2, data3 = 3;

  list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_push_back(list, &data3);

  list_for_each(list, iter_increment);

  TEST_ASSERT_EQUAL(data1, 2);
  TEST_ASSERT_EQUAL(data2, 3);
  TEST_ASSERT_EQUAL(data3, 4);

  list_free(list);
}

TEST_CASE(list_for_each_empty)
{
  list_t* list = list_init();

  list_for_each(list, iter_increment);

  TEST_ASSERT_TRUE(list_empty(list));

  list_free(list);
}

TEST_CASE(list_to_buffer)
{
  int data1 = 1, data2 = 2, data3 = 3;

  list_t* list = list_init();

  list_push_back(list, &data1);
  list_push_back(list, &data2);
  list_push_back(list, &data3);

  void* buffer[3];

  list_to_buffer(list, buffer);

  TEST_ASSERT_PTR_EQUAL(buffer[0], &data1);
  TEST_ASSERT_PTR_EQUAL(buffer[1], &data2);
  TEST_ASSERT_PTR_EQUAL(buffer[2], &data3);

  list_free(list);
}

TEST_MAIN()
{
  TEST_RUN(list_init);
  TEST_RUN(list_init_from_buffer);
  TEST_RUN(list_init_from_buffer_null);
  TEST_RUN(list_size);
  TEST_RUN(list_push_back);
  TEST_RUN(list_pop_back);
  TEST_RUN(list_push_front);
  TEST_RUN(list_pop_front);
  TEST_RUN(list_insert_before_head);
  TEST_RUN(list_insert_before_middle);
  TEST_RUN(list_insert_after_tail);
  TEST_RUN(list_insert_after_middle);
  TEST_RUN(list_remove_head);
  TEST_RUN(list_remove_tail);
  TEST_RUN(list_remove_middle);
  TEST_RUN(list_remove_before_head);
  TEST_RUN(list_remove_after_tail);
  TEST_RUN(list_remove_after_middle);
  TEST_RUN(list_for_each);
  TEST_RUN(list_for_each_empty);
  TEST_RUN(list_to_buffer);
}
