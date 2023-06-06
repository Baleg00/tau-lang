#include "test.h"

#include "list.h"

void iter_increment(void* data)
{
  ++(*(int*)data);
}

test()

  list_t* list;

  describe("list")
    describe("list_init")
      before_each()
        list = list_init();
      end()

      it("should not return NULL")
        assert_not_null(list);

        list_free(list);
      end()

      it("should return an empty list")
        assert_equal(list_size(list), 0);

        list_free(list);
      end()
    end()
      
    describe("list_size")
      before_each()
        list = list_init();
      end()

      it("should return 0 for empty list")
        assert_equal(list_size(list), 0);

        list_free(list);
      end()

      it("should not return 0 for non-empty list")
        int data1, data2, data3;
        
        assert_equal(list_size(list), 0);
        list_push_back(list, &data1);
        assert_equal(list_size(list), 1);
        list_push_back(list, &data2);
        assert_equal(list_size(list), 2);
        list_push_back(list, &data3);
        assert_equal(list_size(list), 3);
        list_pop_back(list);
        assert_equal(list_size(list), 2);
        list_pop_back(list);
        assert_equal(list_size(list), 1);
        list_pop_back(list);
        assert_equal(list_size(list), 0);

        list_free(list);
      end()
    end()

    describe("list_push_back")
      before_each()
        list = list_init();
      end()

      it("should insert node after tail")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        assert_equal(*(int*)list_back(list), data1);
        list_push_back(list, &data2);
        assert_equal(*(int*)list_back(list), data2);
        list_push_back(list, &data3);
        assert_equal(*(int*)list_back(list), data3);

        list_free(list);
      end()

      it("should increase list size")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        assert_equal(list_size(list), 1);
        list_push_back(list, &data2);
        assert_equal(list_size(list), 2);
        list_push_back(list, &data3);
        assert_equal(list_size(list), 3);

        list_free(list);
      end()
    end()

    describe("list_pop_back")
      before_each()
        list = list_init();
      end()

      it("should remove tail")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_push_back(list, &data3);

        assert_equal(*(int*)list_pop_back(list), data3);
        assert_equal(*(int*)list_pop_back(list), data2);
        assert_equal(*(int*)list_pop_back(list), data1);

        list_free(list);
      end()

      it("should decrease list size")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_push_back(list, &data3);

        list_pop_back(list);
        assert_equal(list_size(list), 2);
        list_pop_back(list);
        assert_equal(list_size(list), 1);
        list_pop_back(list);
        assert_equal(list_size(list), 0);

        list_free(list);
      end()
    end()

    describe("list_push_front")
      before_each()
        list = list_init();
      end()

      it("should insert node before head")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_front(list, &data1);
        assert_equal(*(int*)list_front(list), data1);
        list_push_front(list, &data2);
        assert_equal(*(int*)list_front(list), data2);
        list_push_front(list, &data3);
        assert_equal(*(int*)list_front(list), data3);

        list_free(list);
      end()

      it("should increase list size")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_front(list, &data1);
        assert_equal(list_size(list), 1);
        list_push_front(list, &data2);
        assert_equal(list_size(list), 2);
        list_push_front(list, &data3);
        assert_equal(list_size(list), 3);

        list_free(list);
      end()
    end()

    describe("list_pop_front")
      before_each()
        list = list_init();
      end()

      it("should remove head")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_front(list, &data1);
        list_push_front(list, &data2);
        list_push_front(list, &data3);

        assert_equal(*(int*)list_pop_front(list), data3);
        assert_equal(*(int*)list_pop_front(list), data2);
        assert_equal(*(int*)list_pop_front(list), data1);

        list_free(list);
      end()

      it("should decrease list size")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_front(list, &data1);
        list_push_front(list, &data2);
        list_push_front(list, &data3);

        list_pop_front(list);
        assert_equal(list_size(list), 2);
        list_pop_front(list);
        assert_equal(list_size(list), 1);
        list_pop_front(list);
        assert_equal(list_size(list), 0);

        list_free(list);
      end()
    end()

    describe("list_insert_before")
      before_each()
        list = list_init();
      end()

      it("should insert node before head")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_node_t* node1 = list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_push_back(list, &data3);

        int data4 = 4;

        list_node_t* new_head = list_insert_before(node1, &data4);
        assert_equal(*(int*)list_front(list), data4);

        assert_equal(list_node_next(new_head), node1);
        assert_equal(list_node_prev(node1), new_head);

        list_free(list);
      end()
      
      it("should insert between nodes")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_node_t* node1 = list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_push_back(list, &data3);

        int data4 = 4;

        list_node_t* new_node = list_insert_before(node2, &data4);
        
        assert_equal(list_node_next(node1), new_node);
        assert_equal(list_node_prev(node2), new_node);

        assert_equal(list_node_next(new_node), node2);
        assert_equal(list_node_prev(new_node), node1);

        list_free(list);
      end()

      it("should increase list size")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_push_back(list, &data3);

        int data4 = 4, data5 = 5;

        list_insert_before(node2, &data4);
        list_insert_before(list_front_node(list), &data5);

        assert_equal(list_size(list), 5);

        list_free(list);
      end()
    end()

    describe("list_insert_after")
      before_each()
        list = list_init();
      end()

      it("should insert node after tail")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_node_t* node3 = list_push_back(list, &data3);

        int data4 = 4;

        list_node_t* new_tail = list_insert_after(node3, &data4);
        assert_equal(*(int*)list_back(list), data4);

        assert_equal(list_node_next(node3), new_tail);
        assert_equal(list_node_prev(new_tail), node3);

        list_free(list);
      end()
      
      it("should insert between nodes")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_node_t* node1 = list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_push_back(list, &data3);

        int data4 = 4;

        list_node_t* new_node = list_insert_after(node1, &data4);
        
        assert_equal(list_node_next(node1), new_node);
        assert_equal(list_node_prev(node2), new_node);

        assert_equal(list_node_next(new_node), node2);
        assert_equal(list_node_prev(new_node), node1);

        list_free(list);
      end()

      it("should increase list size")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_push_back(list, &data3);

        int data4 = 4, data5 = 5;

        list_insert_after(node2, &data4);
        list_insert_after(list_back_node(list), &data5);

        assert_equal(list_size(list), 5);

        list_free(list);
      end()
    end()

    describe("list_remove")
      before_each()
        list = list_init();
      end()

      it("should remove head")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_node_t* node1 = list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_push_back(list, &data3);

        assert_equal(*(int*)list_remove(node1), data1);
        assert_equal(*(int*)list_front(list), data2);
        assert_null(list_node_prev(list_front_node(list)));

        list_free(list);
      end()
      
      it("should remove tail")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_node_t* node3 = list_push_back(list, &data3);

        assert_equal(*(int*)list_remove(node3), data3);
        assert_equal(*(int*)list_back(list), data2);
        assert_null(list_node_next(list_back_node(list)));

        list_free(list);
      end()
      
      it("should remove intermediate node")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_node_t* node1 = list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_node_t* node3 = list_push_back(list, &data3);

        assert_equal(*(int*)list_remove(node2), data2);
        assert_equal(list_node_next(node1), node3);
        assert_equal(list_node_prev(node3), node1);

        list_free(list);
      end()

      it("should decrease list size")
        int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5;
        
        list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_node_t* node3 = list_push_back(list, &data3);
        list_push_back(list, &data4);
        list_push_back(list, &data5);

        list_remove(node3);
        list_remove(list_front_node(list));
        list_remove(list_back_node(list));

        assert_equal(list_size(list), 2);

        list_free(list);
      end()
    end()

    describe("list_remove_before")
      before_each()
        list = list_init();
      end()
      
      it("should remove head")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_push_back(list, &data3);

        assert_equal(*(int*)list_remove_before(node2), data1);
        assert_equal(*(int*)list_front(list), data2);
        assert_null(list_node_prev(node2));

        list_free(list);
      end()

      it("should remove intermediate node")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_node_t* node1 = list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_node_t* node3 = list_push_back(list, &data3);

        assert_equal(*(int*)list_remove_before(node3), data2);
        assert_equal(list_node_next(node1), node3);
        assert_equal(list_node_prev(node3), node1);

        list_free(list);
      end()

      it("should decrease list size")
        int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5;
        
        list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_push_back(list, &data3);
        list_node_t* node4 = list_push_back(list, &data4);
        list_push_back(list, &data5);

        list_remove_before(node2);
        list_remove_before(node4);

        assert_equal(list_size(list), 3);

        list_free(list);
      end()
    end()

    describe("list_remove_after")
      before_each()
        list = list_init();
      end()
      
      it("should remove tail")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_push_back(list, &data3);

        assert_equal(*(int*)list_remove_after(node2), data3);
        assert_equal(*(int*)list_back(list), data2);
        assert_null(list_node_next(node2));

        list_free(list);
      end()

      it("should remove intermediate node")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_node_t* node1 = list_push_back(list, &data1);
        list_push_back(list, &data2);
        list_node_t* node3 = list_push_back(list, &data3);

        assert_equal(*(int*)list_remove_after(node1), data2);
        assert_equal(list_node_next(node1), node3);
        assert_equal(list_node_prev(node3), node1);

        list_free(list);
      end()

      it("should decrease list size")
        int data1 = 1, data2 = 2, data3 = 3, data4 = 4, data5 = 5;
        
        list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_push_back(list, &data3);
        list_node_t* node4 = list_push_back(list, &data4);
        list_push_back(list, &data5);

        list_remove_before(node2);
        list_remove_before(node4);

        assert_equal(list_size(list), 3);

        list_free(list);
      end()
    end()

    describe("list_for_each")
      before_each()
        list = list_init();
      end()
      
      it("should iterate through list")
        int data1 = 1, data2 = 2, data3 = 3;
        
        list_node_t* node1 = list_push_back(list, &data1);
        list_node_t* node2 = list_push_back(list, &data2);
        list_node_t* node3 = list_push_back(list, &data3);

        list_for_each(list, iter_increment);

        assert_equal(*(int*)list_node_get(node1), 2);
        assert_equal(*(int*)list_node_get(node2), 3);
        assert_equal(*(int*)list_node_get(node3), 4);

        list_free(list);
      end()
      
      it("should work with empty list")
        list_for_each(list, iter_increment);

        assert_true(list_empty(list));

        list_free(list);
      end()
      
      it("should work with NULL")
        list_free(list);

        list_t* list = NULL;

        list_for_each(list, iter_increment);
      end()
    end()
  end()

end()
