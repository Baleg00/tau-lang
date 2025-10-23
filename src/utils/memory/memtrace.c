/**
 * \file
 * 
 * \copyright Copyright (c) Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#define TAU_MEMTRACE_IMPL
#include "utils/memory/memtrace.h"

#include "utils/common.h"
#include "utils/timer.h"

/**
 * \brief Enumeration of memory allocation kinds.
 */
typedef enum tau_memtrace_alloc_kind_t
{
  TAU_MEMTRACE_MALLOC,
  TAU_MEMTRACE_CALLOC,
  TAU_MEMTRACE_REALLOC,
} tau_memtrace_alloc_kind_t;

/**
 * \brief Allocation metadata.
 */
typedef struct tau_memtrace_meta_t
{
  tau_memtrace_alloc_kind_t alloc_kind; // Allcation kind.
  const char* file; // Path to source file.
  const char* func; // Name of containing function.
  int line; // Line number in source file.
  uint64_t time; // Time of allocation in ticks.
} tau_memtrace_meta_t;

/**
 * \brief Allocation data.
 */
typedef struct tau_memtrace_data_t
{
  void* ptr; // Pointer to memory.
  size_t size; // Size of allocation in bytes.
} tau_memtrace_data_t;

/**
 * \brief Allocation object.
 */
typedef struct tau_memtrace_alloc_t
{
  tau_memtrace_data_t data;
  tau_memtrace_meta_t meta;

  struct tau_memtrace_alloc_t *next; // Pointer to next allocation.
} tau_memtrace_alloc_t;

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
void tau_memtrace_atexit(void);

/**
 * \brief Returns the root allocation object.
 */
tau_memtrace_alloc_t* tau_memtrace_alloc_root(void)
{
  static tau_memtrace_alloc_t* root = NULL;

  // Initialize root.
  if (root == NULL)
  {
    root = (tau_memtrace_alloc_t*)calloc(1, sizeof(tau_memtrace_alloc_t));
    atexit(tau_memtrace_atexit);
  }

  return root;
}

void tau_memtrace_atexit(void)
{
  tau_memtrace_alloc_t* root = tau_memtrace_alloc_root();

  tau_log_debug("memtrace", "Total allocated memory: %zu bytes", tau_memtrace_stat_total_alloc());
  tau_log_debug("memtrace", "Peak allocated memory: %zu bytes", tau_memtrace_stat_peak_alloc());
  tau_log_debug("memtrace", "Total allocation count: %zu", tau_memtrace_stat_alloc_count());
  tau_log_debug("memtrace", "Average allocation size: %zu bytes", tau_memtrace_stat_avg_alloc_size());
  tau_log_debug("memtrace", "Average lifetime: %.6g ms", tau_memtrace_stat_avg_lifetime());

  if (root->next == NULL)
    tau_log_debug("memtrace", "No leaks detected.");
  else
  {
    tau_log_debug("memtrace", "Leaks detected!");
    TAU_DEBUGBREAK();
  }

  for (tau_memtrace_alloc_t *alloc = root->next, *next; alloc != NULL; alloc = next)
  {
    next = alloc->next;

    tau_log_debug("memtrace", "%zu bytes at %p (%s:%d)", alloc->data.size, alloc->data.ptr, alloc->meta.file, alloc->meta.line);

    free(alloc->data.ptr);
    free(alloc);
  }
}

void* tau_memtrace_malloc(size_t size, const char* file, int line, const char* func)
{
  if (size == 0)
  {
    tau_log_warn("memtrace", "Allocating 0 bytes.");
    TAU_DEBUGBREAK();
    return NULL;
  }

  void* ptr = malloc(size);

  if (ptr == NULL)
  {
    tau_log_error("memtrace", "Allocation failed.");
    TAU_DEBUGBREAK();
    return NULL;
  }

  tau_memtrace_alloc_t* alloc = (tau_memtrace_alloc_t*)malloc(sizeof(tau_memtrace_alloc_t));
  TAU_ASSERT(alloc != NULL);
  alloc->data.ptr = ptr;
  alloc->data.size = size;
  alloc->meta.alloc_kind = TAU_MEMTRACE_MALLOC;
  alloc->meta.file = file;
  alloc->meta.line = line;
  alloc->meta.func = func;
  alloc->meta.time = tau_timer_now();

  tau_memtrace_alloc_t* root = tau_memtrace_alloc_root();

  alloc->next = root->next;
  root->next = alloc;

  g_memtrace_stat_total_alloc += size;
  g_memtrace_stat_cur_alloc += size;
  g_memtrace_stat_alloc_count++;

  if (g_memtrace_stat_peak_alloc < g_memtrace_stat_cur_alloc)
    g_memtrace_stat_peak_alloc = g_memtrace_stat_cur_alloc;

  return ptr;
}

