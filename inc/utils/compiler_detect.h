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
# define TAU_COMPILER_CLANG
#endif

#if defined(_MSC_VER) && !defined(TAU_COMPILER_CLANG)
# define TAU_COMPILER_MSVC
#endif

#if defined(__GNUC__) && !defined(TAU_COMPILER_CLANG)
# define TAU_COMPILER_GCC
#endif

#if !defined(TAU_COMPILER_CLANG) && !defined(TAU_COMPILER_MSVC) && !defined(TAU_COMPILER_GCC)
# error "Unknown compiler!"
#endif

#endif
