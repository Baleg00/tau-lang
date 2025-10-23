/**
 * \file
 *
 * \brief TAU_OFFSETOF utility macro.
 *
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_OFFSETOF_H
#define TAU_OFFSETOF_H

#include <stddef.h>

#ifdef offsetof

/**
 * \brief Returns the offset of a member within a structure.
 *
 * \param[in] TYPE The type of the structure.
 * \param[in] MEMBER The member within the structure.
 * \returns The offset of the member.
 */
# define TAU_OFFSETOF(TYPE, MEMBER) offsetof(TYPE, MEMBER)

#else

/**
 * \brief Returns the offset of a member within a structure.
 *
 * \param[in] TYPE The type of the structure.
 * \param[in] MEMBER The member within the structure.
 * \returns The offset of the member.
 */
# define TAU_OFFSETOF(TYPE, MEMBER) (size_t)(&((TYPE*)NULL)->MEMBER)

#endif

#endif
