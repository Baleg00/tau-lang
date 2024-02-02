/**
 * \file
 * 
 * \brief Array type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_ARRAY_H
#define TAU_TYPEDESC_ARRAY_H

#include "stages/analysis/types/typedesc/modif.h"

/**
 * \brief Type descriptor for array types.
 */
typedef struct typedesc_array_t
{
  TYPEDESC_MODIF_HEADER;
  size_t length; // Number of elements in the array.
} typedesc_array_t;

/**
 * \brief Initializes a new array type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_array_t* typedesc_array_init(void);

/**
 * \brief Frees all memory allocated by an array type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_array_free(typedesc_array_t* desc);

#endif