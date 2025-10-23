/**
 * \file
 *
 * \brief TAU_DEBUGBREAK utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_DEBUGBREAK_H
#define TAU_DEBUGBREAK_H

#include "utils/compiler_detect.h"

#if TAU_COMPILER_MSVC && TAU_DEBUG

/// Breaks into a debugger for debugging purposes.
# define TAU_DEBUGBREAK() __debugbreak()

#else

/// Breaks into a debugger for debugging purposes.
# define TAU_DEBUGBREAK() ((void)0)

#endif

#endif
