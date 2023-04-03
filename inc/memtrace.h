/**
 * \file
 * 
 * Memory tracing library.
*/

#ifndef TAU_MEMTRACE_H
#define TAU_MEMTRACE_H

#include <stddef.h>

/**
 * Allocates `size` bytes of memory.
 * 
 * \param[in] size Number of bytes to allocate.
 * \param[in] file Path to source file in which the call was made.
 * \param[in] line Line number on which the call was made.
 * \param[in] func Name of function in which the call was made.
 * \returns Pointer to the allocated memory.
*/
void* memtrace_malloc(size_t size, const char* file, int line, const char* func);

/**
 * Allocates memory for `count` objects of size `size` and initializes all bytes to zero.
 * 
 * \param[in] count Number of objects.
 * \param[in] size Size of an object in bytes.
 * \param[in] file Path to source file in which the call was made.
 * \param[in] line Line number on which the call was made.
 * \param[in] func Name of function in which the call was made.
 * \returns Pointer to the allocated memory.
*/
void* memtrace_calloc(size_t count, size_t size, const char* file, int line, const char* func);

/**
 * Reallocates a previously allocated memory.
 * 
 * \param[in] ptr Pointer to the memory to be reallocated.
 * \param[in] size New size of the memory in bytes.
 * \param[in] file Path to source file in which the call was made.
 * \param[in] line Line number on which the call was made.
 * \param[in] func Name of function in which the call was made.
 * \returns Pointer to the reallocated memory.
*/
void* memtrace_realloc(void* ptr, size_t size, const char* file, int line, const char* func);

/**
 * Deallocates a previously allocated memory.
 * 
 * \param[in] ptr Pointer to the memory to be deallocated.
 * \param[in] file Path to source file in which the call was made.
 * \param[in] line Line number on which the call was made.
 * \param[in] func Name of function in which the call was made.
*/
void memtrace_free(void* ptr, const char* file, int line, const char* func);

#ifdef _DEBUG
# ifndef TAU_MEMTRACE_IMPL
/**
 * Allocates `SIZE` bytes of memory.
 * 
 * \param[in] size Number of bytes to allocate.
 * \returns Pointer to the allocated memory.
*/
#   define malloc(SIZE)        memtrace_malloc(SIZE, __FILE__, __LINE__, __func__)

/**
 * Allocates memory for `COUNT` objects of size `SIZE` and initializes all bytes to zero.
 * 
 * \param[in] count Number of objects.
 * \param[in] size Size of an object in bytes.
 * \returns Pointer to the allocated memory.
*/
#   define calloc(COUNT, SIZE) memtrace_calloc(COUNT, SIZE, __FILE__, __LINE__, __func__)

/**
 * Reallocates a previously allocated memory.
 * 
 * \param[in] ptr Pointer to the memory to be reallocated.
 * \returns Pointer to the reallocated memory.
*/
#   define realloc(PTR, SIZE)  memtrace_realloc(PTR, SIZE, __FILE__, __LINE__, __func__)

/**
 * Deallocates a previously allocated memory.
 * 
 * \param[in] ptr Pointer to the memory to be deallocated.
*/
#   define free(PTR)           memtrace_free(PTR, __FILE__, __LINE__, __func__)
# endif
#endif

#endif