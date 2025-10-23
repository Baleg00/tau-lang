/**
 * \file
 *
 * \brief TAU_MIN and TAU_MAX utility macros.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_MINMAX_H
#define TAU_MINMAX_H

/**
 * \brief Returns the minimum value between two numbers.
 *
 * \param[in] A The first number.
 * \param[in] B The second number.
 * \returns The minimum value.
 */
#define TAU_MIN(A, B) ((A) < (B) ? (A) : (B))

/**
 * \brief Returns the maximum value between two numbers.
 *
 * \param[in] A The first number.
 * \param[in] B The second number.
 * \returns The maximum value.
 */
#define TAU_MAX(A, B) ((A) > (B) ? (A) : (B))

#endif
