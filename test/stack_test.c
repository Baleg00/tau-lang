#include "test.h"

#include "stack.h"

test()

  stack_t* stack;

  describe("stack")
    describe("stack_init")
      before_each()
        stack = stack_init();
      end()

      it("should not return NULL")
        assert_not_null(stack);

        stack_free(stack);
      end()
    end()

    describe("stack_push")
      before_each()
        stack = stack_init();
      end()

      it("should push data onto the stack")
        int data = 1;

        stack_push(stack, &data);

        assert_false(stack_empty(stack));

        stack_free(stack);
      end()

      it("should push multiple items onto the stack")
        int data1 = 1, data2 = 2, data3 = 3;
        
        stack_push(stack, &data1);
        stack_push(stack, &data2);
        stack_push(stack, &data3);
        
        assert_false(stack_empty(stack));
        
        stack_free(stack);
      end()
    end()

    describe("stack_pop")
      before_each()
        stack = stack_init();
      end()
      
      it("should pop the top item from the stack")
        int data = 1;
        
        stack_push(stack, &data);
        void* popped_data = stack_pop(stack);
        
        assert_equal(popped_data, &data);
        
        stack_free(stack);
      end()
    end()

    describe("stack_peek")
      before_each()
        stack = stack_init();
      end()
      
      it("should return the top item from the stack without removing it")
        int data = 1;
        
        stack_push(stack, &data);
        void* top_data = stack_peek(stack);
        
        assert_equal(top_data, &data);
        assert_false(stack_empty(stack));
        
        stack_free(stack);
      end()
    end()

    describe("stack_empty")
      before_each()
        stack = stack_init();
      end()
      
      it("should return true if stack is empty")
        assert_true(stack_empty(stack));
        
        stack_free(stack);
      end()

      it("should return false if stack is not empty")
        int data = 1;
        stack_push(stack, &data);
        
        assert_false(stack_empty(stack));
        
        stack_free(stack);
      end()
    end()
  end()

end()

