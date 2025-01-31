/**
 * \file
 *
 * \brief SWAP utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_SWAP_H
#define TAU_SWAP_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef SWAP

/**
 * \brief Swaps the values of `a` and `b`.
 *
 * \param[in,out] a Value to be swapped.
 * \param[in,out] b Value to be swapped.
 */
# define SWAP(a, b)\
  do {\
    void* _a = &(a);\
    void* _b = &(b);\
    const size_t _size = sizeof(*(1 ? &(a) : &(b)));\
    uint8_t _temp[sizeof(*(1 ? &(a) : &(b)))];\
    memcpy(_temp, _a, _size);\
    memmove(_a, _b, _size);\
    memcpy(_b, _temp, _size);\
  } while (0)

#endif

#endif
