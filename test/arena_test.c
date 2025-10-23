#include "test.h"

#include "utils/memory/arena.h"

TEST_CASE(tau_arena_init)
{
  tau_arena_t* arena = tau_arena_init();

  TEST_ASSERT_NOT_NULL(arena);

  tau_arena_free(arena);
}

TEST_CASE(tau_arena_init_with_capacity)
{
  size_t capacity = 1024;
  tau_arena_t* arena = tau_arena_init_with_capacity(capacity);

  TEST_ASSERT_NOT_NULL(arena);
  TEST_ASSERT_EQUAL(tau_arena_capacity(arena), capacity);

  tau_arena_free(arena);
}

TEST_CASE(tau_arena_alloc)
{
  tau_arena_t* arena = tau_arena_init();

  size_t size = 16;
  void* mem = tau_arena_alloc(arena, size);

  TEST_ASSERT_NOT_NULL(mem);

  tau_arena_free(arena);
}

TEST_CASE(tau_arena_alloc_fail)
{
  tau_arena_t* arena = tau_arena_init();

  size_t capacity = tau_arena_capacity(arena);
  void* mem = tau_arena_alloc(arena, capacity + 1);

  TEST_ASSERT_NULL(mem);

  tau_arena_free(arena);
}

TEST_CASE(tau_arena_alloc_extend)
{
  tau_arena_t* arena = tau_arena_init();

  size_t capacity = tau_arena_capacity(arena);
  void* mem1 = tau_arena_alloc(arena, capacity);

  TEST_ASSERT_NOT_NULL(mem1);

  size_t size = 16;
  void* mem2 = tau_arena_alloc(arena, size);

  TEST_ASSERT_NOT_NULL(mem2);

  tau_arena_free(arena);
}

TEST_CASE(tau_arena_capacity_alloc_extend)
{
  size_t capacity = 1024;
  tau_arena_t* arena = tau_arena_init_with_capacity(capacity);

  void* mem1 = tau_arena_alloc(arena, capacity);

  TEST_ASSERT_NOT_NULL(mem1);

  size_t size = 16;
  void* mem2 = tau_arena_alloc(arena, size);

  TEST_ASSERT_NOT_NULL(mem2);

  tau_arena_free(arena);
}

TEST_MAIN()
{
  TEST_RUN(tau_arena_init);
  TEST_RUN(tau_arena_init_with_capacity);
  TEST_RUN(tau_arena_alloc);
  TEST_RUN(tau_arena_alloc_extend);
  TEST_RUN(tau_arena_capacity_alloc_extend);
}
