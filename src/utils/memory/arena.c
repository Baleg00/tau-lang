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
typedef struct tau_arena_chunk_t tau_arena_chunk_t;

struct tau_arena_chunk_t
{
  size_t size; // Number of allocated bytes in the chunk.
  tau_arena_chunk_t* next; // Pointer to the next chunk or `NULL`.
};

struct tau_arena_t
{
  size_t capacity; // Capacity of a chunk.
  tau_arena_chunk_t* head; // Pointer to the first chunk.
};

/**
 * \brief Rounds a size up to the nearest multiple of a specified alignment.
 * 
 * \param[in] size Size to be round up.
 * \param[in] alignment The alignment requirement.
 * \returns Rounded up size.
*/
static size_t tau_arena_round(size_t size, size_t alignment)
{
  return (size + alignment - 1) / alignment * alignment;
}

/**
 * \brief Initializes a new chunk with a specified capacity.
 * 
 * \param[in] cap The capacity of the chunk.
 * \returns Pointer to the newly initialized chunk.
 */
static tau_arena_chunk_t* tau_arena_chunk_init_with_capacity(size_t cap)
{
  size_t chunk_size = tau_arena_round(sizeof(tau_arena_chunk_t), TAU_ALIGNOF(max_align_t)) + cap;

  tau_arena_chunk_t* chunk = (tau_arena_chunk_t*)malloc(chunk_size);
  TAU_ASSERT(chunk != NULL);

  chunk->size = 0;
  chunk->next = NULL;

  return chunk;
}

tau_arena_t* tau_arena_init(void)
{
  return tau_arena_init_with_capacity(ARENA_DEFAULT_CAPACITY);
}

tau_arena_t* tau_arena_init_with_capacity(size_t cap)
{
  tau_arena_t* arena = (tau_arena_t*)malloc(sizeof(tau_arena_t));
  TAU_ASSERT(arena != NULL);

  arena->capacity = cap;
  arena->head = NULL;

  return arena;
}

void tau_arena_free(tau_arena_t* arena)
{
  for (tau_arena_chunk_t *chunk = arena->head, *next = NULL; chunk != NULL; chunk = next)
  {
    next = chunk->next;
    free(chunk);
  }

  free(arena);
}

size_t tau_arena_capacity(tau_arena_t* arena)
{
  return arena->capacity;
}

bool tau_arena_owns(tau_arena_t* arena, void* ptr)
{
  for (tau_arena_chunk_t* chunk = arena->head; chunk != NULL; chunk = chunk->next)
  {
    void* begin = (uint8_t*)chunk + tau_arena_round(sizeof(tau_arena_chunk_t), TAU_ALIGNOF(max_align_t));
    void* end = (uint8_t*)begin + chunk->size;

    if (begin <= ptr && ptr < end)
      return true;
  }

  return false;
}

void* tau_arena_alloc(tau_arena_t* arena, size_t size)
{
  return tau_arena_alloc_aligned(arena, size, TAU_ALIGNOF(max_align_t));
}

void* tau_arena_alloc_aligned(tau_arena_t* arena, size_t size, size_t alignment)
{
  if (size > arena->capacity)
  {
    tau_log_warn("arena", "Allocation size larger than capacity.");

    return NULL;
  }

  if (arena->head == NULL)
    arena->head = tau_arena_chunk_init_with_capacity(arena->capacity);

  tau_arena_chunk_t* last_chunk = NULL;

  for (tau_arena_chunk_t* chunk = arena->head; chunk != NULL; last_chunk = chunk, chunk = chunk->next)
  {
    size_t new_size = tau_arena_round(chunk->size, TAU_MIN(TAU_ALIGNOF(max_align_t), alignment)) + size;

    if (new_size > arena->capacity)
      continue;

    void* ptr = (uint8_t*)chunk + tau_arena_round(sizeof(tau_arena_chunk_t), TAU_ALIGNOF(max_align_t)) + new_size - size;

    chunk->size = new_size;

    return ptr;
  }

  tau_arena_chunk_t* chunk = tau_arena_chunk_init_with_capacity(arena->capacity);
  last_chunk->next = chunk;

  void* ptr = (uint8_t*)chunk + tau_arena_round(sizeof(tau_arena_chunk_t), TAU_ALIGNOF(max_align_t));

  chunk->size = size;

  return ptr;
}
