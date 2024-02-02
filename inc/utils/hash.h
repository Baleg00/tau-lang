/**
 * \file
 * 
 * \brief Hash function library interface.
 * 
 * \details Hash functions are algorithms that convert input data
 * (e.g., strings, numbers) into fixed-size values, known as hash values.
 * They are used in various applications, like data indexing, password storage,
 * and digital signatures, providing efficient data retrieval and comparison.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#ifndef TAU_HASH_H
#define TAU_HASH_H

#include <stdint.h>

/**
 * \brief Generates a hash value for a given data.
 *
 * \param[in] data Pointer to the data to be hashed.
 * \param[in] size The size of the data in bytes.
 * \returns The generated hash value.
 */
size_t hash_digest(const void* data, size_t size);

/**
 * \brief Combines a seed with the hash value of a given data.
 * 
 * \param seed The seed.
 * \param data Pointer to the data.
 * \param size Size of the data in bytes.
 * \returns The combined hash value.
 */
size_t hash_combine_with_data(size_t seed, const void* data, size_t size);

/**
 * \brief Combines a seed with a hash value.
 * 
 * \param seed The seed.
 * \param data Pointer to the data.
 * \param size Size of the data in bytes.
 * \returns The combined hash value.
 */
size_t hash_combine_with_hash(size_t seed, size_t hash);

#endif