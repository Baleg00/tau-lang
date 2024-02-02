/**
 * \file
 * 
 * \brief Memory tracing library interface.
 * 
 * \details The memory tracing library is a utility that helps in tracking and
 * managing memory allocations and deallocations in the program. It intercepts
 * the memory allocation and deallocation calls, allowing developers to monitor
 * and analyze memory usage at runtime. The memory tracing library can detect
 * memory leaks, identify excessive memory allocations, and provide insights
 * into memory usage patterns. It can also collect statistics and generate
 * reports to aid in debugging and optimizing memory usage. The memory tracing
 * library is a useful tool for diagnosing memory-related issues and improving
 * the overall memory management of the application.
 * 
 * \copyright Copyright (c) Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#ifndef TAU_MEMTRACE_H
#define TAU_MEMTRACE_H

#include <stddef.h>
#include <stdlib.h>

#ifdef TAU_DEBUG
# ifndef TAU_MEMTRACE_IMPL
/**
 * \brief Macro for memory allocation using memtrace_malloc.
 *
 * \param[in] SIZE The size of the memory to allocate.
 * \returns A pointer to the allocated memory.
 */
#   define malloc(SIZE) memtrace_malloc((SIZE), __FILE__, __LINE__, __func__)

/**
 * \brief Macro for array allocation using memtrace_calloc.
 *
 * \param[in] COUNT The number of elements in the array.
 * \param[in] SIZE The size of each element in the array.
 * \returns A pointer to the allocated memory.
 */
#   define calloc(COUNT, SIZE) memtrace_calloc((COUNT), (SIZE), __FILE__, __LINE__, __func__)

/**
 * \brief Macro for memory reallocation using memtrace_realloc.
 *
 * \param[in] PTR A pointer to the previously allocated memory block.
 * \param[in] SIZE The new size of the memory block.
 * \returns A pointer to the reallocated memory block.
 */
#   define realloc(PTR, SIZE) memtrace_realloc((PTR), (SIZE), __FILE__, __LINE__, __func__)

/**
 * \brief Macro for memory deallocation using memtrace_free.
 *
 * \param[in] PTR A pointer to the memory block to deallocate.
 */
#   define free(PTR) memtrace_free((PTR), __FILE__, __LINE__, __func__)
# endif
#endif

/**
 * \brief Allocates memory of the specified size and tracks the allocation
 * using memory tracing.
 *
 * \param[in] size The size of the memory to allocate.
 * \param[in] file The source file where the memory allocation occurs.
 * \param[in] line The line number in the source file where the memory allocation occurs.
 * \param[in] func The function name where the memory allocation occurs.
 * \returns A pointer to the allocated memory.
 */
void* memtrace_malloc(size_t size, const char* file, int line, const char* func);

/**
 * \brief Allocates memory for an array of elements and tracks the allocation
 * using memory tracing.
 *
 * \param[in] count The number of elements in the array.
 * \param[in] size The size of each element in the array.
 * \param[in] file The source file where the memory allocation occurs.
 * \param[in] line The line number in the source file where the memory allocation occurs.
 * \param[in] func The function name where the memory allocation occurs.
 * \returns A pointer to the allocated memory.
 */
void* memtrace_calloc(size_t count, size_t size, const char* file, int line, const char* func);

/**
 * \brief Changes the size of the memory block pointed to by `ptr` and tracks
 * the reallocation using memory tracing.
 *
 * \param[in] ptr A pointer to the previously allocated memory block.
 * \param[in] size The new size of the memory block.
 * \param[in] file The source file where the memory reallocation occurs.
 * \param[in] line The line number in the source file where the memory reallocation occurs.
 * \param[in] func The function name where the memory reallocation occurs.
 * \returns A pointer to the reallocated memory block.
 */
void* memtrace_realloc(void* ptr, size_t size, const char* file, int line, const char* func);

/**
 * \brief Deallocates the memory block pointed to by `ptr` and tracks the
 * deallocation using memory tracing.
 *
 * \param[in] ptr A pointer to the memory block to deallocate.
 * \param[in] file The source file where the memory deallocation occurs.
 * \param[in] line The line number in the source file where the memory deallocation occurs.
 * \param[in] func The function name where the memory deallocation occurs.
 */
void memtrace_free(void* ptr, const char* file, int line, const char* func);

/**
 * \brief Returns the total amount of memory allocated by the program.
 *
 * \returns The total size of memory allocated.
 */
size_t memtrace_stat_total_alloc(void);

/**
 * \brief Returns the current amount of memory used by the program.
 *
 * \returns The current memory usage.
 */
size_t memtrace_stat_cur_alloc(void);

/**
 * \brief Returns the peak amount of memory used by the program.
 *
 * \returns The peak memory usage.
 */
size_t memtrace_stat_peak_alloc(void);

/**
 * \brief Returns the total number of memory allocation requests.
 *
 * \returns The total count of memory allocations.
 */
size_t memtrace_stat_alloc_count(void);

/**
 * \brief Returns the average size of memory allocated per allocation.
 *
 * \return The average size of memory allocated.
 */
size_t memtrace_stat_avg_alloc_size(void);

/**
 * \brief Returns the average lifetime of allocated memory in milliseconds.
 *
 * \returns The average lifetime of allocated memory in milliseconds.
 */
double memtrace_stat_avg_lifetime(void);

#endif