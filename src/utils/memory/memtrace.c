/**
 * \file
 * 
 * \copyright Copyright (c) Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#define TAU_MEMTRACE_IMPL
#include "utils/memory/memtrace.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utils/common.h"
#include "utils/esc_seq.h"
#include "utils/io/log.h"
#include "utils/timer.h"

/**
 * \brief Enumeration of memory allocation kinds.
 */
typedef enum memtrace_alloc_kind_e
{
  MEMTRACE_MALLOC,
  MEMTRACE_CALLOC,
  MEMTRACE_REALLOC,
} memtrace_alloc_kind_t;

/**
 * \brief Allocation metadata.
 */
typedef struct memtrace_meta_t
{
  memtrace_alloc_kind_t alloc_kind; // Allcation kind.
  const char* file; // Path to source file.
  const char* func; // Name of containing function.
  int line; // Line number in source file.
  uint64_t time; // Time of allocation in ticks.
} memtrace_meta_t;

/**
 * \brief Allocation data.
 */
typedef struct memtrace_data_t
{
  void* ptr; // Pointer to memory.
  size_t size; // Size of allocation in bytes.
} memtrace_data_t;

/**
 * \brief Allocation object.
 */
typedef struct memtrace_alloc_t
{
  memtrace_data_t data;
  memtrace_meta_t meta;

  struct memtrace_alloc_t *next; // Pointer to next allocation.
} memtrace_alloc_t;

/**
 * \brief The total amount of memory allocated by the program.
 */
static size_t g_memtrace_stat_total_alloc = 0;

/**
 * \brief The current amount of memory used by the program.
 */
static size_t g_memtrace_stat_cur_alloc = 0;

/**
 * \brief The peak amount of memory used by the program.
 */
static size_t g_memtrace_stat_peak_alloc = 0;

/**
 * \brief The total number of memory allocation requests.
 */
static size_t g_memtrace_stat_alloc_count = 0;

/**
 * \brief The total number of memory deallocation requests.
 */
static size_t g_memtrace_stat_dealloc_count = 0;

/**
 * \brief The sum of the lifetimes of all deallocated memory in ticks.
 */
static uint64_t g_memtrace_stat_total_lifetime = 0;

/**
 * \brief Function to be called at program exit.
 */
void memtrace_atexit(void);

/**
 * \brief Returns the root allocation object.
 */
memtrace_alloc_t* memtrace_alloc_root(void)
{
  static memtrace_alloc_t* root = NULL;

  // Initialize root.
  if (root == NULL)
  {
    root = (memtrace_alloc_t*)calloc(1, sizeof(memtrace_alloc_t));
    atexit(memtrace_atexit);
  }

  return root;
}

void memtrace_atexit(void)
{
  memtrace_alloc_t* root = memtrace_alloc_root();

  log_debug("memtrace", "Total allocated memory: %zu bytes", memtrace_stat_total_alloc());
  log_debug("memtrace", "Peak allocated memory: %zu bytes", memtrace_stat_peak_alloc());
  log_debug("memtrace", "Total allocation count: %zu bytes", memtrace_stat_alloc_count());
  log_debug("memtrace", "Average allocation size: %zu bytes", memtrace_stat_avg_alloc_size());
  log_debug("memtrace", "Average lifetime: %.6g ms", memtrace_stat_avg_lifetime());

  if (root->next == NULL)
    log_debug("memtrace", "No leaks detected.");
  else
  {
    log_debug("memtrace", "Leaks detected!");
    DEBUGBREAK();
  }

  for (memtrace_alloc_t *alloc = root->next, *next; alloc != NULL; alloc = next)
  {
    next = alloc->next;

    log_debug("memtrace", "%zu bytes at %p (%s:%d)", alloc->data.size, alloc->data.ptr, alloc->meta.file, alloc->meta.line);

    free(alloc->data.ptr);
    free(alloc);
  }
}

void* memtrace_malloc(size_t size, const char* file, int line, const char* func)
{
  if (size == 0)
  {
    log_warn("memtrace", "Allocating 0 bytes.");
    DEBUGBREAK();
    return NULL;
  }

  void* ptr = malloc(size);
  
  if (ptr == NULL)
  {
    log_error("memtrace", "Allocation failed.");
    DEBUGBREAK();
    return NULL;
  }

  memtrace_alloc_t* alloc = (memtrace_alloc_t*)malloc(sizeof(memtrace_alloc_t));
  ASSERT(alloc != NULL);
  alloc->data.ptr = ptr;
  alloc->data.size = size;
  alloc->meta.alloc_kind = MEMTRACE_MALLOC;
  alloc->meta.file = file;
  alloc->meta.line = line;
  alloc->meta.func = func;
  alloc->meta.time = timer_now();

  memtrace_alloc_t* root = memtrace_alloc_root();

  alloc->next = root->next;
  root->next = alloc;

  g_memtrace_stat_total_alloc += size;
  g_memtrace_stat_cur_alloc += size;
  g_memtrace_stat_alloc_count++;

  if (g_memtrace_stat_peak_alloc < g_memtrace_stat_cur_alloc)
    g_memtrace_stat_peak_alloc = g_memtrace_stat_cur_alloc;

  return ptr;
}

