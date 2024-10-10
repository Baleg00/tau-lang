/**
 * \file
 *
 * \brief MIN and MAX utility macros.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_MINMAX_H
#define TAU_MINMAX_H

#ifndef MIN

/**
 * \brief Returns the minimum value between two numbers.
 *
 * \param[in] A The first number.
 * \param[in] B The second number.
 * \returns The minimum value.
 */
# define MIN(A, B) ((A) < (B) ? (A) : (B))

#endif

#ifndef MAX

/**
 * \brief Returns the maximum value between two numbers.
 *
 * \param[in] A The first number.
 * \param[in] B The second number.
 * \returns The maximum value.
 */
# define MAX(A, B) ((A) > (B) ? (A) : (B))

#endif

#endif
