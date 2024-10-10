/**
 * \file
 *
 * \brief NORETURN utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_NORETURN_H
#define TAU_NORETURN_H

#include <stdnoreturn.h>

#ifdef noreturn

/// Tells the compiler that a function cannot return.
# define NORETURN noreturn

#elif defined(_MSC_VER)

/// Tells the compiler that a function cannot return.
# define NORETURN __declspec(noreturn)

#elif defined(__clang__) || defined(__GNUC__)

/// Tells the compiler that a function cannot return.
# define NORETURN __attribute__((noreturn))

#else

/// Tells the compiler that a function cannot return.
# define NORETURN

#endif

#endif