void* tau_memtrace_calloc(size_t count, size_t size, const char* file, int line, const char* func)
{
  if (count * size == 0)
  {
    tau_log_warn("memtrace", "Allocating 0 bytes.", file, line);
    TAU_DEBUGBREAK();
    return NULL;
  }

  void* ptr = calloc(count, size);

  if (ptr == NULL)
  {
    tau_log_error("memtrace", "Allocation failed.");
    TAU_DEBUGBREAK();
    return NULL;
  }

  tau_memtrace_alloc_t* alloc = (tau_memtrace_alloc_t*)malloc(sizeof(tau_memtrace_alloc_t));
  TAU_ASSERT(alloc != NULL);
  alloc->data.ptr = ptr;
  alloc->data.size = size;
  alloc->meta.alloc_kind = TAU_MEMTRACE_CALLOC;
  alloc->meta.file = file;
  alloc->meta.line = line;
  alloc->meta.func = func;
  alloc->meta.time = tau_timer_now();

  tau_memtrace_alloc_t* root = tau_memtrace_alloc_root();

  alloc->next = root->next;
  root->next = alloc;

  g_memtrace_stat_total_alloc += size;
  g_memtrace_stat_cur_alloc += size;
  g_memtrace_stat_alloc_count++;

  if (g_memtrace_stat_peak_alloc < g_memtrace_stat_cur_alloc)
    g_memtrace_stat_peak_alloc = g_memtrace_stat_cur_alloc;

  return ptr;
}

void* tau_memtrace_realloc(void* ptr, size_t size, const char* file, int line, const char* func)
{
  if (ptr == NULL)
    return tau_memtrace_malloc(size, file, line, func);

  if (size == 0)
  {
    tau_log_warn("memtrace", "Reallocating 0 bytes.");
    TAU_DEBUGBREAK();
    tau_memtrace_free(ptr, file, line, func);
    return NULL;
  }

  tau_memtrace_alloc_t* alloc = tau_memtrace_alloc_root();
  alloc = alloc->next;

  for (; alloc != NULL; alloc = alloc->next)
    if (alloc->data.ptr == ptr)
      break;

  if (alloc == NULL)
  {
    tau_log_error("memtrace", "Reallocating invalid memory: %p.", ptr);
    TAU_DEBUGBREAK();
    return NULL;
  }

  void* new_ptr = realloc(alloc->data.ptr, size);

  if (new_ptr == NULL)
  {
    tau_log_warn("memtrace", "Reallocation failed.");
    TAU_DEBUGBREAK();
    return NULL;
  }

  g_memtrace_stat_cur_alloc += size - alloc->data.size;

  alloc->data.ptr = new_ptr;
  alloc->data.size = size;
  alloc->meta.alloc_kind = TAU_MEMTRACE_REALLOC;
  alloc->meta.file = file;
  alloc->meta.line = line;
  alloc->meta.func = func;

  uint64_t now = tau_timer_now();
  g_memtrace_stat_total_lifetime += now - alloc->meta.time;

  alloc->meta.time = now;

  g_memtrace_stat_total_alloc += size;
  g_memtrace_stat_alloc_count++;
  g_memtrace_stat_dealloc_count++;

  if (g_memtrace_stat_peak_alloc < g_memtrace_stat_cur_alloc)
    g_memtrace_stat_peak_alloc = g_memtrace_stat_cur_alloc;

  return alloc->data.ptr;
}

void tau_memtrace_free(void* ptr, const char* TAU_UNUSED(file), int TAU_UNUSED(line), const char* TAU_UNUSED(func))
{
  if (ptr == NULL)
    return;

  tau_memtrace_alloc_t *alloc = tau_memtrace_alloc_root(), *prev = alloc;
  alloc = alloc->next;

  for (; alloc != NULL; prev = alloc, alloc = alloc->next)
    if (alloc->data.ptr == ptr)
      break;

  if (alloc == NULL)
  {
    tau_log_error("memtrace", "Deallocating invalid memory: %p.", ptr);
    TAU_DEBUGBREAK();
    return;
  }

  prev->next = alloc->next;

  g_memtrace_stat_total_lifetime += tau_timer_now() - alloc->meta.time;
  g_memtrace_stat_cur_alloc -= alloc->data.size;
  g_memtrace_stat_dealloc_count++;

  free(alloc->data.ptr);
  free(alloc);
}

size_t tau_memtrace_stat_total_alloc(void)
{
  return g_memtrace_stat_total_alloc;
}

size_t tau_memtrace_stat_cur_alloc(void)
{
  return g_memtrace_stat_cur_alloc;
}

size_t tau_memtrace_stat_peak_alloc(void)
{
  return g_memtrace_stat_peak_alloc;
}

size_t tau_memtrace_stat_alloc_count(void)
{
  return g_memtrace_stat_alloc_count;
}

size_t tau_memtrace_stat_avg_alloc_size(void)
{
  return tau_memtrace_stat_total_alloc() / tau_memtrace_stat_alloc_count();
}

double tau_memtrace_stat_avg_lifetime(void)
{
  return (double)g_memtrace_stat_total_lifetime / (double)g_memtrace_stat_dealloc_count / (double)tau_timer_freq() * 1000.0;
}
