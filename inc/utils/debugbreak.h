/**
 * \file
 *
 * \brief DEBUGBREAK utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_DEBUGBREAK_H
#define TAU_DEBUGBREAK_H

#if TAU_DEBUG

# ifdef _MSC_VER

/// Breaks into a debugger for debugging purposes.
#   define DEBUGBREAK() __debugbreak()

# else

/// Breaks into a debugger for debugging purposes.
#   define DEBUGBREAK() ((void)0)

# endif

#else

/// Breaks into a debugger for debugging purposes.
# define DEBUGBREAK() ((void)0)

#endif

#endif
