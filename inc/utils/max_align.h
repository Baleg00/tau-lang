/**
 * \file
 *
 * \brief max_align_t utility type.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_MAX_ALIGN_H
#define TAU_MAX_ALIGN_H

#include <stddef.h>
#include <stdint.h>

#if defined(_MSC_VER) && !defined(__clang__)

/**
 * \brief A type whose alignment requirement is at least as strict
 * as that of every scalar type.
 *
 * \note MSVC doesn't conform to the C standard and doesn't provide
 * a definition for `max_align_t` so we make our own.
 */
typedef union
{
  int8_t i8;
  int16_t i16;
  int32_t i32;
  int64_t i64;
  intmax_t imax;
  float f;
  double d;
  long double ld;
  void* p;
} max_align_t;

#endif

#endif
