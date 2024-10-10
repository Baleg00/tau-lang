/**
 * \file
 * 
 * \brief Set data structure interface.
 * 
 * \details A set data structure is a collection that stores a distinct group of
 * elements, ensuring that each element appears only once. It is designed to
 * provide efficient membership testing, insertion, and deletion operations.
 * Sets are commonly used to manage unique values or to test for the presence of
 * specific elements within a dataset.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_SET_H
#define TAU_SET_H

#include <stdbool.h>
#include <stdint.h>

#include "utils/extern_c.h"

TAU_EXTERN_C_BEGIN

/// Represents a set data structure.
typedef struct set_t set_t;

/// Function type used to compare elements in a set.
typedef int(*set_cmp_func_t)(const void*, const void*);

/// Utility function pointer type for `set_for_each` function.
typedef void(*set_for_each_func_t)(void*);

/**
 * \brief Initializes a new set with the provided comparison function.
 *
 * \param[in] cmp Comparison function used to order elements in the set.
 * \returns A pointer to the newly initialized set, or NULL on failure.
 */
set_t* set_init(set_cmp_func_t cmp);

/**
 * \brief Frees the resources associated with the set.
 *
 * \param[in] set Pointer to the set to be freed.
 */
void set_free(set_t* set);

/**
 * \brief Adds the specified data to the set.
 *
 * \param[in,out] set Pointer to the set.
 * \param[in] data Pointer to the data to be added.
 * \returns True if the data was added successfully, false otherwise.
 */
bool set_add(set_t* restrict set, void* restrict data);

/**
 * \brief Removes the specified data from the set.
 *
 * \param[in,out] set Pointer to the set.
 * \param[in] data Pointer to the data to be removed.
 * \returns True if the data was removed successfully, false otherwise.
 */
bool set_remove(set_t* restrict set, void* restrict data);

/**
 * \brief Retrieves the specified data from the set.
 *
 * \param[in] set Pointer to the set.
 * \param[in] data Pointer to the data to be retrieved.
 * \returns The data if it is present in the set, NULL otherwise.
 */
void* set_get(const set_t* restrict set, const void* restrict data);

/**
 * \brief Checks if the set contains the specified data.
 *
 * \param[in] set Pointer to the set.
 * \param[in] data Pointer to the data to be checked.
 * \returns True if the data is present in the set, false otherwise.
 */
bool set_contains(const set_t* restrict set, const void* restrict data);

/**
 * \brief Returns the minimum element in the set.
 *
 * \param[in] set Pointer to the set.
 * \returns A pointer to the minimum element, or NULL if the set is empty.
 */
void* set_min(const set_t* set);

/**
 * \brief Returns the maximum element in the set.
 *
 * \param[in] set Pointer to the set.
 * \returns A pointer to the maximum element, or NULL if the set is empty.
 */
void* set_max(const set_t* set);

/**
 * \brief Checks if a set is empty.
 *
 * \param[in] set Pointer to the set.
 * \returns `true` if the set is empty, `false` otherwise.
 */
bool set_empty(const set_t* set);

/**
 * \brief Returns the size of a set.
 *
 * \param[in] set Pointer to the set.
 * \returns The number of elements in the set.
 */
size_t set_size(const set_t* set);

/**
 * \brief Applies a given function to each element in the set.
 *
 * \param[in] set Pointer to the set.
 * \param[in] func The function to be applied to each element.
 */
void set_for_each(set_t* set, set_for_each_func_t func);

TAU_EXTERN_C_END

#endif
