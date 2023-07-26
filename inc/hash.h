/**
 * \file hash.h
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
 * \brief Hash value.
 */
typedef uint64_t hash_t;

/**
 * \brief Generates a hash code for the given data.
 *
 * \param[in] data Pointer to the data to be hashed.
 * \param[in] size The size of the data in bytes.
 * \returns The generated hash code.
 */
hash_t hash_digest(const void* data, size_t size);

#endif