#define TAU_MEMTRACE_IMPL
#include "memtrace.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"
#include "esc_seq.h"

static const char* MEMTRACE_ALLOC_KIND_STR[] = { "malloc", "calloc", "realloc" };

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
    printf("[" ESC_BG_GREEN "MEMTRACE" ESC_RESET "]> No memory leaks detected.\n");

  for (memtrace_alloc_t *alloc = root->next, *next; alloc != NULL; alloc = next)
  {
    next = alloc->next;

    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Leaked %zu bytes at %p allocated by %s.\n", 
      alloc->fileinfo.file, alloc->fileinfo.line,
      alloc->data.size, alloc->data.ptr, MEMTRACE_ALLOC_KIND_STR[alloc->fileinfo.alloc_kind]);
    __debugbreak();

    free(alloc->data.ptr);
    free(alloc);
  }
}

void* memtrace_malloc(size_t size, const char* file, int line, const char* func)
{
  if (size == 0)
  {
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Allocating 0 bytes using malloc is undefined behaviour.\n",
      file, line);
    __debugbreak();
  }

  void* ptr = malloc(size);
  
  if (ptr == NULL)
  {
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Could not allocate %zu bytes using malloc.\n",
      file, line, size);
    __debugbreak();
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
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Allocating 0 bytes using calloc is undefined behaviour.\n",
      file, line);
    __debugbreak();
  }

  void* ptr = calloc(count, size);
  
  if (ptr == NULL)
  {
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Could not allocate %zu bytes using calloc.\n",
      file, line, count * size);
    __debugbreak();
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
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Reallocating 0 bytes is undefined behaviour.\n",
      file, line);
    __debugbreak();
  }

  memtrace_alloc_t* alloc = memtrace_alloc_root();
  alloc = alloc->next;

  for (; alloc != NULL; alloc = alloc->next)
    if (alloc->data.ptr == ptr)
      break;

  if (alloc == NULL)
  {
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Reallocating unallocated memory at %p.\n",
      file, line, ptr);
    __debugbreak();
  }

  void* new_ptr = realloc(alloc->data.ptr, size);

  if (new_ptr == NULL)
  {
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Could not reallocate memory at %p to %zu bytes.\n",
      file, line, ptr, size);
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
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Calling free with a null pointer.\n",
      file, line);

   memtrace_alloc_t *alloc = memtrace_alloc_root(), *prev = alloc;
  alloc = alloc->next;

  for (; alloc != NULL; prev = alloc, alloc = alloc->next)
    if (alloc->data.ptr == ptr)
      break;

  if (alloc == NULL)
  {
    printf("[" ESC_BG_RED "MEMTRACE" ESC_RESET "]> %s:%d: Trying to free unallocated memory at %p.\n",
      file, line, ptr);
    __debugbreak();
  }

  prev->next = alloc->next;

  free(alloc->data.ptr);
  free(alloc);
}
