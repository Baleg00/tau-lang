/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/memory/arena.h"

#include <string.h>

#include "utils/common.h"
#include "utils/io/log.h"
#include "utils/memory/memtrace.h"

/**
 * \brief Default capacity of an arena allocator.
 */
#define ARENA_DEFAULT_CAPACITY (8 * (1 << 10))

/**
 * \brief Maximum possible alignment of any object.
 */
#define ARENA_MAX_ALIGN (ALIGNOF(union { uintmax_t imax; void* ptr; long double ld; }))

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

/**
 * \brief Arena allocator chunk.
 */
typedef struct arena_chunk_t arena_chunk_t;

struct arena_chunk_t
{
  void* begin; // Begin pointer of chunk.
  void* end; // End pointer of chunk.
  size_t capacity; // Chunk capacity.
  void* ptr; // Pointer to first available byte in chunk.
  arena_chunk_t* next; // Pointer to next arena or NULL.
  // The actual memory chunk is allocated after the arena.
};

/**
 * \brief Initializes a new chunk with a specified capacity.
 * 
 * \param[in] cap The capacity of the chunk.
 * \returns Pointer to the initialized chunk.
 */
static arena_chunk_t* arena_chunk_init_with_capacity(size_t cap)
{
  size_t aligned_size = arena_round(sizeof(arena_chunk_t) + cap);

  arena_chunk_t* chunk = (arena_chunk_t*)malloc(aligned_size);
  ASSERT(chunk != NULL);

  void* unaligned_ptr = (void*)((uintptr_t)chunk + sizeof(arena_chunk_t));

  chunk->begin = arena_align(unaligned_ptr);
  chunk->end = (void*)((uintptr_t)chunk + aligned_size);
  chunk->capacity = (size_t)((uintptr_t)chunk->end - (uintptr_t)chunk->begin);
  chunk->ptr = chunk->begin;
  chunk->next = NULL;

  return chunk;
}

/**
 * \brief Allocates memory using using an arena allocator.
 * 
 * \param[in] chunk Pointer to the first chunk of the arena allocator.
 * \param[in] size Number of bytes to allocate.
 * \returns Pointer to the allocated memory.
 */
static void* arena_allocate(arena_chunk_t* chunk, size_t size)
{
  size_t aligned_size = arena_round(size);

  if (chunk->capacity < aligned_size)
  {
    log_error("arena", "Allocation size exceeds capacity! (size: %zu, capacity: %zu)", aligned_size, chunk->capacity);
    return NULL;
  }

  arena_chunk_t* last = NULL;

  while (chunk != NULL && (uintptr_t)chunk->ptr + aligned_size >= (uintptr_t)chunk->end)
  {
    last = chunk;
    chunk = chunk->next;
  }

  if (chunk == NULL)
  {
    chunk = arena_chunk_init_with_capacity(last->capacity);
    last->next = chunk;
  }

  void* ptr = chunk->ptr;
  chunk->ptr = (void*)((uintptr_t)chunk->ptr + aligned_size);

  return ptr;
}

/**
 * \brief Deallocates memory using an arena allocator.
 * 
 * \param[in] chunk Pointer to the first chunk of the arena allocator.
 * \param[in] ptr Pointer to the memory to deallocate.
 */
static void arena_deallocate(arena_chunk_t* chunk, void* ptr)
{
  UNUSED(chunk);
  UNUSED(ptr);
}

/**
 * \brief Reallocates memory using an arena allocator.
 * 
 * \param[in] ctx Pointer to the first chunk of the arena allocator.
 * \param[in] ptr Pointer to the memory to reallocate.
 * \param[in] new_size The new size of the memory block.
 * \returns Pointer to the reallocated memory.
 */
static void* arena_reallocate(arena_chunk_t* chunk, void* ptr, size_t new_size)
{
  UNUSED(chunk);
  UNUSED(ptr);
  UNUSED(new_size);
  return NULL;
}

/**
 * \brief Cleans up memory allocated by an arena allocator.
 * 
 * \param[in] ctx Pointer to the first chunk of the arena allocator.
 */
static void arena_cleanup(arena_chunk_t* chunk)
{
  for (arena_chunk_t* next; chunk != NULL; chunk = next)
  {
    next = chunk->next;
    free(chunk);
  }
}

allocator_t* arena_init(void)
{
  return arena_init_with_capacity(ARENA_DEFAULT_CAPACITY);
}

allocator_t* arena_init_with_capacity(size_t cap)
{
  arena_chunk_t* chunk = arena_chunk_init_with_capacity(cap);
  return allocator_init(chunk,
    (allocator_allocate_func_t)arena_allocate,
    (allocator_deallocate_func_t)arena_deallocate,
    (allocator_reallocate_func_t)arena_reallocate,
    (allocator_cleanup_func_t)arena_cleanup);
}

size_t arena_capacity(allocator_t* alloc)
{
  arena_chunk_t* arena = (arena_chunk_t*)allocator_context(alloc);
  return arena->capacity;
}
