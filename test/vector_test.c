#include "test.h"

#include "utils/collections/vector.h"

test()
  describe("vector")
    describe("vector_init")
      it("should initialize a new vector")
        vector_t* vec = vector_init();

        assert_not_null(vec);

        vector_free(vec);
      end()
    end()

    describe("vector_init_from_buffer")
      it("should initialize vector from empty buffer")
        vector_t* vec = vector_init_from_buffer(NULL, 0);

        assert_not_null(vec);

        vector_free(vec);
      end()

      it("should initialize vector from non-empty buffer")
        int data1 = 1;
        int data2 = 2;
        int data3 = 3;

        void* buffer[] = { &data1, &data2, &data3 };

        vector_t* vec = vector_init_from_buffer(buffer, 3);

        assert_not_null(vec);
        assert_equal(vector_size(vec), 3);
        assert_equal(vector_get(vec, 0), &data1);
        assert_equal(vector_get(vec, 1), &data2);
        assert_equal(vector_get(vec, 2), &data3);

        vector_free(vec);
      end()
    end()

    describe("vector_push, vector_get")
      it("should add elements to the vector and retrieve them")
        vector_t* vec = vector_init();

        int data1 = 5;
        int data2 = 10;

        vector_push(vec, &data1);
        vector_push(vec, &data2);

        assert_equal(vector_size(vec), 2);
        assert_equal(*(int*)vector_get(vec, 0), data1);
        assert_equal(*(int*)vector_get(vec, 1), data2);

        vector_free(vec);
      end()
    end()

    describe("vector_set")
      it("should set elements in the vector")
        vector_t* vec = vector_init();

        int data1 = 5;
        int data2 = 10;

        vector_push(vec, &data1);
        vector_push(vec, &data2);

        int new_data = 20;
        vector_set(vec, 0, &new_data);

        assert_equal(vector_size(vec), 2);
        assert_equal(*(int*)vector_get(vec, 0), new_data);

        vector_free(vec);
      end()
    end()

    describe("vector_pop")
      it("should remove and return the last element from the vector")
        vector_t* vec = vector_init();

        int data1 = 5;
        int data2 = 10;

        vector_push(vec, &data1);
        vector_push(vec, &data2);

        void* popped = vector_pop(vec);

        assert_equal(vector_size(vec), 1);
        assert_equal(*(int*)popped, data2);

        vector_free(vec);
      end()
    end()

    describe("vector_insert and vector_remove")
      it("should insert and remove elements from the vector")
        vector_t* vec = vector_init();

        int data1 = 5;
        int data2 = 10;
        int data3 = 15;

        vector_push(vec, &data1);
        vector_push(vec, &data2);

        vector_insert(vec, 1, &data3);

        assert_equal(vector_size(vec), 3);
        assert_equal(*(int*)vector_get(vec, 1), data3);

        void* removed = vector_remove(vec, 1);

        assert_equal(vector_size(vec), 2);
        assert_equal(*(int*)removed, data3);

        vector_free(vec);
      end()
    end()

    describe("vector_clear and vector_empty")
      it("should clear the vector and check if it's empty")
        vector_t* vec = vector_init();

        int data1 = 5;
        int data2 = 10;

        vector_push(vec, &data1);
        vector_push(vec, &data2);

        vector_clear(vec);

        assert_true(vector_empty(vec));
        assert_equal(vector_size(vec), 0);

        vector_free(vec);
      end()
    end()

    describe("vector_size")
      it("should return the number of elements in the vector")
        vector_t* vec = vector_init();

        int data1 = 5;
        int data2 = 10;

        vector_push(vec, &data1);
        vector_push(vec, &data2);

        assert_equal(vector_size(vec), 2);

        vector_free(vec);
      end()
    end()

    describe("vector_to_buffer")
      it("should write vector to buffer")
        vector_t* vec = vector_init();

        int data1 = 1;
        int data2 = 2;
        int data3 = 3;

        vector_push(vec, &data1);
        vector_push(vec, &data2);
        vector_push(vec, &data3);

        void* buffer[3];

        vector_to_buffer(vec, buffer);

        assert_ptr_equal(buffer[0], &data1);
        assert_ptr_equal(buffer[1], &data2);
        assert_ptr_equal(buffer[2], &data3);

        vector_free(vec);
      end()
    end()
  end()
end()
