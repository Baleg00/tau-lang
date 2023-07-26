/**
 * \file hash.c
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
*/

#include "hash.h"

hash_t hash_digest(const void* data, size_t size)
{
  /**
   * FNV-1a hash
   * 
   * The FNV-1a hash function is a non-cryptographic hash function that produces
   * a 32-bit or 64-bit hash code from an input data sequence. It uses prime
   * numbers and bitwise operations to efficiently generate a hash value. The
   * algorithm processes the data sequentially and is well-suited for hashing
   * short strings and keys.
   * 
   * https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
  */
 
  hash_t h = 0xCBF29CE484222325ULL;

  for (size_t i = 0; i < size; ++i)
  {
    h ^= ((const uint8_t*)data)[i];
    h *= 0x00000100000001B3ULL;
  }

  return h;
}
