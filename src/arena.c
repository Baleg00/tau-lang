/**
 * \file arena.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "arena.h"

#include <string.h>

#include "log.h"
#include "memtrace.h"
#include "util.h"

struct arena_s
{
  void* begin; // Begin pointer of chunk.
  void* end; // Past end pointer of chunk.
  void* ptr; // Pointer to first available byte in chunk.
  arena_t* next; // Pointer to next arena or null pointer.
  // The actual memory chunk is allocated after the arena.
};

/** Maximum possible alignment of any object. */
#define ARENA_MAX_ALIGN (alignof(union { uintmax_t a; uintptr_t b; long double c; }))

/**
 * \brief Aligns a pointer to ARENA_MAX_ALIGN.
 * 
 * \param[in] PTR Pointer to be aligned.
 * \returns Aligned pointer.
*/
#define arena_align(PTR) ((void*)(((uintptr_t)(PTR) + ARENA_MAX_ALIGN - 1) & ~(ARENA_MAX_ALIGN - 1)))

/**
 * \brief Rounds a size up to the nearest multiple of ARENA_MAX_ALIGN.
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

void* arena_malloc(arena_t* arena, size_t size)
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
    arena = arena_init_capacity(arena_capacity(last));
    last->next = arena;
  }

  void* ptr = arena->ptr;
  arena->ptr = (void*)((uintptr_t)arena->ptr + aligned_size);

  return ptr;
}

void* arena_calloc(arena_t* arena, size_t count, size_t size)
{
  void* ptr = arena_malloc(arena, count * size);

  if (ptr == NULL)
    return NULL;

  memset(ptr, 0, count * size);

  return ptr;
}
