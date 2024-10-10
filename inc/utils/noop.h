/**
 * \file
 *
 * \brief NOOP utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_NOOP_H
#define TAU_NOOP_H

#ifndef NOOP

/// A statement that does nothing.
# define NOOP() do {} while (0)

#endif

#endif