void* memtrace_calloc(size_t count, size_t size, const char* file, int line, const char* func)
{
  if (count * size == 0)
  {
    log_warn("memtrace", "Allocating 0 bytes.", file, line);
    DEBUGBREAK();
    return NULL;
  }

  void* ptr = calloc(count, size);
  
  if (ptr == NULL)
  {
    log_error("memtrace", "Allocation failed.");
    DEBUGBREAK();
    return NULL;
  }

  memtrace_alloc_t* alloc = (memtrace_alloc_t*)malloc(sizeof(memtrace_alloc_t));
  ASSERT(alloc != NULL);
  alloc->data.ptr = ptr;
  alloc->data.size = size;
  alloc->meta.alloc_kind = MEMTRACE_CALLOC;
  alloc->meta.file = file;
  alloc->meta.line = line;
  alloc->meta.func = func;
  alloc->meta.time = timer_now();

  memtrace_alloc_t* root = memtrace_alloc_root();

  alloc->next = root->next;
  root->next = alloc;

  g_memtrace_stat_total_alloc += size;
  g_memtrace_stat_cur_alloc += size;
  g_memtrace_stat_alloc_count++;

  if (g_memtrace_stat_peak_alloc < g_memtrace_stat_cur_alloc)
    g_memtrace_stat_peak_alloc = g_memtrace_stat_cur_alloc;

  return ptr;
}

void* memtrace_realloc(void* ptr, size_t size, const char* file, int line, const char* func)
{
  if (ptr == NULL)
    return memtrace_malloc(size, file, line, func);

  if (size == 0)
  {
    log_warn("memtrace", "Reallocating 0 bytes.");
    DEBUGBREAK();
    memtrace_free(ptr, file, line, func);
    return NULL;
  }

  memtrace_alloc_t* alloc = memtrace_alloc_root();
  alloc = alloc->next;

  for (; alloc != NULL; alloc = alloc->next)
    if (alloc->data.ptr == ptr)
      break;

  if (alloc == NULL)
  {
    log_error("memtrace", "Reallocating invalid memory: %p.", ptr);
    DEBUGBREAK();
    return NULL;
  }

  void* new_ptr = realloc(alloc->data.ptr, size);

  if (new_ptr == NULL)
  {
    log_warn("memtrace", "Reallocation failed.");
    DEBUGBREAK();
    return NULL;
  }

  g_memtrace_stat_cur_alloc += size - alloc->data.size;

  alloc->data.ptr = new_ptr;
  alloc->data.size = size;
  alloc->meta.alloc_kind = MEMTRACE_REALLOC;
  alloc->meta.file = file;
  alloc->meta.line = line;
  alloc->meta.func = func;

  uint64_t now = timer_now();
  g_memtrace_stat_total_lifetime += now - alloc->meta.time;

  alloc->meta.time = now;

  g_memtrace_stat_total_alloc += size;
  g_memtrace_stat_alloc_count++;
  g_memtrace_stat_dealloc_count++;

  if (g_memtrace_stat_peak_alloc < g_memtrace_stat_cur_alloc)
    g_memtrace_stat_peak_alloc = g_memtrace_stat_cur_alloc;

  return alloc->data.ptr;
}

void memtrace_free(void* ptr, const char* file, int line, const char* func)
{
  UNUSED(file);
  UNUSED(line);
  UNUSED(func);

  if (ptr == NULL)
    return;
  
  memtrace_alloc_t *alloc = memtrace_alloc_root(), *prev = alloc;
  alloc = alloc->next;

  for (; alloc != NULL; prev = alloc, alloc = alloc->next)
    if (alloc->data.ptr == ptr)
      break;

  if (alloc == NULL)
  {
    log_error("memtrace", "Deallocating invalid memory: %p.", ptr);
    DEBUGBREAK();
    return;
  }

  prev->next = alloc->next;

  g_memtrace_stat_total_lifetime += timer_now() - alloc->meta.time;
  g_memtrace_stat_cur_alloc -= alloc->data.size;
  g_memtrace_stat_dealloc_count++;
  
  free(alloc->data.ptr);
  free(alloc);
}

size_t memtrace_stat_total_alloc(void)
{
  return g_memtrace_stat_total_alloc;
}

size_t memtrace_stat_cur_alloc(void)
{
  return g_memtrace_stat_cur_alloc;
}

size_t memtrace_stat_peak_alloc(void)
{
  return g_memtrace_stat_peak_alloc;
}

size_t memtrace_stat_alloc_count(void)
{
  return g_memtrace_stat_alloc_count;
}

size_t memtrace_stat_avg_alloc_size(void)
{
  return memtrace_stat_total_alloc() / memtrace_stat_alloc_count();
}

double memtrace_stat_avg_lifetime(void)
{
  return ((double)g_memtrace_stat_total_lifetime / (double)g_memtrace_stat_dealloc_count) / (double)timer_freq() * 1000.0;
}
