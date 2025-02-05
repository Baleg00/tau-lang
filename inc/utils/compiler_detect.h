/**
 * \file
 *
 * \brief Compiler detection header.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_COMPILER_DETECT_H
#define TAU_COMPILER_DETECT_H

#if defined(__clang__)
# define TAU_COMPILER_CLANG 1
#else
# define TAU_COMPILER_CLANG 0
#endif

#if defined(_MSC_VER) && !TAU_COMPILER_CLANG
# define TAU_COMPILER_MSVC 1
#else
# define TAU_COMPILER_MSVC 0
#endif

#if defined(__GNUC__) && !TAU_COMPILER_CLANG
# define TAU_COMPILER_GCC 1
#else
# define TAU_COMPILER_GCC 0
#endif

#if !TAU_COMPILER_CLANG && !TAU_COMPILER_MSVC && !TAU_COMPILER_GCC
# error "Unknown compiler!"
#endif

#endif
