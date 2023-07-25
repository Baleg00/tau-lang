/**
 * \file arena.h
 * 
 * \brief Arena allocator interface.
 * 
 * \details An arena allocator is a memory allocation strategy that allocates
 * memory from a fixed-size block of memory called an arena. It is particularly
 * useful in situations where a large number of objects are allocated and
 * deallocated together as a group, such as within a specific scope or during
 * the execution of a function. Instead of individually allocating and
 * deallocating memory for each object, the arena allocator allocates memory
 * in bulk for the entire group. This improves memory allocation and
 * deallocation efficiency by reducing the overhead associated with frequent
 * memory management operations.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_ARENA_H
#define TAU_ARENA_H

#include <stddef.h>

#define ARENA_DEFAULT_CAPACITY (8 * (1 << 10))

/**
 * \brief Arena allocator.
 */
typedef struct arena_s arena_t;

/**
 * \brief Initializes a new arena.
 * 
 * \returns Pointer to the initialized arena.
 */
arena_t* arena_init(void);

/**
 * \brief Initializes a new arena with a specified capacity.
 * 
 * \param[in] cap The capacity of the arena in bytes.
 * \returns Pointer to the initialized arena.
 */
arena_t* arena_init_capacity(size_t cap);

/**
 * \brief Frees the memory allocated by an arena.
 * 
 * \param[in] arena Pointer to the arena to be freed.
 */
void arena_free(arena_t* arena);

/**
 * \brief Retrieves the capacity of an arena.
 * 
 * \param[in] arena Pointer to the arena.
 * \returns The capacity of the arena in bytes.
 */
size_t arena_capacity(arena_t* arena);

/**
 * \brief Allocates memory from an arena.
 * 
 * \param[in] arena Pointer to the arena.
 * \param[in] size The size of memory to allocate in bytes.
 * \returns Pointer to the allocated memory, or NULL if allocation fails.
 */
void* arena_malloc(arena_t* arena, size_t size);

/**
 * \brief Allocates and initializes memory with zeros from an arena.
 * 
 * \param[in] arena Pointer to the arena.
 * \param[in] count The number of elements to allocate.
 * \param[in] size The size of each element in bytes.
 * \returns Pointer to the allocated memory, or NULL if allocation fails.
 */
void* arena_calloc(arena_t* arena, size_t count, size_t size);

#endif