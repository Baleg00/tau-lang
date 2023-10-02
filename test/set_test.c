#include "test.h"

#include "set.h"

int cmp_int(void* lhs, void* rhs)
{
  return (*(int*)lhs) - (*(int*)rhs);
}

void iter_increment(void* data)
{
  ++(*(int*)data);
}

test()
  describe("set")
    describe("set_init")
      it("should initialize a new set")
        set_t* set = set_init(cmp_int);
        assert_not_null(set);
        set_free(set);
      end()
    end()

    describe("set_add")
      it("should add elements to the set")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        assert_true(set_add(set, &data1));
        assert_true(set_add(set, &data5));
        assert_true(set_add(set, &data4));
        assert_true(set_add(set, &data3));
        assert_true(set_add(set, &data2));

        set_free(set);
      end()

      it("should not add duplicate elements to the set")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        assert_false(set_add(set, &data1));
        set_add(set, &data4);
        assert_false(set_add(set, &data5));
        set_add(set, &data3);
        set_add(set, &data2);
        assert_false(set_add(set, &data2));
        assert_false(set_add(set, &data1));

        set_free(set);
      end()
    end()

    describe("set_remove")
      it("should remove elements from the set")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        set_add(set, &data4);
        set_add(set, &data3);
        set_add(set, &data2);

        assert_true(set_remove(set, &data1));
        assert_false(set_contains(set, &data1));

        set_free(set);
      end()

      it("should not remove elements not present in the set")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        set_add(set, &data4);

        assert_false(set_contains(set, &data2));
        assert_false(set_remove(set, &data2));

        assert_false(set_contains(set, &data3));
        assert_false(set_remove(set, &data3));

        set_free(set);
      end()
    end()

    describe("set_get")
      it("should retrieve elements")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        set_add(set, &data4);

        assert_ptr_equal(set_get(set, &data1), &data1);
        assert_ptr_equal(set_get(set, &data5), &data5);
        assert_ptr_equal(set_get(set, &data4), &data4);

        set_free(set);
      end()

      it("should not retrieve elements")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        set_add(set, &data4);

        assert_null(set_get(set, &data2));
        assert_null(set_get(set, &data3));

        set_free(set);
      end()
    end()

    describe("set_contains")
      it("should check if the set contains elements")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        set_add(set, &data4);

        assert_true(set_contains(set, &data1));
        assert_true(set_contains(set, &data5));
        assert_true(set_contains(set, &data4));
        assert_false(set_contains(set, &data2));
        assert_false(set_contains(set, &data3));

        set_free(set);
      end()
    end()

    describe("set_min")
      it("should return the minimum element in the set")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        set_add(set, &data4);
        set_add(set, &data3);
        set_add(set, &data2);

        int* min_data = (int*)set_min(set);
        assert_not_null(min_data);
        assert_equal(*min_data, data1);

        set_free(set);
      end()

      it("should return NULL for empty set")
        set_t* set = set_init(cmp_int);

        int* min_data = (int*)set_min(set);
        assert_null(min_data);

        set_free(set);
      end()
    end()

    describe("set_max")
      it("should return the maximum element in the set")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        set_add(set, &data4);
        set_add(set, &data3);
        set_add(set, &data2);

        int* max_data = (int*)set_max(set);
        assert_not_null(max_data);
        assert_equal(*max_data, data5);

        set_free(set);
      end()
      
      it("should return NULL for empty set")
        set_t* set = set_init(cmp_int);

        int* max_data = (int*)set_max(set);
        assert_null(max_data);

        set_free(set);
      end()
    end()

    describe("set_empty")
      it("should check if the set is empty")
        set_t* set = set_init(cmp_int);
        assert_true(set_empty(set));

        int data = 5;
        set_add(set, &data);

        assert_false(set_empty(set));

        set_free(set);
      end()
    end()

    describe("set_size")
      it("should return the size of the set")
        set_t* set = set_init(cmp_int);
        assert_equal(set_size(set), 0);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        assert_equal(set_size(set), 0);
        
        set_add(set, &data1);
        assert_equal(set_size(set), 1);
        
        set_add(set, &data5);
        assert_equal(set_size(set), 2);
        
        set_add(set, &data4);
        assert_equal(set_size(set), 3);
        
        set_add(set, &data3);
        assert_equal(set_size(set), 4);
        
        set_add(set, &data2);
        assert_equal(set_size(set), 5);

        set_remove(set, &data4);
        assert_equal(set_size(set), 4);

        set_remove(set, &data1);
        assert_equal(set_size(set), 3);

        set_remove(set, &data2);
        assert_equal(set_size(set), 2);

        set_remove(set, &data5);
        assert_equal(set_size(set), 1);

        set_remove(set, &data3);
        assert_equal(set_size(set), 0);

        set_free(set);
      end()
    end()

    describe("set_for_each")
      it("should apply a function to each element in the set")
        set_t* set = set_init(cmp_int);

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;
        int data4 = 4;
        int data5 = 5;

        set_add(set, &data1);
        set_add(set, &data5);
        set_add(set, &data4);
        set_add(set, &data3);
        set_add(set, &data2);

        set_for_each(set, iter_increment);

        assert_equal(data1, 2);
        assert_equal(data2, 3);
        assert_equal(data3, 4);
        assert_equal(data4, 5);
        assert_equal(data5, 6);

        set_free(set);
      end()
    end()
  end()
end()
