/**
 * \file
 *
 * \brief ALIGNOF utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_ALIGNOF_H
#define TAU_ALIGNOF_H

#include <stdalign.h>

#include "utils/offsetof.h"

#if (defined(_MSC_VER) && defined(__alignof_is_defined)) || defined(__clang__) || defined(__GNUC__)

/**
 * \brief Returns the alignment requirement of a type.
 *
 * \param[in] TYPE The type.
 * \returns The alignment requirement of the type.
 */
# define ALIGNOF(TYPE) alignof(TYPE)

#else

/**
 * \brief Returns the alignment requirement of a type.
 *
 * \param[in] TYPE The type.
 * \returns The alignment requirement of the type.
 */
# define ALIGNOF(TYPE) OFFSETOF(struct { char c; TYPE t; }, t)

#endif

#endif
