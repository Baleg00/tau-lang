/**
 * \file
 *
 * \brief COUNTOF utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_COUNTOF_H
#define TAU_COUNTOF_H

#ifndef COUNTOF

/**
 * \brief Returns the number of elements in an array.
 *
 * \param[in] ARRAY The array.
 * \returns The number of elements in the array.
 */
# define COUNTOF(ARRAY) (sizeof((ARRAY)) / sizeof((ARRAY)[0]))

#endif

#endif
