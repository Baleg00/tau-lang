/**
 * \file
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

#include <stdbool.h>
#include <stddef.h>

#include "utils/extern_c.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Represents an arena allocator.
 */
typedef struct tau_arena_t tau_arena_t;

/**
 * \brief Initializes a new arena allocator.
 * 
 * \returns Pointer to the initialized arena allocator.
 */
tau_arena_t* tau_arena_init(void);

/**
 * \brief Initializes a new arena allocator with a specified capacity.
 * 
 * \param[in] cap The capacity of the arena in bytes.
 * \returns Pointer to the initialized arena allocator.
 */
tau_arena_t* tau_arena_init_with_capacity(size_t cap);

/**
 * \brief Frees all memory associated with an arena allocator.
 *
 * \param[in] arena Pointer to the arena allocator to be freed.
 */
void tau_arena_free(tau_arena_t* arena);

/**
 * \brief Retrieves the capacity of an arena allocator.
 * 
 * \param[in] arena Pointer to the arena allocator.
 * \returns The capacity of the arena allocator in bytes.
 */
size_t tau_arena_capacity(tau_arena_t* arena);

/**
 * \brief Checks if an arena owns a pointer (aka it points to a memory region
 * that was allocated by the arena).
 *
 * \param[in] arena Pointer to the arena allocator.
 * \param[in] ptr The pointer to be checked.
 * \returns `true` if the arena owns the pointer, `false` otherwise.
 */
bool tau_arena_owns(tau_arena_t* arena, void* ptr);

/**
 * \brief Allocates memory using an arena allocator.
 *
 * \param[in] arena Pointer to the arena allocator.
 * \param[in] size The number of bytes to be allocated.
 * \returns Pointer to the newly allocated memory or `NULL`.
 */
void* tau_arena_alloc(tau_arena_t* arena, size_t size);

/**
 * \brief Allocates aligned memory using an arena allocator.
 *
 * \param[in] arena Pointer to the arena allocator.
 * \param[in] size The number of bytes to be allocated.
 * \param[in] alignment The alignment requirement.
 * \returns Pointer to the newly allocated memory or `NULL`.
 */
void* tau_arena_alloc_aligned(tau_arena_t* arena, size_t size, size_t alignment);

TAU_EXTERN_C_END

#endif
