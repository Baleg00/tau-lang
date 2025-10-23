/**
 * \file
 *
 * \brief TAU_ASSERT utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_ASSERT_H
#define TAU_ASSERT_H

#include <stdlib.h>

#include "utils/debugbreak.h"
#include "utils/io/log.h"

#if TAU_DEBUG

/**
 * \brief Asserts a condition and triggers a fatal error if the condition is `false`.
 *
 * \param[in] COND The condition to check.
 */
# define TAU_ASSERT(COND)\
  do {\
    if (!(COND)) {\
      tau_log_fatal("assert", "%s:%d Assertion failed: %s", __FILE__, __LINE__, #COND);\
      TAU_DEBUGBREAK();\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

#else

/**
 * \brief Asserts a condition and triggers a fatal error if the condition is `false`.
 *
 * \param[in] COND The condition to check.
 */
# define TAU_ASSERT(COND) ((void)(COND))

#endif

#endif
