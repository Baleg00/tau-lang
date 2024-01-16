#include "test.h"

#include "utils/collections/queue.h"

test()

  queue_t* queue;

  describe("queue")
    describe("queue_init")
      before_each()
        queue = queue_init();
      end()

      it("should not return NULL")
        assert_not_null(queue);

        queue_free(queue);
      end()
    end()

    describe("queue_offer")
      before_each()
        queue = queue_init();
      end()
      
      it("should add data to the queue")
        int data = 1;

        queue_offer(queue, &data);
        assert_false(queue_empty(queue));
        
        queue_free(queue);
      end()

      it("should add multiple items to the queue")
        int data1 = 1, data2 = 2, data3 = 3;

        queue_offer(queue, &data1);
        queue_offer(queue, &data2);
        queue_offer(queue, &data3);

        assert_false(queue_empty(queue));

        queue_free(queue);
      end()
    end()

    describe("queue_poll")
      before_each()
        queue = queue_init();
      end()
      
      it("should remove and return the head of the queue")
        int data = 1;

        queue_offer(queue, &data);
        void* polled_data = queue_poll(queue);
        
        assert_equal(polled_data, &data);
        
        queue_free(queue);
      end()
    end()

    describe("queue_peek")
      before_each()
        queue = queue_init();
      end()
      
      it("should return the head of the queue without removing it")
        int data = 1;

        queue_offer(queue, &data);
        void* peeked_data = queue_peek(queue);
        
        assert_equal(peeked_data, &data);
        assert_false(queue_empty(queue));
        
        queue_free(queue);
      end()
    end()

    describe("queue_empty")
      before_each()
        queue = queue_init();
      end()
      
      it("should return true if the queue is empty")
        assert_true(queue_empty(queue));

        queue_free(queue);
      end()

      it("should return false if the queue is not empty")
        int data = 1;
        
        queue_offer(queue, &data);
        assert_false(queue_empty(queue));
        
        queue_free(queue);
      end()
    end()
  end()

end()
