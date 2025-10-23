/**
 * \file
 * 
 * \brief Vector data structure interface.
 * 
 * \details A vector, also known as a dynamic array, is a resizable array-like
 * container that allows elements to be stored and accessed efficiently based on
 * their indices. Vectors dynamically adjust their size as elements are added or
 * removed, ensuring efficient memory usage and providing fast random access to
 * elements.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_VECTOR_H
#define TAU_VECTOR_H

#include "utils/common.h"

TAU_EXTERN_C_BEGIN

/**
 * \brief Macro for iterating over each element in a vector using a for loop.
 * 
 * \param[in] VAR The name of the index variable.
 * \param[in] VEC The vector to iterate over.
*/
#define TAU_VECTOR_FOR_LOOP(VAR, VEC)\
  for (\
    size_t (VAR) = 0;\
    (VAR) < ((VEC) == NULL ? 0 : tau_vector_size(VEC));\
    (VAR)++\
  )

/// Utility function pointer type for `tau_vector_for_each` function.
typedef void(*tau_vector_for_each_func_t)(void*);

/// Represents a vector (dynamic array).
typedef struct tau_vector_t tau_vector_t;

/**
 * \brief Initializes a new vector.
 *
 * \returns A pointer to the newly initialized vector.
 */
tau_vector_t* tau_vector_init(void);

/**
 * \brief Initializes a new vector with the specified capacity.
 *
 * \param[in] capacity The capacity of the vector.
 * \returns A pointer to the newly initialized vector.
 */
tau_vector_t* tau_vector_init_with_capacity(size_t capacity);

/**
 * \brief Initializes a new vector from a memory buffer.
 *
 * \param[in] buffer Pointer to the memory buffer.
 * \param[in] length The number of elements in the buffer.
 * \returns Pointer to the initialized vector.
 */
tau_vector_t* tau_vector_init_from_buffer(void* buffer, size_t length);

/**
 * \brief Frees the resources associated with the vector.
 *
 * \param[in] vec Pointer to the vector to be freed.
 */
void tau_vector_free(tau_vector_t* vec);

/**
 * \brief Creates a copy of a vector.
 *
 * \param[in] vec Pointer to the vector to copy.
 * \returns Pointer to the copied vector.
 */
tau_vector_t* tau_vector_copy(const tau_vector_t* vec);

/**
 * \brief Retrieves the element at the specified index.
 *
 * \param[in] vec Pointer to the vector.
 * \param[in] idx Index of the element to retrieve.
 * \returns A pointer to the element at the specified index.
 */
void* tau_vector_get(const tau_vector_t* vec, size_t idx);

/**
 * \brief Sets the element at the specified index.
 *
 * \param[in,out] vec Pointer to the vector.
 * \param[in] idx Index of the element to set.
 * \param[in] data Pointer to the data to set.
 */
void tau_vector_set(const tau_vector_t* restrict vec, size_t idx, void* restrict data);

/**
 * \brief Retrieves the data stored in the front of a vector.
 *
 * \param[in] vec Pointer to the vector.
 * \returns Pointer to the data at the front of the vector.
 */
void* tau_vector_front(const tau_vector_t* vec);

/**
 * \brief Retrieves the data stored in the back of a vector.
 *
 * \param[in] vec Pointer to the vector.
 * \returns Pointer to the data at the back of the vector.
 */
void* tau_vector_back(const tau_vector_t* vec);

/**
 * \brief Appends an element to the end of the vector.
 *
 * \param[in,out] vec Pointer to the vector.
 * \param[in] data Pointer to the data to be appended.
 */
void tau_vector_push(tau_vector_t* restrict vec, void* restrict data);

/**
 * \brief Removes and returns the last element of the vector.
 *
 * \param[in,out] vec Pointer to the vector.
 * \returns A pointer to the removed element.
 */
void* tau_vector_pop(tau_vector_t* vec);

/**
 * \brief Inserts an element at the specified index, shifting other elements to
 * the right.
 *
 * \param[in,out] vec Pointer to the vector.
 * \param[in] idx Index at which the element should be inserted.
 * \param[in] data Pointer to the data to be inserted.
 */
void tau_vector_insert(tau_vector_t* restrict vec, size_t idx, void* restrict data);

/**
 * \brief Appends all elements of a vector to another vector.
 *
 * \param[in,out] dest Pointer to the vector to extend.
 * \param[in] src Pointer to the vector to append.
 */
void tau_vector_extend(tau_vector_t* restrict dest, const tau_vector_t* restrict src);

/**
 * \brief Removes and returns the element at the specified index, shifting other
 * elements to the left.
 *
 * \param[in,out] vec Pointer to the vector.
 * \param[in] idx Index of the element to be removed.
 * \returns A pointer to the removed element.
 */
void* tau_vector_remove(tau_vector_t* vec, size_t idx);

/**
 * \brief Removes all elements from the vector.
 *
 * \param[in,out] vec Pointer to the vector.
 */
void tau_vector_clear(tau_vector_t* vec);

/**
 * \brief Finds the index of the first occurrence of a specified element.
 * 
 * \param[in] vec Pointer to the vector.
 * \param[in] data Pointer to the data to be found.
 * \returns The index of the element if it is present, `tau_vector_size(vec)` otherwise.
 */
size_t tau_vector_find(const tau_vector_t* restrict vec, void* restrict data);

/**
 * \brief Returns the number of elements in the vector.
 *
 * \param[in] vec Pointer to the vector.
 * \returns The number of elements in the vector.
 */
size_t tau_vector_size(const tau_vector_t* vec);

/**
 * \brief Checks if the vector is empty.
 *
 * \param[in] vec Pointer to the vector.
 * \returns `true` if the vector is empty, `false` otherwise.
 */
bool tau_vector_empty(const tau_vector_t* vec);

/**
 * \brief Applies a given function to each element of the vector.
 *
 * \param[in] vec Pointer to the vector.
 * \param[in] func The function to be applied to each element.
 */
void tau_vector_for_each(const tau_vector_t* vec, tau_vector_for_each_func_t func);

/**
 * \brief Writes every element of a vector into a memory buffer.
 * 
 * \note The memory buffer must be large enough to hold all elements, otherwise
 * a buffer overflow will occur.
 * 
 * \param vec Pointer to the vector.
 * \param buffer Pointer to the memory buffer.
 */
void tau_vector_to_buffer(const tau_vector_t* restrict vec, void* restrict buffer);

TAU_EXTERN_C_END

#endif
