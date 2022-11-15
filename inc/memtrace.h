#ifndef TAU_MEMTRACE_H
#define TAU_MEMTRACE_H

#include <stddef.h>

void* memtrace_malloc(size_t size, const char* file, int line, const char* func);
void* memtrace_calloc(size_t count, size_t size, const char* file, int line, const char* func);
void* memtrace_realloc(void* ptr, size_t size, const char* file, int line, const char* func);
void memtrace_free(void* ptr, const char* file, int line, const char* func);

#ifdef _DEBUG
# ifndef TAU_MEMTRACE_IMPL
#   define malloc(SIZE)        memtrace_malloc(SIZE, __FILE__, __LINE__, __func__)
#   define calloc(COUNT, SIZE) memtrace_calloc(COUNT, SIZE, __FILE__, __LINE__, __func__)
#   define realloc(PTR, SIZE)  memtrace_realloc(PTR, SIZE, __FILE__, __LINE__, __func__)
#   define free(PTR)           memtrace_free(PTR, __FILE__, __LINE__, __func__)
# endif
#endif

#endif