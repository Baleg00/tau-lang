/**
 * \file
 *
 * \brief UNREACHABLE utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_UNREACHABLE_H
#define TAU_UNREACHABLE_H

#include <stdlib.h>

#include "utils/debugbreak.h"
#include "utils/io/log.h"

#ifdef TAU_DEBUG

/// Marks a code path as unreachable and triggers a fatal error.
# define UNREACHABLE()\
  do {\
    log_fatal("unreachable", "%s:%d", __FILE__, __LINE__);\
    DEBUGBREAK();\
    exit(EXIT_FAILURE);\
  } while (0)

#else

/// Marks a code path as unreachable and triggers a fatal error.
# define UNREACHABLE() ((void)0)

#endif

#endif
