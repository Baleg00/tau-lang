#include "test.h"

#include "utils/memory/arena.h"

test()
  describe("arena")
    describe("arena_init")
      it("should initialize an arena with default capacity")
        arena_t* arena = arena_init();

        assert_not_null(arena);
        
        arena_free(arena);
      end()
    end()

    describe("arena_init_with_capacity and arena_capacity")
      it("should initialize an arena with specified capacity")
        size_t capacity = 1024;
        arena_t* arena = arena_init_with_capacity(capacity);

        assert_not_null(arena);
        assert_equal(arena_capacity(arena), capacity);
        
        arena_free(arena);
      end()
    end()

    describe("allocator_allocate")
      it("should allocate memory from the arena")
        arena_t* arena = arena_init();

        size_t size = 16;
        void* mem = arena_alloc(arena, size);
        
        assert_not_null(mem);
        
        arena_free(arena);
      end()

      it("should return NULL if allocation fails due to insufficient capacity")
        arena_t* arena = arena_init();

        size_t capacity = arena_capacity(arena);
        void* mem = arena_alloc(arena, capacity + 1);
        
        assert_null(mem);
        
        arena_free(arena);
      end()

      it("should extend arena with default capacity if arena is full")
        arena_t* arena = arena_init();

        size_t capacity = arena_capacity(arena);
        void* mem1 = arena_alloc(arena, capacity);

        assert_not_null(mem1);

        size_t size = 16;
        void* mem2 = arena_alloc(arena, size);

        assert_not_null(mem2);

        arena_free(arena);
      end()

      it("should extend arena with specified capacity if arena is full")
        size_t capacity = 1024;
        arena_t* arena = arena_init_with_capacity(capacity);

        void* mem1 = arena_alloc(arena, capacity);

        assert_not_null(mem1);

        size_t size = 16;
        void* mem2 = arena_alloc(arena, size);

        assert_not_null(mem2);

        arena_free(arena);
      end()
    end()
  end()
end()

