#include "arena.h"

#include "memtrace.h"
#include "util.h"
#include "log.h"

#define ARENA_DEFAULT_CAPACITY (8 * (1 << 10))

#ifdef __alignof_is_defined

#include <stdalign.h>
#define ARENA_MAX_ALIGN (alignof(max_align_t))

#else

typedef union arena_max_align_u
{
  uint8_t x;

  union
  {
    uintmax_t a;
    double b;
    long double c;
    void* d;
    void(*e)(void);
  } y;
} arena_max_align_t;

#define ARENA_MAX_ALIGN (offsetof(arena_max_align_t, y))

#endif

/**
 * \brief Aligns a pointer to `ARENA_MAX_ALIGN`.
 * 
 * \param[in] PTR Pointer to be aligned.
 * \returns Aligned pointer.
*/
#define arena_align(PTR) ((void*)(((uintptr_t)(PTR) + ARENA_MAX_ALIGN - 1) & ~(ARENA_MAX_ALIGN - 1)))

/**
 * \brief Rounds a size up to the nearest multiple of `ARENA_MAX_ALIGN`.
 * 
 * \param[in] SIZE Size to be round up.
 * \returns Rounded up size.
*/
#define arena_round(SIZE) (((size_t)(SIZE) + ARENA_MAX_ALIGN - 1) / ARENA_MAX_ALIGN * ARENA_MAX_ALIGN)

arena_t* arena_init(void)
{
  return arena_init_capacity(ARENA_DEFAULT_CAPACITY);
}

arena_t* arena_init_capacity(size_t cap)
{
  size_t aligned_size = arena_round(sizeof(arena_t) + cap);

  arena_t* arena = (arena_t*)malloc(aligned_size);
  assert(arena != NULL);

  void* unaligned_ptr = (void*)((uintptr_t)arena + sizeof(arena_t));

  arena->begin = arena_align(unaligned_ptr);
  arena->end = (void*)((uintptr_t)arena + aligned_size);
  arena->ptr = arena->begin;
  arena->next = NULL;

  return arena;
}

void arena_free(arena_t* arena)
{
  for (arena_t *it = arena, *next; it != NULL; it = next)
  {
    next = it->next;
    free(it);
  }
}

size_t arena_capacity(arena_t* arena)
{
  return (size_t)((uintptr_t)arena->end - (uintptr_t)arena->begin);
}

void* arena_alloc(arena_t* arena, size_t size)
{
  size_t aligned_size = arena_round(size);

  if (arena_capacity(arena) < aligned_size)
  {
    log_error("arena", "Allocation size exceeds capacity! (size: %zu, capacity: %zu)", aligned_size, arena_capacity(arena));
    return NULL;
  }

  arena_t* last = NULL;

  while (arena != NULL && (uintptr_t)arena->ptr + aligned_size >= (uintptr_t)arena->end)
  {
    last = arena;
    arena = arena->next;
  }

  if (arena == NULL)
  {
    arena = arena_init_capacity(arena_capacity(arena));
    last->next = arena;
  }

  void* ptr = arena->ptr;
  arena->ptr = (void*)((uintptr_t)arena->ptr + aligned_size);

  return ptr;
}
