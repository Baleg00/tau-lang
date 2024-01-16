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

#include "utils/memory/allocator.h"

/**
 * \brief Initializes a new arena allocator.
 * 
 * \returns Pointer to the initialized arena allocator.
 */
allocator_t* arena_init(void);

/**
 * \brief Initializes a new arena allocator with a specified capacity.
 * 
 * \param[in] cap The capacity of the arena in bytes.
 * \returns Pointer to the initialized arena allocator.
 */
allocator_t* arena_init_with_capacity(size_t cap);

/**
 * \brief Retrieves the capacity of an arena allocator.
 * 
 * \param[in] alloc Pointer to the arena allocator.
 * \returns The capacity of the arena allocator in bytes.
 */
size_t arena_capacity(allocator_t* alloc);

#endif