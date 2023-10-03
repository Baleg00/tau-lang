/**
 * \file allocator.h
 * 
 * \brief Abstract allocator library interface.
 * 
 * \details The abstract allocator library provides a unified interface for
 * managing memory allocation and deallocation in a flexible and customizable
 * way. Developers can easily switch between different memory allocation
 * strategies without having to rewrite major parts of the program. This makes
 * it easier to experiment with different memory management techniques, optimize
 * memory usage, and maintain consistent memory management practices across the
 * entire codebase.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_ALLOCATOR_H
#define TAU_ALLOCATOR_H

#include <stdint.h>

/**
 * \brief Function type for allocating memory.
 *
 * \param[in] ctx Context data to the allocator.
 * \param[in] size Number of bytes to allocate.
 * \returns A pointer to the allocated memory, or NULL if allocation failed.
 */
typedef void*(*allocator_allocate_func_t)(void* /* ctx */, size_t /* size */);

/**
 * \brief Function type for deallocating memory.
 *
 * \param[in] ctx Context data passed to the allocator.
 * \param[in] ptr Pointer to the memory to deallocate.
 */
typedef void(*allocator_deallocate_func_t)(void* /* ctx */, void* /* ptr */);

/**
 * \brief Function type for reallocating memory.
 *
 * \param[in] ctx Context data passed to the allocator.
 * \param[in] ptr Pointer to the memory to deallocate.
 * \param[in] new_size The new size of the memory block.
 */
typedef void*(*allocator_reallocate_func_t)(void* /* ctx */, void* /* ptr */, size_t /* new_size */);

/**
 * \brief Function type for allocator cleanup.
 *
 * \param[in] ctx Context data.
 */
typedef void(*allocator_cleanup_func_t)(void* /* ctx */);

/**
 * \brief Memory allocator structure.
 */
typedef struct allocator_t allocator_t;

/**
 * \brief Initializes a memory allocator with the provided functions and
 * allocator object.
 *
 * \param[in] ctx Context data to be passed to the allocator functions.
 * \param[in] alloc_func Allocator function for memory allocation.
 * \param[in] dealloc_func Deallocator function for memory deallocation.
 * \param[in] realloc_func Reallocator function for memory reallocation.
 * \returns A pointer to the initialized allocator.
 */
allocator_t* allocator_init(void* ctx, allocator_allocate_func_t alloc_func, allocator_deallocate_func_t dealloc_func, allocator_reallocate_func_t realloc_func, allocator_cleanup_func_t cleanup_func);

/**
 * \brief Frees the resources associated with the allocator.
 *
 * \param[in] alloc Pointer to the allocator to be freed.
 */
void allocator_free(allocator_t* alloc);

/**
 * \brief Retrieves the global memory allocator.
 *
 * \returns A pointer to the global allocator.
 */
allocator_t* allocator_global(void);

/**
 * \brief Retrieves the context associated with an allocator.
 *
 * \param[in] alloc Pointer to the allocator.
 * \returns A pointer to the associated context.
 */
void* allocator_context(allocator_t* alloc);

/**
 * \brief Allocates memory using the specified allocator.
 *
 * \param[in] alloc Pointer to the allocator.
 * \param[in] size Number of bytes to allocate.
 * \returns A pointer to the allocated memory.
 */
void* allocator_allocate(allocator_t* alloc, size_t size);

/**
 * \brief Deallocates memory using the specified allocator.
 *
 * \param[in] alloc Pointer to the allocator.
 * \param[in] ptr Pointer to the memory to deallocate.
 */
void allocator_deallocate(allocator_t* alloc, void* ptr);

/**
 * \brief Reallocates memory using the specified allocator.
 *
 * \param[in] alloc Pointer to the allocator.
 * \param[in] ptr Pointer to the memory to reallocate.
 * \param[in] new_size The new size of the memory block.
 * \returns A pointer to the reallocated memory.
 */
void* allocator_reallocate(allocator_t* alloc, void* ptr, size_t new_size);

#endif