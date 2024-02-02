/**
 * \file
 * 
 * \brief Enum type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_DECL_ENUM_H
#define TAU_TYPEDESC_DECL_ENUM_H

#include "stages/analysis/types/typedesc/decl.h"
#include "utils/collections/vector.h"

/**
 * \brief Type descriptor for enum types.
 */
typedef struct typedesc_enum_t
{
  TYPEDESC_DECL_HEADER;
} typedesc_enum_t;

/**
 * \brief Initializes a new enum type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_enum_t* typedesc_enum_init(void);

/**
 * \brief Frees all memory allocated by an enum type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_enum_free(typedesc_enum_t* desc);

#endif