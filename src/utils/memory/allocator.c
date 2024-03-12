/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/memory/allocator.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

struct allocator_t
{
  void* ctx; // Pointer to the allocator context data.
  allocator_allocate_func_t alloc_func; // Pointer to the allocation function.
  allocator_deallocate_func_t dealloc_func; // Pointer to the deallocation function.
  allocator_reallocate_func_t realloc_func; // Pointer to the reallocation function.
  allocator_cleanup_func_t cleanup_func; // Pointer to the cleanup function.
};

/**
 * \brief Global memory allocator.
 */
static allocator_t* g_allocator = NULL;

allocator_t* allocator_init(void* ctx, allocator_allocate_func_t alloc_func, allocator_deallocate_func_t dealloc_func, allocator_reallocate_func_t realloc_func, allocator_cleanup_func_t cleanup_func)
{
  allocator_t* alloc = (allocator_t*)malloc(sizeof(allocator_t));
  ASSERT(alloc != NULL);

  alloc->ctx = ctx;
  alloc->alloc_func = alloc_func;
  alloc->dealloc_func = dealloc_func;
  alloc->realloc_func = realloc_func;
  alloc->cleanup_func = cleanup_func;

  return alloc;
}

void allocator_free(allocator_t* alloc)
{
  ASSERT(alloc != g_allocator);

  alloc->cleanup_func(alloc->ctx);
  free(alloc);
}

/**
 * \brief Allocates memory using the global memory allocator.
 * 
 * \param[in] ctx Pointer to the allocator context data.
 * \param[in] size Number of bytes to allocate.
 * \returns Pointer to the allocated memory.
 */
static void* allocator_global_allocate(void* ctx, size_t size)
{
  UNUSED(ctx);
  return malloc(size);
}

/**
 * \brief Deallocates memory using the global memory allocator.
 * 
 * \param[in] ctx Pointer to the allocator context data.
 * \param[in] ptr Pointer to the memory to deallocate.
 */
static void allocator_global_deallocate(void* ctx, void* ptr)
{
  UNUSED(ctx);
  free(ptr);
}

/**
 * \brief Reallocates memory using the global memory allocator.
 * 
 * \param[in] ctx Pointer to the allocator context data.
 * \param[in] ptr Pointer to the memory to reallocate.
 * \param[in] new_size The new size of the memory block.
 * \returns Pointer to the reallocated memory.
 */
static void* allocator_global_reallocate(void* ctx, void* ptr, size_t new_size)
{
  UNUSED(ctx);
  return realloc(ptr, new_size);
}

/**
 * \brief Cleans up memory allocated by the global memory allocator.
 * 
 * \param[in] ctx Pointer to the allocator context data.
 */
static void allocator_global_cleanup(void* ctx)
{
  UNUSED(ctx);
}

/**
 * \brief Frees the global memory allocator at program exit.
 */
static void allocator_global_atexit(void)
{
  if (g_allocator != NULL)
    free(g_allocator);
}

allocator_t* allocator_global(void)
{
  if (g_allocator == NULL)
  {
    g_allocator = allocator_init(NULL,
      (allocator_allocate_func_t)allocator_global_allocate,
      (allocator_deallocate_func_t)allocator_global_deallocate,
      (allocator_reallocate_func_t)allocator_global_reallocate,
      (allocator_cleanup_func_t)allocator_global_cleanup);
    atexit(allocator_global_atexit);
  }

  return g_allocator;
}

void* allocator_context(allocator_t* alloc)
{
  return alloc->ctx;
}

void* allocator_allocate(allocator_t* alloc, size_t size)
{
  return alloc->alloc_func(alloc->ctx, size);
}

void allocator_deallocate(allocator_t* alloc, void* ptr)
{
  alloc->dealloc_func(alloc->ctx, ptr);
}

void* allocator_reallocate(allocator_t* alloc, void* ptr, size_t new_size)
{
  return alloc->realloc_func(alloc->ctx, ptr, new_size);
}
