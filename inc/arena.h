/**
 * \file
 * 
 * Arena allocator.
*/

#ifndef TAU_ARENA_H
#define TAU_ARENA_H

#include <stddef.h>
#include <stdint.h>

#include "typedefs.h"

#define ARENA_DEFAULT_CAPACITY (8 * (1 << 10))

struct arena_s
{
  void* begin; // Begin pointer of chunk.
  void* end; // Past end pointer of chunk.
  void* ptr; // Pointer to first available byte in chunk.
  arena_t* next; // Pointer to next arena or null pointer.
  // The actual memory chunk is allocated after the arena.
};

/**
 * \brief Initializes a new arena.
 * 
 * \returns New arena.
*/
arena_t* arena_init(void);

/**
 * \brief Initializes a new arena with the specified capacity.
 * 
 * \param[in] cap Capacity in bytes.
 * \returns New arena.
*/
arena_t* arena_init_capacity(size_t cap);

/**
 * \brief Destroys an arena.
 * 
 * \param[in] arena Arena to be destroyed.
*/
void arena_free(arena_t* arena);

/**
 * \brief Returns the capacity of an arena.
 * 
 * \param[in] arena Arena to be used.
 * \returns Arena capacity.
*/
size_t arena_capacity(arena_t* arena);

/**
 * \brief Allocates memory using an arena.
 * 
 * \param[in] arena Arena to be used.
 * \param[in] size Number of bytes to be allocated.
 * \returns Pointer to newly allocated memory or null pointer if allocation
 * failed.
*/
void* arena_malloc(arena_t* arena, size_t size);

/**
 * \brief Allocates memory for count object of size using an arena and
 * initializes all bytes to zero.
 * 
 * \param[in] arena Arena to be used.
 * \param[in] count Number of objects to be allocated.
 * \param[in] size Size of an object in bytes.
 * \returns Pointer to newly allocated memory or null pointer if allocation 
 * failed.
*/
void* arena_calloc(arena_t* arena, size_t count, size_t size);

#endif