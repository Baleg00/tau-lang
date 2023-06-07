#include "test.h"

#include "arena.h"

test()
  describe("arena")
    describe("arena_init")
      it("should initialize an arena with default capacity")
        arena_t* arena = arena_init();

        assert_not_null(arena);
        
        arena_free(arena);
      end()
    end()

    describe("arena_init_capacity")
      it("should initialize an arena with specified capacity")
        size_t capacity = 1024;
        arena_t* arena = arena_init_capacity(capacity);

        assert_not_null(arena);
        
        arena_free(arena);
      end()
    end()

    describe("arena_capacity")
      it("should return default capacity")
        arena_t* arena = arena_init();

        assert_equal(arena_capacity(arena), ARENA_DEFAULT_CAPACITY);

        arena_free(arena);
      end()

      it("should return specified capacity")
        size_t capacity = 1024;
        arena_t* arena = arena_init_capacity(capacity);

        assert_equal(arena_capacity(arena), capacity);

        arena_free(arena);
      end()
    end()

    describe("arena_malloc")
      it("should allocate memory from the arena")
        arena_t* arena = arena_init();

        size_t size = 16;
        void* mem = arena_malloc(arena, size);
        
        assert_not_null(mem);
        
        arena_free(arena);
      end()

      it("should return NULL if allocation fails due to insufficient capacity")
        arena_t* arena = arena_init();

        size_t capacity = arena_capacity(arena);
        void* mem = arena_malloc(arena, capacity + 1);
        
        assert_null(mem);
        
        arena_free(arena);
      end()

      it("should extend arena with default capacity if arena is full")
        arena_t* arena = arena_init();

        size_t capacity = arena_capacity(arena);
        void* mem1 = arena_malloc(arena, capacity);

        assert_not_null(mem1);

        size_t size = 16;
        void* mem2 = arena_malloc(arena, size);

        assert_not_null(mem2);

        arena_free(arena);
      end()

      it("should extend arena with specified capacity if arena is full")
        size_t capacity = 1024;
        arena_t* arena = arena_init_capacity(capacity);

        void* mem1 = arena_malloc(arena, capacity);

        assert_not_null(mem1);

        size_t size = 16;
        void* mem2 = arena_malloc(arena, size);

        assert_not_null(mem2);

        arena_free(arena);
      end()
    end()

    describe("arena_calloc")
      it("should allocate and zero-initialize memory from the arena")
        arena_t* arena = arena_init();

        size_t count = 4;
        size_t size = 8;
        void* mem = arena_calloc(arena, count, size);
        
        assert_not_null(mem);
        
        unsigned char* byte_ptr = (unsigned char*)mem;
        
        for (size_t i = 0; i < count * size; ++i)
          assert_equal(byte_ptr[i], 0);

        arena_free(arena);
      end()

      it("should return NULL if allocation fails due to insufficient capacity")
        arena_t* arena = arena_init();

        size_t capacity = arena_capacity(arena);
        size_t count = capacity / sizeof(int);
        void* mem = arena_calloc(arena, count + 1, sizeof(int));
        
        assert_null(mem);
        
        arena_free(arena);
      end()
    end()
  end()
end()

