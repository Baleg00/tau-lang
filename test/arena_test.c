#include "test.h"

#include "utils/memory/arena.h"

TEST_CASE(arena_init)
{
  arena_t* arena = arena_init();

  TEST_ASSERT_NOT_NULL(arena);

  arena_free(arena);
}

TEST_CASE(arena_init_with_capacity)
{
  size_t capacity = 1024;
  arena_t* arena = arena_init_with_capacity(capacity);

  TEST_ASSERT_NOT_NULL(arena);
  TEST_ASSERT_EQUAL(arena_capacity(arena), capacity);

  arena_free(arena);
}

TEST_CASE(arena_alloc)
{
  arena_t* arena = arena_init();

  size_t size = 16;
  void* mem = arena_alloc(arena, size);

  TEST_ASSERT_NOT_NULL(mem);

  arena_free(arena);
}

TEST_CASE(arena_alloc_fail)
{
  arena_t* arena = arena_init();

  size_t capacity = arena_capacity(arena);
  void* mem = arena_alloc(arena, capacity + 1);

  TEST_ASSERT_NULL(mem);

  arena_free(arena);
}

TEST_CASE(arena_alloc_extend)
{
  arena_t* arena = arena_init();

  size_t capacity = arena_capacity(arena);
  void* mem1 = arena_alloc(arena, capacity);

  TEST_ASSERT_NOT_NULL(mem1);

  size_t size = 16;
  void* mem2 = arena_alloc(arena, size);

  TEST_ASSERT_NOT_NULL(mem2);

  arena_free(arena);
}

TEST_CASE(arena_capacity_alloc_extend)
{
  size_t capacity = 1024;
  arena_t* arena = arena_init_with_capacity(capacity);

  void* mem1 = arena_alloc(arena, capacity);

  TEST_ASSERT_NOT_NULL(mem1);

  size_t size = 16;
  void* mem2 = arena_alloc(arena, size);

  TEST_ASSERT_NOT_NULL(mem2);

  arena_free(arena);
}

TEST_MAIN()
{
  TEST_RUN(arena_init);
  TEST_RUN(arena_init_with_capacity);
  TEST_RUN(arena_alloc);
  TEST_RUN(arena_alloc_extend);
  TEST_RUN(arena_capacity_alloc_extend);
}
