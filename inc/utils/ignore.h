/**
 * \file
 *
 * \brief IGNORE utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_IGNORE_H
#define TAU_IGNORE_H

#include "utils/compiler_detect.h"

#if TAU_COMPILER_GCC

# define PUSH_IGNORED_WARN\
    _Pragma("GCC diagnostic push")\
    _Pragma("GCC diagnostic ignored \"-Wunused-result\"")

# define POP_IGNORED_WARN\
    _Pragma("GCC diagnostic pop")

#elif TAU_COMPILER_CLANG

# define PUSH_IGNORED_WARN\
    _Pragma("clang diagnostic push")\
    _Pragma("clang diagnostic ignored \"-Wunused-result\"")

# define POP_IGNORED_WARN\
    _Pragma("clang diagnostic pop")

#else

# define PUSH_IGNORED_WARN
# define POP_IGNORED_WARN

#endif

/**
 * \brief Discards the return value of an expression without raising warnings.
 *
 * \param[in] expr Expression whose return value should be explicitly discarded.
 */
#define IGNORE(expr)\
  do {\
    PUSH_IGNORED_WARN;\
    (void)(expr);\
    POP_IGNORED_WARN;\
  } while (0)

#endif
