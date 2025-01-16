/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/memory/arena.h"

#include "utils/common.h"

/**
 * \brief Default capacity of an arena allocator.
 */
#define ARENA_DEFAULT_CAPACITY (8 * (1 << 10))

/**
 * \brief Represents a memory chunk in an arena allocator.
 *
 * \details The member variables of a chunk act like a header and the actual
 * memory from which the arena allocates follows right after.
 */
typedef struct arena_chunk_t arena_chunk_t;

struct arena_chunk_t
{
  size_t size; // Number of allocated bytes in the chunk.
  arena_chunk_t* next; // Pointer to the next chunk or `NULL`.
};

struct arena_t
{
  size_t capacity; // Capacity of a chunk.
  arena_chunk_t* head; // Pointer to the first chunk.
};

/**
 * \brief Rounds a size up to the nearest multiple of a specified alignment.
 * 
 * \param[in] size Size to be round up.
 * \param[in] alignment The alignment requirement.
 * \returns Rounded up size.
*/
static size_t arena_round(size_t size, size_t alignment)
{
  return (size + alignment - 1) / alignment * alignment;
}

/**
 * \brief Initializes a new chunk with a specified capacity.
 * 
 * \param[in] cap The capacity of the chunk.
 * \returns Pointer to the newly initialized chunk.
 */
static arena_chunk_t* arena_chunk_init_with_capacity(size_t cap)
{
  size_t chunk_size = arena_round(sizeof(arena_chunk_t), ALIGNOF(max_align_t)) + cap;

  arena_chunk_t* chunk = (arena_chunk_t*)malloc(chunk_size);
  ASSERT(chunk != NULL);

  chunk->size = 0;
  chunk->next = NULL;

  return chunk;
}

arena_t* arena_init(void)
{
  return arena_init_with_capacity(ARENA_DEFAULT_CAPACITY);
}

arena_t* arena_init_with_capacity(size_t cap)
{
  arena_t* arena = (arena_t*)malloc(sizeof(arena_t));
  ASSERT(arena != NULL);

  arena->capacity = cap;
  arena->head = NULL;

  return arena;
}

void arena_free(arena_t* arena)
{
  for (arena_chunk_t *chunk = arena->head, *next = NULL; chunk != NULL; chunk = next)
  {
    next = chunk->next;
    free(chunk);
  }

  free(arena);
}

size_t arena_capacity(arena_t* arena)
{
  return arena->capacity;
}

bool arena_owns(arena_t* arena, void* ptr)
{
  for (arena_chunk_t* chunk = arena->head; chunk != NULL; chunk = chunk->next)
  {
    void* begin = (uint8_t*)chunk + arena_round(sizeof(arena_chunk_t), ALIGNOF(max_align_t));
    void* end = (uint8_t*)begin + chunk->size;

    if (begin <= ptr && ptr < end)
      return true;
  }

  return false;
}

void* arena_alloc(arena_t* arena, size_t size)
{
  return arena_alloc_aligned(arena, size, ALIGNOF(max_align_t));
}

void* arena_alloc_aligned(arena_t* arena, size_t size, size_t alignment)
{
  if (size > arena->capacity)
  {
    log_warn("arena", "Allocation size larger than capacity.");

    return NULL;
  }

  if (arena->head == NULL)
    arena->head = arena_chunk_init_with_capacity(arena->capacity);

  arena_chunk_t* last_chunk = NULL;

  for (arena_chunk_t* chunk = arena->head; chunk != NULL; last_chunk = chunk, chunk = chunk->next)
  {
    size_t new_size = arena_round(chunk->size, MIN(ALIGNOF(max_align_t), alignment)) + size;

    if (new_size > arena->capacity)
      continue;

    void* ptr = (uint8_t*)chunk + arena_round(sizeof(arena_chunk_t), ALIGNOF(max_align_t)) + new_size - size;

    chunk->size = new_size;

    return ptr;
  }

  arena_chunk_t* chunk = arena_chunk_init_with_capacity(arena->capacity);
  last_chunk->next = chunk;

  void* ptr = (uint8_t*)chunk + arena_round(sizeof(arena_chunk_t), ALIGNOF(max_align_t));

  chunk->size = size;

  return ptr;
}
