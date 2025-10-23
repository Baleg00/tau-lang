/**
 * \file
 *
 * \brief TAU_NORETURN utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_NORETURN_H
#define TAU_NORETURN_H

#include <stdnoreturn.h>

#include "utils/compiler_detect.h"

#ifdef noreturn

/// Tells the compiler that a function cannot return.
# define TAU_NORETURN noreturn

#elif TAU_COMPILER_MSVC

/// Tells the compiler that a function cannot return.
# define TAU_NORETURN __declspec(noreturn)

#elif TAU_COMPILER_CLANG || TAU_COMPILER_GCC

/// Tells the compiler that a function cannot return.
# define TAU_NORETURN __attribute__((noreturn))

#else

/// Tells the compiler that a function cannot return.
# define TAU_NORETURN

#endif

#endif
