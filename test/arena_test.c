#include "test.h"

#include "utils/memory/arena.h"

test()
  describe("arena")
    describe("arena_init")
      it("should initialize an arena with default capacity")
        allocator_t* arena = arena_init();

        assert_not_null(arena);
        
        allocator_free(arena);
      end()
    end()

    describe("arena_init_with_capacity and arena_capacity")
      it("should initialize an arena with specified capacity")
        size_t capacity = 1024;
        allocator_t* arena = arena_init_with_capacity(capacity);

        assert_not_null(arena);
        assert_equal(arena_capacity(arena), capacity);
        
        allocator_free(arena);
      end()
    end()

    describe("allocator_allocate")
      it("should allocate memory from the arena")
        allocator_t* arena = arena_init();

        size_t size = 16;
        void* mem = allocator_allocate(arena, size);
        
        assert_not_null(mem);
        
        allocator_free(arena);
      end()

      it("should return NULL if allocation fails due to insufficient capacity")
        allocator_t* arena = arena_init();

        size_t capacity = arena_capacity(arena);
        void* mem = allocator_allocate(arena, capacity + 1);
        
        assert_null(mem);
        
        allocator_free(arena);
      end()

      it("should extend arena with default capacity if arena is full")
        allocator_t* arena = arena_init();

        size_t capacity = arena_capacity(arena);
        void* mem1 = allocator_allocate(arena, capacity);

        assert_not_null(mem1);

        size_t size = 16;
        void* mem2 = allocator_allocate(arena, size);

        assert_not_null(mem2);

        allocator_free(arena);
      end()

      it("should extend arena with specified capacity if arena is full")
        size_t capacity = 1024;
        allocator_t* arena = arena_init_with_capacity(capacity);

        void* mem1 = allocator_allocate(arena, capacity);

        assert_not_null(mem1);

        size_t size = 16;
        void* mem2 = allocator_allocate(arena, size);

        assert_not_null(mem2);

        allocator_free(arena);
      end()
    end()
  end()
end()

