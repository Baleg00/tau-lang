/**
 * \file
 *
 * \brief TAU_UNUSED utility macros.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_UNUSED_H
#define TAU_UNUSED_H

#include "utils/compiler_detect.h"

#if TAU_COMPILER_MSVC && !TAU_COMPILER_CLANG

/**
 * \brief Marks a parameter as unused to suppress warnings.
 *
 * \param[in] X The parameter's identifier.
 */
# define TAU_UNUSED(X) _Pragma("warning(suppress:4100)") unused_##X

#elif TAU_COMPILER_CLANG || TAU_COMPILER_GCC

/**
 * \brief Marks a parameter as unused to suppress warnings.
 *
 * \param[in] X The parameter's identifier.
 */
# define TAU_UNUSED(X) unused_##X __attribute__((__unused__))

#else

/**
 * \brief Marks a parameter as unused to suppress warnings.
 *
 * \param[in] X The parameter's identifier.
 */
# define TAU_UNUSED(X)

#endif

#endif
