/**
 * \file vector.h
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

#include <stdbool.h>
#include <stdint.h>

/**
 * \brief Dynamic array-based vector data structure.
 */
typedef struct vector_t vector_t;

/**
 * \brief Initializes a new vector.
 *
 * \returns A pointer to the newly initialized vector.
 */
vector_t* vector_init(void);

/**
 * \brief Initializes a new vector with the specified capacity.
 *
 * \param[in] capacity The capacity of the vector.
 * \returns A pointer to the newly initialized vector.
 */
vector_t* vector_init_with_capacity(size_t capacity);

/**
 * \brief Initializes a new vector from a memory buffer.
 *
 * \param[in] buffer Pointer to the memory buffer.
 * \param[in] length The number of elements in the buffer.
 * \returns Pointer to the initialized vector.
 */
vector_t* vector_init_from_buffer(void* buffer, size_t length);

/**
 * \brief Frees the resources associated with the vector.
 *
 * \param[in] vec Pointer to the vector to be freed.
 */
void vector_free(vector_t* vec);

/**
 * \brief Retrieves the element at the specified index.
 *
 * \param[in] vec Pointer to the vector.
 * \param[in] idx Index of the element to retrieve.
 * \returns A pointer to the element at the specified index.
 */
void* vector_get(vector_t* vec, size_t idx);

/**
 * \brief Sets the element at the specified index.
 *
 * \param[in,out] vec Pointer to the vector.
 * \param[in] idx Index of the element to set.
 * \param[in] data Pointer to the data to set.
 */
void vector_set(vector_t* vec, size_t idx, void* data);

/**
 * \brief Appends an element to the end of the vector.
 *
 * \param[in,out] vec Pointer to the vector.
 * \param[in] data Pointer to the data to be appended.
 */
void vector_push(vector_t* vec, void* data);

/**
 * \brief Removes and returns the last element of the vector.
 *
 * \param[in,out] vec Pointer to the vector.
 * \returns A pointer to the removed element.
 */
void* vector_pop(vector_t* vec);

/**
 * \brief Inserts an element at the specified index, shifting other elements to
 * the right.
 *
 * \param[in,out] vec Pointer to the vector.
 * \param[in] idx Index at which the element should be inserted.
 * \param[in] data Pointer to the data to be inserted.
 */
void vector_insert(vector_t* vec, size_t idx, void* data);

/**
 * \brief Removes and returns the element at the specified index, shifting other
 * elements to the left.
 *
 * \param[in,out] vec Pointer to the vector.
 * \param[in] idx Index of the element to be removed.
 * \returns A pointer to the removed element.
 */
void* vector_remove(vector_t* vec, size_t idx);

/**
 * \brief Removes all elements from the vector.
 *
 * \param[in,out] vec Pointer to the vector.
 */
void vector_clear(vector_t* vec);

/**
 * \brief Returns the number of elements in the vector.
 *
 * \param[in] vec Pointer to the vector.
 * \returns The number of elements in the vector.
 */
size_t vector_size(vector_t* vec);

/**
 * \brief Checks if the vector is empty.
 *
 * \param[in] vec Pointer to the vector.
 * \returns `true` if the vector is empty, `false` otherwise.
 */
bool vector_empty(vector_t* vec);

/**
 * \brief Writes every element of a vector into a memory buffer.
 * 
 * \note The memory buffer must be large enough to hold all elements, otherwise
 * a buffer overflow will occur.
 * 
 * \param vec Pointer to the vector.
 * \param buffer Pointer to the memory buffer.
 */
void vector_to_buffer(vector_t* vec, void* buffer);

#endif