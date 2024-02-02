/**
 * \file null.h
 * 
 * \brief Null type descriptor interface.
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#ifndef TAU_TYPEDESC_NULL_H
#define TAU_TYPEDESC_NULL_H

#include "stages/analysis/types/typedesc/base.h"

/**
 * \brief Type descriptor for null types.
 */
typedef struct typedesc_null_t
{
  TYPEDESC_HEADER;
} typedesc_null_t;

/**
 * \brief Initializes a new null type descriptor.
 * 
 * \returns Pointer to the newly initialized type descriptor.
*/
typedesc_null_t* typedesc_null_init(void);

/**
 * \brief Frees all memory allocated by an null type descriptor.
 * 
 * \param[in] desc Pointer to the type descriptor to be freed.
 */
void typedesc_null_free(typedesc_null_t* desc);

#endif