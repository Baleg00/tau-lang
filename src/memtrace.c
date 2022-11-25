#define TAU_MEMTRACE_IMPL
#include "memtrace.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"
#include "log.h"
#include "esc_seq.h"

typedef enum memtrace_alloc_kind_e {
  MEMTRACE_MALLOC,
  MEMTRACE_CALLOC,
  MEMTRACE_REALLOC,
} memtrace_alloc_kind_t;

typedef struct memtrace_fileinfo_s {
  memtrace_alloc_kind_t alloc_kind;
  const char *file, *func;
  int line;
} memtrace_fileinfo_t;

typedef struct memtrace_data_s {
  void* ptr;
  size_t size;  
} memtrace_data_t;

typedef struct memtrace_alloc_s {
  memtrace_data_t data;
  memtrace_fileinfo_t fileinfo;

  struct memtrace_alloc_s *next;
} memtrace_alloc_t;

void memtrace_atexit(void);

memtrace_alloc_t* memtrace_alloc_root(void)
{
  static memtrace_alloc_t* root = NULL;

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

  if (root->next == NULL)
    log_debug("memtrace", "No leaks detected.");

  for (memtrace_alloc_t *alloc = root->next, *next; alloc != NULL; alloc = next)
  {
    next = alloc->next;

    log_error("memtrace", "%s:%d: %zu bytes leaked.", 
      alloc->fileinfo.file, alloc->fileinfo.line, alloc->data.size);
    debugbreak();

    free(alloc->data.ptr);
    free(alloc);
  }
}

void* memtrace_malloc(size_t size, const char* file, int line, const char* func)
{
  if (size == 0)
  {
    log_warn("memtrace", "%s:%d: Undefined behaviour: allocating 0 bytes.",
      file, line);
    debugbreak();
  }

  void* ptr = malloc(size);
  
  if (ptr == NULL)
  {
    log_error("memtrace", "%s:%d: Allocation failed.",
      file, line);
    debugbreak();
    return NULL;
  }

  memtrace_alloc_t* alloc = (memtrace_alloc_t*)malloc(sizeof(memtrace_alloc_t));
  assert(alloc != NULL);
  alloc->data.ptr = ptr;
  alloc->data.size = size;
  alloc->fileinfo.alloc_kind = MEMTRACE_MALLOC;
  alloc->fileinfo.file = file;
  alloc->fileinfo.line = line;
  alloc->fileinfo.func = func;

  memtrace_alloc_t* root = memtrace_alloc_root();

  alloc->next = root->next;
  root->next = alloc;

  return ptr;
}

void* memtrace_calloc(size_t count, size_t size, const char* file, int line, const char* func)
{
  if (count * size == 0)
  {
    log_warn("memtrace", "%s:%d: Undefined behaviour: allocating 0 bytes.",
      file, line);
    debugbreak();
  }

  void* ptr = calloc(count, size);
  
  if (ptr == NULL)
  {
    log_error("memtrace", "%s:%d: Allocation failed.",
      file, line);
    debugbreak();
    return NULL;
  }

  memtrace_alloc_t* alloc = (memtrace_alloc_t*)malloc(sizeof(memtrace_alloc_t));
  assert(alloc != NULL);
  alloc->data.ptr = ptr;
  alloc->data.size = size;
  alloc->fileinfo.alloc_kind = MEMTRACE_CALLOC;
  alloc->fileinfo.file = file;
  alloc->fileinfo.line = line;
  alloc->fileinfo.func = func;

  memtrace_alloc_t* root = memtrace_alloc_root();

  alloc->next = root->next;
  root->next = alloc;

  return ptr;
}

void* memtrace_realloc(void* ptr, size_t size, const char* file, int line, const char* func)
{
  if (ptr == NULL)
    return memtrace_malloc(size, file, line, func);

  if (size == 0)
  {
    log_warn("memtrace", "%s:%d: Undefined behaviour: reallocating 0 bytes.",
      file, line);
    debugbreak();
  }

  memtrace_alloc_t* alloc = memtrace_alloc_root();
  alloc = alloc->next;

  for (; alloc != NULL; alloc = alloc->next)
    if (alloc->data.ptr == ptr)
      break;

  if (alloc == NULL)
  {
    log_error("memtrace", "%s:%d: Reallocating invalid memory: %p.",
      file, line, ptr);
    debugbreak();
    return NULL;
  }

  void* new_ptr = realloc(alloc->data.ptr, size);

  if (new_ptr == NULL)
  {
    log_warn("memtrace", "%s:%d: Reallocation failed.",
      file, line);
    debugbreak();
    return NULL;
  }

  alloc->data.ptr = new_ptr;
  alloc->data.size = size;
  alloc->fileinfo.alloc_kind = MEMTRACE_REALLOC;
  alloc->fileinfo.file = file;
  alloc->fileinfo.line = line;
  alloc->fileinfo.func = func;

  return alloc->data.ptr;
}

void memtrace_free(void* ptr, const char* file, int line, const char* func)
{
  if (ptr == NULL)
    return;
  
  memtrace_alloc_t *alloc = memtrace_alloc_root(), *prev = alloc;
  alloc = alloc->next;

  for (; alloc != NULL; prev = alloc, alloc = alloc->next)
    if (alloc->data.ptr == ptr)
      break;

  if (alloc == NULL)
  {
    log_error("memtrace", "%s:%d: Freeing invalid memory: %p.",
      file, line, ptr);
    debugbreak();
    return;
  }

  prev->next = alloc->next;

  free(alloc->data.ptr);
  free(alloc);
}
